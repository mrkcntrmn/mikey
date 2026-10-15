#include <assert.h>
#include <stdint.h>

#include "BinaryValue.h"

using namespace mikey;

static void testPlaceValuesAreEightFourTwoOne() {
  assert(BinaryValue::placeValue(0) == 8);
  assert(BinaryValue::placeValue(1) == 4);
  assert(BinaryValue::placeValue(2) == 2);
  assert(BinaryValue::placeValue(3) == 1);
  assert(BinaryValue::placeValue(4) == 0);
}

static void testKnownRepresentations() {
  BinaryValue value;

  value.set(0);
  assert(!value.bitAt(0));
  assert(!value.bitAt(1));
  assert(!value.bitAt(2));
  assert(!value.bitAt(3));

  value.set(5);  // 0101
  assert(!value.bitAt(0));
  assert(value.bitAt(1));
  assert(!value.bitAt(2));
  assert(value.bitAt(3));

  value.set(10);  // 1010
  assert(value.bitAt(0));
  assert(!value.bitAt(1));
  assert(value.bitAt(2));
  assert(!value.bitAt(3));

  value.set(15);
  for (uint8_t i = 0; i < BinaryValue::kBitCount; ++i) {
    assert(value.bitAt(i));
  }
}

static void testSetKeepsFourBits() {
  BinaryValue value;
  value.set(16);
  assert(value.value() == 0);
  value.set(31);
  assert(value.value() == 15);
}

static void testIncrementWrapsAtFifteen() {
  BinaryValue value;
  value.set(14);
  value.increment();
  assert(value.value() == 15);
  value.increment();
  assert(value.value() == 0);
}

static void testDecrementWrapsAtZero() {
  BinaryValue value;
  value.reset();
  value.decrement();
  assert(value.value() == 15);
  value.decrement();
  assert(value.value() == 14);
}

static void testInvalidBitPositionIsOff() {
  BinaryValue value;
  value.set(15);
  assert(!value.bitAt(4));
}

int main() {
  testPlaceValuesAreEightFourTwoOne();
  testKnownRepresentations();
  testSetKeepsFourBits();
  testIncrementWrapsAtFifteen();
  testDecrementWrapsAtZero();
  testInvalidBitPositionIsOff();
  return 0;
}
