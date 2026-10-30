#!/usr/bin/env python3
"""Offline-only MIKEY-011 signed OTA release packaging for Arduino-ESP32 3.3.8.

Never uploads firmware, reads credentials from the environment, or connects to a device.
Requires 'cryptography'. Uses the vendor-compatible 512-byte padded RSA-PSS trailer.
"""
import argparse
import base64
import hashlib
import json
import re
from pathlib import Path
from urllib.parse import urlsplit

from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa

EXPECTED_FINGERPRINT = "f7e95de7bc1e316ea8e3121815738eaf522d8cff0c50f6d877103d065e8e2265"
PRODUCT = "mikey-tech-lab"
HARDWARE = "crowpanel-1.28-esp32s3-16m"
PARTITION_LAYOUT = "mikey-16m-ota-v1"
TRAILER_BYTES = 512
SLOT_BYTES = 0x600000
MAX_SIGNED_BYTES = int(SLOT_BYTES * 0.80)
CHANNELS = {"canary", "stable"}


def canonical(payload):
    return json.dumps(payload, sort_keys=True, separators=(",", ":"), ensure_ascii=True).encode("ascii")


def hash_data(data):
    return hashlib.sha256(data).hexdigest()


def fail_unless(condition, message):
    if not condition:
        raise ValueError(message)


def public_fingerprint(key):
    der = key.public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
    return hash_data(der)


def load_public(path, expected_fingerprint):
    key = serialization.load_pem_public_key(Path(path).read_bytes())
    fail_unless(isinstance(key, rsa.RSAPublicKey) and key.key_size == 2048, "expected RSA-2048 public key")
    actual = public_fingerprint(key)
    fail_unless(actual == expected_fingerprint, "public key fingerprint mismatch")
    return key, actual


def pss():
    return padding.PSS(mgf=padding.MGF1(hashes.SHA256()), salt_length=padding.PSS.MAX_LENGTH)


def check_url(url):
    parsed = urlsplit(url)
    fail_unless(parsed.scheme == "https" and bool(parsed.netloc)
                and not parsed.username and not parsed.password and not parsed.fragment,
                "HTTPS URL without credentials/fragment required")
    return url


def sign_release(args):
    fail_unless(re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", args.version) is not None,
                "version must be numeric major.minor.patch")
    fail_unless(re.fullmatch(r"[a-fA-F0-9]{40}", args.build_sha) is not None,
                "build SHA must be a full 40-hex commit ID")
    fail_unless(args.channel in CHANNELS, "unknown channel")
    origin = check_url(args.origin).rstrip("/")
    public, fingerprint = load_public(args.public_key, args.expected_fingerprint)
    private = serialization.load_pem_private_key(Path(args.private_key).read_bytes(), password=None)
    fail_unless(isinstance(private, rsa.RSAPrivateKey) and private.key_size == 2048,
                "expected RSA-2048 private key")
    fail_unless(private.public_key().public_numbers() == public.public_numbers(), "key pair mismatch")
    data = Path(args.firmware).read_bytes()
    fail_unless(len(data) > 64 and data[0] == 0xE9, "invalid ESP32 application image header")
    fail_unless(len(data) + TRAILER_BYTES <= MAX_SIGNED_BYTES,
                "signed firmware exceeds reserved OTA release size")
    signature = private.sign(data, pss(), hashes.SHA256())
    fail_unless(len(signature) == 256, "unexpected RSA signature length")
    image = data + signature + bytes(TRAILER_BYTES - len(signature))
    image_name = f"MikeyTechLab-{args.version}-signed.bin"
    manifest_name = f"manifest-{args.version}.json"
    folder = f"releases/{args.channel}/{args.version}"
    payload = {
        "schema": 1, "product": PRODUCT, "hardware_id": HARDWARE,
        "partition_layout_id": PARTITION_LAYOUT, "channel": args.channel,
        "version": args.version, "min_supported_version": args.min_supported_version,
        "build_sha": args.build_sha.lower(), "image_url": f"{origin}/{folder}/{image_name}",
        "image_size": len(image), "sha256": hash_data(image),
        "signing_key_id": fingerprint,
    }
    manifest_sig = private.sign(canonical(payload), pss(), hashes.SHA256())
    doc = {"payload": payload, "signature_algorithm": "rsa-pss-sha256",
           "signature_b64": base64.b64encode(manifest_sig).decode("ascii")}
    output = Path(args.output_dir)
    fail_unless(output.exists() and output.is_dir(), "output directory must already exist")
    for name in (image_name, manifest_name):
        fail_unless(not (output / name).exists(), "refusing to overwrite existing release")
    (output / image_name).write_bytes(image)
    (output / manifest_name).write_bytes(canonical(doc) + b"\n")
    print(f"SIGNED_RELEASE=PREPARED IMAGE={output / image_name} MANIFEST={output / manifest_name}")
    print(f"RELEASE_SHA256={payload['sha256']} KEY_FINGERPRINT={fingerprint}")
    print("UPLOAD=NOT_PERFORMED DEVICE_FLASH=NOT_PERFORMED")


def verify_release(args):
    public, fingerprint = load_public(args.public_key, args.expected_fingerprint)
    document = json.loads(Path(args.manifest).read_text(encoding="utf-8"))
    fail_unless(set(document) == {"payload", "signature_algorithm", "signature_b64"},
                "unknown manifest envelope fields")
    fail_unless(document["signature_algorithm"] == "rsa-pss-sha256", "unsupported signature algorithm")
    payload = document["payload"]
    required = {"schema", "product", "hardware_id", "partition_layout_id", "channel", "version",
                "min_supported_version", "build_sha", "image_url", "image_size", "sha256", "signing_key_id"}
    fail_unless(isinstance(payload, dict) and set(payload) == required, "invalid manifest schema")
    signature = base64.b64decode(document["signature_b64"], validate=True)
    public.verify(signature, canonical(payload), pss(), hashes.SHA256())
    fail_unless(payload["schema"] == 1 and payload["product"] == PRODUCT
                and payload["hardware_id"] == HARDWARE
                and payload["partition_layout_id"] == PARTITION_LAYOUT
                and payload["channel"] in CHANNELS
                and payload["signing_key_id"] == fingerprint, "manifest policy mismatch")
    fail_unless(re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", payload["version"]) is not None,
                "invalid version")
    fail_unless(re.fullmatch(r"[a-f0-9]{40}", payload["build_sha"]) is not None,
                "invalid commit ID")
    check_url(payload["image_url"])
    image = Path(args.image).read_bytes()
    fail_unless(len(image) >= 64 + TRAILER_BYTES and len(image) <= MAX_SIGNED_BYTES,
                "invalid signed image length")
    fail_unless(payload["image_size"] == len(image) and payload["sha256"] == hash_data(image),
                "image checksum/size mismatch")
    fail_unless(urlsplit(payload["image_url"]).path.endswith("/" + Path(args.image).name),
                "manifest image filename mismatch")
    firmware, trailer = image[:-TRAILER_BYTES], image[-TRAILER_BYTES:]
    fail_unless(firmware[0] == 0xE9 and trailer[256:] == bytes(256),
                "invalid ESP32 image or RSA signature trailer")
    public.verify(trailer[:256], firmware, pss(), hashes.SHA256())
    print(f"RELEASE_VERIFY=PASS VERSION={payload['version']} CHANNEL={payload['channel']}")
    print("DEVICE_VERIFICATION=NOT_TESTED")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="action", required=True)
    sign = sub.add_parser("sign", help="Sign offline, never upload")
    sign.add_argument("--firmware", required=True)
    sign.add_argument("--private-key", required=True)
    sign.add_argument("--public-key", required=True)
    sign.add_argument("--output-dir", required=True)
    sign.add_argument("--origin", required=True, help="HTTPS origin (no trailing release path)")
    sign.add_argument("--version", required=True)
    sign.add_argument("--channel", choices=sorted(CHANNELS), required=True)
    sign.add_argument("--build-sha", required=True)
    sign.add_argument("--min-supported-version", default="0.0.0")
    verify = sub.add_parser("verify", help="Verify locally without private key")
    verify.add_argument("--manifest", required=True)
    verify.add_argument("--image", required=True)
    verify.add_argument("--public-key", required=True)
    for command in (sign, verify):
        command.add_argument("--expected-fingerprint", default=EXPECTED_FINGERPRINT)
    args = parser.parse_args()
    try:
        sign_release(args) if args.action == "sign" else verify_release(args)
    except Exception as exc:
        parser.exit(1, f"RELEASE_CHECK=FAIL {type(exc).__name__}: {exc}\n")


if __name__ == "__main__":
    main()
