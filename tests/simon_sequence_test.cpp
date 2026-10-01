#include <assert.h>
#include <stdint.h>

#include "SimonSequence.h"

using namespace mikey;

static void testOrderedRound() {
  SimonSequence sequence;
  sequence.reset(2);
  assert(sequence.append(4));
  assert(sequence.append(1));

  sequence.beginInput();
  assert(sequence.length() == 3);
  assert(sequence.expected() == 2);
  assert(sequence.submit(2) == SimonSubmitResult::AcceptedContinue);
  assert(sequence.expected() == 4);
  assert(sequence.submit(4) == SimonSubmitResult::AcceptedContinue);
  assert(sequence.expected() == 1);
  assert(sequence.submit(1) == SimonSubmitResult::RoundComplete);
}

static void testMismatchPreservesSequenceForRetry() {
  SimonSequence sequence;
  sequence.reset(3);
  assert(sequence.append(0));

  sequence.beginInput();
  assert(sequence.submit(2) == SimonSubmitResult::Mismatch);
  assert(sequence.length() == 2);
  assert(sequence.at(0) == 3);
  assert(sequence.at(1) == 0);
  assert(sequence.inputIndex() == 0);

  sequence.beginInput();
  assert(sequence.submit(3) == SimonSubmitResult::AcceptedContinue);
  assert(sequence.submit(0) == SimonSubmitResult::RoundComplete);
}

static void testProgressiveGrowthAndCap() {
  SimonSequence sequence;
  sequence.reset(0);

  for (uint8_t i = 1; i < SimonSequence::kMaxLength; ++i) {
    assert(sequence.append(i));
  }

  assert(sequence.length() == SimonSequence::kMaxLength);
  const uint8_t last = sequence.at(SimonSequence::kMaxLength - 1);
  assert(!sequence.append(4));
  assert(sequence.length() == SimonSequence::kMaxLength);
  assert(sequence.at(SimonSequence::kMaxLength - 1) == last);
}

static void testSelectionWraparound() {
  uint8_t selected = 0;
  selected = SimonSequence::previousLight(selected);
  assert(selected == 4);
  selected = SimonSequence::nextLight(selected);
  assert(selected == 0);

  for (uint8_t i = 0; i < SimonSequence::kLightCount; ++i) {
    selected = SimonSequence::nextLight(selected);
  }
  assert(selected == 0);
}

static void testInputResetDoesNotChangeSequence() {
  SimonSequence sequence;
  sequence.reset(1);
  assert(sequence.append(2));

  sequence.beginInput();
  assert(sequence.submit(1) == SimonSubmitResult::AcceptedContinue);
  assert(sequence.inputIndex() == 1);

  sequence.beginInput();
  assert(sequence.inputIndex() == 0);
  assert(sequence.length() == 2);
  assert(sequence.at(0) == 1);
  assert(sequence.at(1) == 2);
}

int main() {
  testOrderedRound();
  testMismatchPreservesSequenceForRetry();
  testProgressiveGrowthAndCap();
  testSelectionWraparound();
  testInputResetDoesNotChangeSequence();
  return 0;
}
