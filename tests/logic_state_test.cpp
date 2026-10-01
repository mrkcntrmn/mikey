#include <assert.h>

#include "LogicState.h"

using namespace mikey;

static void setInputs(LogicState& state, bool a, bool b) {
  state.reset();
  if (a) state.toggleA();
  if (b) state.toggleB();
}

static void testAndTruthTable() {
  LogicState state;
  setInputs(state, false, false);
  assert(!state.output());

  setInputs(state, false, true);
  assert(!state.output());

  setInputs(state, true, false);
  assert(!state.output());

  setInputs(state, true, true);
  assert(state.output());
}

static void testOrTruthTable() {
  LogicState state;
  setInputs(state, false, false);
  state.nextRule();
  assert(!state.output());

  setInputs(state, false, true);
  state.nextRule();
  assert(state.output());

  setInputs(state, true, false);
  state.nextRule();
  assert(state.output());

  setInputs(state, true, true);
  state.nextRule();
  assert(state.output());
}

static void testNotATruthTableIgnoresB() {
  LogicState state;

  setInputs(state, false, false);
  state.nextRule();
  state.nextRule();
  assert(state.output());

  setInputs(state, false, true);
  state.nextRule();
  state.nextRule();
  assert(state.output());

  setInputs(state, true, false);
  state.nextRule();
  state.nextRule();
  assert(!state.output());

  setInputs(state, true, true);
  state.nextRule();
  state.nextRule();
  assert(!state.output());
}

static void testRuleCycleWrapsBothDirections() {
  LogicState state;
  state.reset();
  assert(state.rule() == LogicRule::And);

  state.nextRule();
  assert(state.rule() == LogicRule::Or);
  state.nextRule();
  assert(state.rule() == LogicRule::NotA);
  state.nextRule();
  assert(state.rule() == LogicRule::And);

  state.previousRule();
  assert(state.rule() == LogicRule::NotA);
  state.previousRule();
  assert(state.rule() == LogicRule::Or);
  state.previousRule();
  assert(state.rule() == LogicRule::And);
}

static void testInputToggles() {
  LogicState state;
  state.reset();

  assert(!state.a());
  assert(!state.b());

  state.toggleA();
  assert(state.a());
  state.toggleA();
  assert(!state.a());

  state.toggleB();
  assert(state.b());
  state.toggleB();
  assert(!state.b());
}

int main() {
  testAndTruthTable();
  testOrTruthTable();
  testNotATruthTableIgnoresB();
  testRuleCycleWrapsBothDirections();
  testInputToggles();
  return 0;
}
