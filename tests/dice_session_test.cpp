#include <assert.h>
#include <stdint.h>

#include "DiceSession.h"

using namespace mikey;

static void testFreshSessionIsEmpty() {
  DiceSession session;
  assert(session.totalRolls() == 0);
  for (uint8_t face = 1; face <= DiceSession::kFaceCount; ++face) {
    assert(session.countFor(face) == 0);
  }
}

static void testRandomIndicesMapToSixFaces() {
  for (uint8_t index = 0; index < DiceSession::kFaceCount; ++index) {
    assert(DiceSession::faceFromRandomIndex(index) == index + 1);
  }
  assert(DiceSession::faceFromRandomIndex(6) == 1);
}

static void testRecordsAllSixFaces() {
  DiceSession session;
  for (uint8_t index = 0; index < DiceSession::kFaceCount; ++index) {
    const uint8_t face = DiceSession::faceFromRandomIndex(index);
    assert(session.record(face));
  }

  assert(session.totalRolls() == 6);
  for (uint8_t face = 1; face <= DiceSession::kFaceCount; ++face) {
    assert(session.countFor(face) == 1);
  }
}

static void testFrequencyCountsAccumulate() {
  DiceSession session;
  assert(session.record(4));
  assert(session.record(2));
  assert(session.record(4));
  assert(session.record(4));

  assert(session.totalRolls() == 4);
  assert(session.countFor(2) == 1);
  assert(session.countFor(4) == 3);
}

static void testInvalidFacesDoNotMutateSession() {
  DiceSession session;
  assert(!session.record(0));
  assert(!session.record(7));
  assert(session.totalRolls() == 0);
  assert(session.countFor(0) == 0);
  assert(session.countFor(7) == 0);
}

static void testResetClearsHistory() {
  DiceSession session;
  assert(session.record(1));
  assert(session.record(6));
  session.reset();

  assert(session.totalRolls() == 0);
  for (uint8_t face = 1; face <= DiceSession::kFaceCount; ++face) {
    assert(session.countFor(face) == 0);
  }
}

int main() {
  testFreshSessionIsEmpty();
  testRandomIndicesMapToSixFaces();
  testRecordsAllSixFaces();
  testFrequencyCountsAccumulate();
  testInvalidFacesDoNotMutateSession();
  testResetClearsHistory();
  return 0;
}
