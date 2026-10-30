#pragma once
// MIKEY-011: hardware-independent release policy. NOT wired to the ESP32 runtime.
// A future signed HTTPS client must authenticate metadata before calling this.
#include <cstdint>
#include <cstring>

namespace mikey {
namespace ota {

struct Version {
  uint32_t major = 0;
  uint32_t minor = 0;
  uint32_t patch = 0;
};

inline bool parseVersion(const char* value, Version* result) {
  if (!value || !result) return false;
  uint32_t fields[3] = {0, 0, 0};
  const char* ptr = value;
  for (unsigned field = 0; field < 3; ++field) {
    if (*ptr < '0' || *ptr > '9') return false;
    if (*ptr == '0' && ptr[1] >= '0' && ptr[1] <= '9') return false;
    unsigned digits = 0;
    uint32_t parsed = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      if (++digits > 7) return false;
      parsed = parsed * 10 + uint32_t(*ptr - '0');
      if (parsed > 1000000U) return false;
      ++ptr;
    }
    fields[field] = parsed;
    if (field != 2) {
      if (*ptr++ != '.') return false;
    }
  }
  if (*ptr != 0) return false;
  result->major = fields[0];
  result->minor = fields[1];
  result->patch = fields[2];
  return true;
}

inline int compareVersions(const Version& a, const Version& b) {
  if (a.major != b.major) return a.major < b.major ? -1 : 1;
  if (a.minor != b.minor) return a.minor < b.minor ? -1 : 1;
  if (a.patch != b.patch) return a.patch < b.patch ? -1 : 1;
  return 0;
}

inline bool isReleaseAllowed(
    const char* product, const char* hardwareId, const char* layoutId,
    const char* releaseChannel, const char* expectedChannel,
    const char* currentVersion, const char* candidateVersion,
    const char* minSupportedVersion, bool idle, bool manifestAuthenticated,
    bool imageSignatureVerified) {
  if (!product || !hardwareId || !layoutId || !releaseChannel || !expectedChannel) return false;
  if (std::strcmp(product, "mikey-tech-lab") != 0 ||
      std::strcmp(hardwareId, "crowpanel-1.28-esp32s3-16m") != 0 ||
      std::strcmp(layoutId, "mikey-16m-ota-v1") != 0 ||
      std::strcmp(releaseChannel, expectedChannel) != 0) return false;
  if (!idle || !manifestAuthenticated || !imageSignatureVerified) return false;
  Version current, candidate, minimum;
  if (!parseVersion(currentVersion, &current) ||
      !parseVersion(candidateVersion, &candidate) ||
      !parseVersion(minSupportedVersion, &minimum)) return false;
  // Only strictly newer releases. Existing local version must satisfy minimum.
  return compareVersions(candidate, current) > 0 &&
         compareVersions(current, minimum) >= 0;
}

}  // namespace ota
}  // namespace mikey
