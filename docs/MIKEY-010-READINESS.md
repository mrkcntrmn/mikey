# MIKEY-010 — Phase 1 Learning Readiness

**Status:** plan and mentor protocol only, started after seven PLAY activities were integrated on 2026-10-15. This document does **not** claim an educational assessment has been conducted. LEARN and CHALLENGE are still **SOON** in firmware.

## Purpose

Find out what the learner can **predict, observe and explain**, not how fast they can complete games. Use the existing Core Seven in a low-pressure session and respect the project learning model: **PREDICT → ACT → OBSERVE → EXPLAIN**. The mentor asks short questions; the device remains the primary learning tool. No age, name, identifiers, accounts, performance telemetry or online profiles are needed in this public repository.

## Mentor session protocol

1. Start with five minutes of unstructured PLAY. The learner chooses the game.
2. Pick one or two concepts per session, not the entire seven-game checklist.
3. Ask a short prediction **before** the relevant device action. Permit wrong predictions.
4. Run one visible experiment, then ask what happened and why.
5. Change only one thing if the answer is uncertain, then observe again.
6. Stop if it stops being fun; capture only anonymous, optional mentor notes away from this public repository.

Suggested rating for each observable claim: **Independent**, **With a prompt**, **Not yet demonstrated**, or **Not tried**. Do not use exam scores, streaks, punitive failure states or claims about ability from one session.

## Seven observable concept checks

| Activity | Prediction or challenge (spoken or device-assisted) | Explanation evidence |
|---|---|---|
| Jackpot Wheel | Predict the change when speed increases; stop and compare the position to the jackpot at the top | Distinguishes physical LEDs switching from one moving light; identifies input (tap), processing (position comparison) and output (WIN/MISS) |
| Reaction Racer | Predict what happens if pressed before the GO cue; then wait for GO | Describes timing in milliseconds, an early input/false start, and repeatable measurement |
| Simon Lights | Predict whether changing the second light changes the sequence; reproduce three steps | Explains that order matters and the device remembers a sequence when LEDs are off |
| Digital Dice | Predict whether the next roll must match the last; try several rolls | Explains outcomes 1–6 and why short runs do not have to be evenly distributed |
| Binary Lab | Predict which LEDs light for decimal 5 or 10 | Connects `5 = 0101` and `10 = 1010` to 8/4/2/1 places; understands ON/OFF as bits |
| Logic Lab | Set A=1, B=0; predict AND and OR outputs, then NOT A | Explains AND needs both inputs, OR either input, and NOT A ignores B |
| Debug Detective | Read an expected/observed mismatch and choose a hypothesis before testing | Names the mismatch, proposes a possible cause, and says what evidence would confirm or reject it |

**Readiness signal:** the learner can explain representative concepts with a device demonstration, including input/output, stored sequence, binary place values, a Boolean rule, and one debugging hypothesis. Record support needed; do not require every check independently or force IDE graduation on a calendar date.

## LEARN mode design direction (not yet implemented)

- One idea at a time using the **already accepted** PLAY activity; no unrelated mini-app.
- First prompt asks for a prediction; action and a clear observed result follow.
- Feedback describes **what changed and why**, without only saying "wrong".
- Fit text and controls to a 240×240 circular display, with high contrast and visible position/number cues alongside colors.
- Preserve encoder rotation, single-press selection, screen-tap behavior and ~900 ms long-press Home.
- No network, cloud services, accounts, leaderboards or personal-data collection.
- Provide an immediate route back to PLAY; never turn PLAY into a mandatory quiz.

## CHALLENGE mode design direction (not yet implemented)

- Every challenge has a deterministic or objectively checkable answer, described in ordinary language.
- Start with **Discover**, then **Predict**, **Explain** and **Debug** rather than simply increasing animation speed.
- A wrong attempt preserves the learner's place, gives actionable feedback, and permits another try.
- Use model logic that can be tested without GPIO or display hardware.
- Separate source PRs and physical acceptance for each new mode; regress all seven existing PLAY activities.

## Candidate development sequence after this review

1. **MIKEY-010A — Readiness observation:** conduct at least one low-pressure mentor session. Identify which concepts need examples or scaffolding; do not post identifiable observations publicly.
2. **MIKEY-010B — Teaching UI contract:** define a reusable prompt/result/feedback pattern compatible with the small round display, touch and encoder. Build mockups/tests only; preserve existing app-shell navigation.
3. **MIKEY-010C — Guided pilot:** implement LEARN + CHALLENGE for one simple activity (recommended: Binary Lab because inputs/outputs are deterministic) behind explicit mode readiness, with native tests and physical acceptance.
4. **MIKEY-010D — Expand based on evidence:** use the pilot to inform the other activities, one independently accepted change at a time.
5. **MIKEY-010E — IDE readiness decision:** only consider one-variable code-reveal exercises after the learner can explain relevant behavior, and only with mentor consent.

These are **proposed work packages**, not claims of completed modes or a commitment to deliver every mode in a single code change.

## Definition of ready to implement MIKEY-010C

- Mentor and curriculum agree on the first concept and wording.
- Mode stays gated as SOON until its own test/physical release gate passes.
- Input mapping and long-press Home are documented and tested.
- Deterministic model unit tests, firmware compile and Jackpot oracle compile are green.
- Physical test proves no regressions to all seven PLAY modes.
- The implementation requires no new board pins or network capability.
