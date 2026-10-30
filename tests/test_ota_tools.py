"""Source-only OTA tests. Ephemeral test RSA keys never represent the owner's key."""
import argparse
import copy
import json
from pathlib import Path
import tempfile
import unittest

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from tools.ota import preflight, sign_release


PARTITIONS = Path(__file__).resolve().parents[1] / "firmware/mikey-tech-lab/MikeyTechLab/partitions.csv"


class PartitionTests(unittest.TestCase):
    def test_geometry(self):
        result = preflight.validate(preflight.load_partitions(PARTITIONS), 1024 * 1024)
        self.assertEqual(result["slot_bytes"], 0x600000)

    def test_oversized_rejected(self):
        with self.assertRaises(preflight.PartitionError):
            preflight.validate(preflight.load_partitions(PARTITIONS), 0x600000)

    def test_overlapping_partitions_rejected(self):
        parts = copy.deepcopy(preflight.load_partitions(PARTITIONS))
        k, s, _, n = parts["ota_1"]
        parts["ota_1"] = (k, s, 0x20000, n)
        with self.assertRaises(preflight.PartitionError):
            preflight.validate(parts)

    def test_wrong_flash_size_rejected(self):
        parts = copy.deepcopy(preflight.load_partitions(PARTITIONS))
        k, s, _, n = parts["spiffs"]
        parts["spiffs"] = (k, s, 0xFF0000, n)
        with self.assertRaises(preflight.PartitionError):
            preflight.validate(parts)


class SigningTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        self.private = self.root / "test-private.pem"
        self.public = self.root / "test-public.pem"
        self.private.write_bytes(self.key.private_bytes(
            serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8,
            serialization.NoEncryption()))
        self.public.write_bytes(self.key.public_key().public_bytes(
            serialization.Encoding.PEM, serialization.PublicFormat.SubjectPublicKeyInfo))
        self.fingerprint = sign_release.public_fingerprint(self.key.public_key())
        self.firmware = self.root / "firmware.bin"
        self.firmware.write_bytes(bytes([0xE9]) + b"A" * 4095)
        self.sign_args = argparse.Namespace(
            version="0.0.1", build_sha="a" * 40, channel="canary",
            origin="https://ota.example.invalid", firmware=str(self.firmware),
            private_key=str(self.private), public_key=str(self.public),
            output_dir=str(self.root), min_supported_version="0.0.0",
            expected_fingerprint=self.fingerprint)
        self.image = self.root / "MikeyTechLab-0.0.1-signed.bin"
        self.manifest = self.root / "manifest-0.0.1.json"
        self.verify_args = argparse.Namespace(manifest=str(self.manifest),
            image=str(self.image), public_key=str(self.public),
            expected_fingerprint=self.fingerprint)

    def test_sign_verify_and_reject_tamper(self):
        sign_release.sign_release(self.sign_args)
        sign_release.verify_release(self.verify_args)
        image = bytearray(self.image.read_bytes())
        image[100] ^= 1
        self.image.write_bytes(image)
        with self.assertRaises(ValueError):
            sign_release.verify_release(self.verify_args)

    def test_manifest_signature_rejects_rewrite(self):
        sign_release.sign_release(self.sign_args)
        doc = json.loads(self.manifest.read_text())
        doc["payload"]["channel"] = "stable"
        self.manifest.write_text(json.dumps(doc))
        with self.assertRaises(Exception):
            sign_release.verify_release(self.verify_args)

    def test_wrong_expected_key_rejected(self):
        self.sign_args.expected_fingerprint = "0" * 64
        with self.assertRaises(ValueError):
            sign_release.sign_release(self.sign_args)

    def test_refuse_overwrite(self):
        sign_release.sign_release(self.sign_args)
        with self.assertRaises(ValueError):
            sign_release.sign_release(self.sign_args)

    def test_non_https_origin_rejected(self):
        self.sign_args.origin = "http://insecure.example"
        with self.assertRaises(ValueError):
            sign_release.sign_release(self.sign_args)


if __name__ == "__main__":
    unittest.main()
