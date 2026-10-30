#include <cassert>
#include <cstring>
#include "../firmware/mikey-tech-lab/MikeyTechLab/src/ota/OtaPolicy.h"

using mikey::ota::Version;
using mikey::ota::parseVersion;
using mikey::ota::compareVersions;
using mikey::ota::isReleaseAllowed;

static bool allowed(const char* product, const char* hardware,
                    const char* layout, const char* channel,
                    const char* current, const char* candidate,
                    const char* minimum, bool idle = true,
                    bool meta = true, bool signature = true) {
  return isReleaseAllowed(product, hardware, layout, channel, "canary",
      current, candidate, minimum, idle, meta, signature);
}

int main() {
  Version version;
  assert(parseVersion("0.1.1", &version));
  assert(version.major == 0 && version.minor == 1 && version.patch == 1);
  assert(!parseVersion("", &version));
  assert(!parseVersion("01.2.3", &version));
  assert(!parseVersion("1.2", &version));
  assert(!parseVersion("1.2.3beta", &version));
  assert(!parseVersion("4294967295.0.0", &version));
  Version one, two;
  assert(parseVersion("1.0.0", &one));
  assert(parseVersion("1.0.1", &two));
  assert(compareVersions(two, one) > 0);
  const char* prod = "mikey-tech-lab";
  const char* hw = "crowpanel-1.28-esp32s3-16m";
  const char* layout = "mikey-16m-ota-v1";
  assert(allowed(prod, hw, layout, "canary", "1.0.0", "1.0.1", "0.0.0"));
  assert(!allowed(prod, hw, layout, "canary", "1.0.1", "1.0.0", "0.0.0"));
  assert(!allowed(prod, hw, layout, "canary", "1.0.1", "1.0.1", "0.0.0"));
  assert(!allowed(prod, hw, layout, "canary", "1.0.0", "1.0.1", "1.0.1"));
  assert(!allowed("other", hw, layout, "canary", "1.0.0", "1.0.1", "0.0.0"));
  assert(!allowed(prod, "wrong", layout, "canary", "1.0.0", "1.0.1", "0.0.0"));
  assert(!allowed(prod, hw, "wrong", "canary", "1.0.0", "1.0.1", "0.0.0"));
  assert(!allowed(prod, hw, layout, "stable", "1.0.0", "1.0.1", "0.0.0"));
  assert(!allowed(prod, hw, layout, "canary", "1.0.0", "1.0.1", "0.0.0", false));
  assert(!allowed(prod, hw, layout, "canary", "1.0.0", "1.0.1", "0.0.0", true, false));
  assert(!allowed(prod, hw, layout, "canary", "1.0.0", "1.0.1", "0.0.0", true, true, false));
  return 0;
}
