#!/usr/bin/env python3
"""Export only the fingerprint-verified PUBLIC OTA key as an Arduino C++ header.

No private-key access or cloud operations. Intended for a later authorized
source change; do not write the private PEM into the repository.
"""
import argparse
from pathlib import Path
from tools.ota.sign_release import EXPECTED_FINGERPRINT, load_public

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--public-key", required=True, help="PEM containing only public key")
    p.add_argument("--output", required=True, help="New header path; never overwrites")
    args = p.parse_args()
    _, fingerprint = load_public(args.public_key, EXPECTED_FINGERPRINT)
    raw = Path(args.public_key).read_bytes()
    if not raw.startswith(b"-----BEGIN PUBLIC KEY-----"):
        p.exit(1, "Expected public-only SubjectPublicKeyInfo PEM\n")
    raw = raw.rstrip(b"\r\n") + b"\n\x00"
    dest = Path(args.output)
    if dest.exists():
        p.exit(1, "Output already exists. Refusing overwrite.\n")
    body = ", ".join(f"0x{byte:02x}" for byte in raw)
    data = (
        "#pragma once\n#include <cstddef>\n#include <cstdint>\n"
        "// Public key only. DER/SPKI SHA-256: " + fingerprint + "\n"
        "namespace mikey { namespace ota {\n"
        "static const uint8_t kOtaPublicKeyPem[] = {" + body + "};\n"
        "static constexpr size_t kOtaPublicKeyPemSize = sizeof(kOtaPublicKeyPem);\n"
        "}}  // namespace mikey::ota\n"
    )
    dest.write_text(data, encoding="utf-8")
    print(f"PUBLIC_HEADER=WRITTEN FINGERPRINT={fingerprint} PRIVATE_KEY=NOT_ACCESSED")


if __name__ == "__main__":
    main()
