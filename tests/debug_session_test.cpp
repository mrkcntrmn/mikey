#include <assert.h>
#include <stdint.h>

#include "DebugSession.h"

using namespace mikey;

static void chooseOption(DebugSession& session, uint8_t option) {
  while (session.selectedOption() != option) {
    session.nextOption();
  }
}

static void testAnswerKeyAlternates() {
  assert(DebugSession::correctOptionForCase(0) == 0);
  assert(DebugSession::correctOptionForCase(1) == 1);
  assert(DebugSession::correctOptionForCase(2) == 0);
  assert(DebugSession::correctOptionForCase(3) == 1);
  assert(DebugSession::correctOptionForCase(4) == 0);
}

static void testOptionSelectionWraps() {
  DebugSession session;
  session.reset();
  assert(session.selectedOption() == 0);

  session.nextOption();
  assert(session.selectedOption() == 1);
  session.nextOption();
  assert(session.selectedOption() == 0);

  session.previousOption();
  assert(session.selectedOption() == 1);
  session.previousOption();
  assert(session.selectedOption() == 0);
}

static void testIncorrectAnswerDoesNotAdvance() {
  DebugSession session;
  session.reset();
  chooseOption(session, 1);

  assert(session.submit() == DebugSubmitResult::Incorrect);
  assert(session.caseIndex() == 0);
  assert(session.solvedCount() == 0);
  assert(!session.complete());
}

static void testCorrectAnswersAdvanceCases() {
  DebugSession session;
  session.reset();

  for (uint8_t index = 0; index < DebugSession::kCaseCount - 1; ++index) {
    assert(session.caseIndex() == index);
    chooseOption(session, DebugSession::correctOptionForCase(index));
    assert(session.selectionIsCorrect());
    assert(session.submit() == DebugSubmitResult::CorrectContinue);
    assert(session.solvedCount() == index + 1);
    assert(session.caseIndex() == index + 1);
  }
}

static void testFinalCorrectAnswerCompletesSession() {
  DebugSession session;
  session.reset();

  for (uint8_t index = 0; index < DebugSession::kCaseCount; ++index) {
    chooseOption(session, DebugSession::correctOptionForCase(index));
    const DebugSubmitResult result = session.submit();
    if (index + 1 < DebugSession::kCaseCount) {
      assert(result == DebugSubmitResult::CorrectContinue);
    } else {
      assert(result == DebugSubmitResult::Complete);
    }
  }

  assert(session.complete());
  assert(session.solvedCount() == DebugSession::kCaseCount);
}

static void testResetStartsFirstCaseAgain() {
  DebugSession session;
  session.reset();
  chooseOption(session, DebugSession::correctOptionForCase(0));
  assert(session.submit() == DebugSubmitResult::CorrectContinue);

  session.reset();
  assert(session.caseIndex() == 0);
  assert(session.selectedOption() == 0);
  assert(session.solvedCount() == 0);
  assert(!session.complete());
}

int main() {
  testAnswerKeyAlternates();
  testOptionSelectionWraps();
  testIncorrectAnswerDoesNotAdvance();
  testCorrectAnswersAdvanceCases();
  testFinalCorrectAnswerCompletesSession();
  testResetStartsFirstCaseAgain();
  return 0;
}
