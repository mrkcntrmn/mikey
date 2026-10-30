#!/usr/bin/env python3
"""MIKEY-011 source-only 16 MB OTA partition and firmware-size preflight.

No USB access, flashing, signing, network access, or secret handling.
"""
import argparse
import csv
from pathlib import Path

FLASH_BYTES = 16 * 1024 * 1024
SIGNATURE_TRAILER = 512  # Arduino-ESP32 3.3.8 bin_signing.py fixed trailer
MAX_SLOT_FRACTION = 0.80  # reserve 20% for future firmware growth
LAYOUT_ID = "mikey-16m-ota-v1"


class PartitionError(ValueError):
    pass


def load_partitions(path):
    parts = {}
    with Path(path).open(newline="", encoding="utf-8") as handle:
        for line_number, row in enumerate(csv.reader(handle), 1):
            if not row or not row[0].strip() or row[0].lstrip().startswith("#"):
                continue
            if len(row) < 5:
                raise PartitionError(f"line {line_number}: five columns required")
            name, kind, subtype, offset, size = [cell.strip() for cell in row[:5]]
            if not name or name in parts:
                raise PartitionError(f"line {line_number}: empty/duplicate partition")
            try:
                start, length = int(offset, 0), int(size, 0)
            except ValueError as exc:
                raise PartitionError(f"line {line_number}: explicit numeric offsets and sizes required") from exc
            parts[name] = (kind, subtype, start, length)
    return parts


def validate(parts, firmware_size=None):
    required = {
        "nvs": ("data", "nvs"),
        "otadata": ("data", "ota"),
        "phy_init": ("data", "phy"),
        "ota_0": ("app", "ota_0"),
        "ota_1": ("app", "ota_1"),
        "spiffs": ("data", "spiffs"),
    }
    if set(parts) != set(required):
        raise PartitionError("unexpected partition inventory")
    intervals = []
    for name, (kind, subtype, start, size) in parts.items():
        if (kind, subtype) != required[name]:
            raise PartitionError(f"{name}: wrong type/subtype")
        if start < 0x9000 or size <= 0 or start + size > FLASH_BYTES:
            raise PartitionError(f"{name}: invalid range or beyond 16MB")
        alignment = 0x10000 if kind == "app" else 0x1000
        if start % alignment or size % 0x1000:
            raise PartitionError(f"{name}: misaligned offset/size")
        intervals.append((start, start + size, name))
    intervals.sort()
    for prev, curr in zip(intervals, intervals[1:]):
        if curr[0] < prev[1]:
            raise PartitionError(f"{prev[2]} overlaps {curr[2]}")
    if parts["otadata"][3] != 0x2000:
        raise PartitionError("otadata must be exactly two erase sectors")
    if parts["ota_0"][3] != parts["ota_1"][3]:
        raise PartitionError("OTA slots must have equal capacity")
    if parts["ota_0"][3] < 0x300000:
        raise PartitionError("OTA slot capacity is too small")
    if firmware_size is not None:
        if firmware_size <= 0:
            raise PartitionError("firmware binary is empty")
        signed_size = firmware_size + SIGNATURE_TRAILER
        allowed = int(parts["ota_0"][3] * MAX_SLOT_FRACTION)
        if signed_size > allowed:
            raise PartitionError(
                f"signed firmware {signed_size}B exceeds preflight limit {allowed}B "
                f"(slot {parts['ota_0'][3]}B)"
            )
    return {"layout_id": LAYOUT_ID, "slot_bytes": parts["ota_0"][3],
            "flash_bytes": FLASH_BYTES, "firmware_size": firmware_size}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--partitions", type=Path, required=True)
    p.add_argument("--firmware", type=Path, help="Unsigned application .bin (optional)")
    args = p.parse_args()
    size = args.firmware.stat().st_size if args.firmware else None
    result = validate(load_partitions(args.partitions), size)
    print(f"PARTITION_PREFLIGHT=PASS LAYOUT={result['layout_id']} "
          f"SLOT={result['slot_bytes']} FLASH={result['flash_bytes']} "
          f"UNSIGNED_BIN={size if size is not None else 'NOT_MEASURED'} "
          "DEVICE_ACCEPTANCE=PENDING")


if __name__ == "__main__":
    main()
