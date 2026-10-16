# MIKEY-010E — Simon Lights guided sequence pilot

**Status:** source-development candidate only. Hardware acceptance is **not performed**, and normal firmware keeps both guided Simon modes **SOON**.

**Stacking authority:** This branch depends on MIKEY-010D draft [PR #12](https://github.com/mrkcntrmn/mikey/pull/12), which depends on Binary guided draft [PR #11](https://github.com/mrkcntrmn/mikey/pull/11). No source promotion or merge is authorized by automated software tests alone.

## Learning outcome

Help the learner discover that a sequence is **stored information in a specific order**, not a collection of colors. An interactive pattern shows the first one to three *numbered* light positions and hides the final light; the learner predicts the missing last symbol, then sees the actual physical LED and reads a short explanation.

The device uses one-based light labels **1–5**. Simon PLAY's color order, randomized sequence, nonblocking playback, short-press input and long-press Home remain unchanged.

### Lessons and challenge cases

| Mode | Prefix on screen | Correct missing symbol | Explanation |
|---|---|---|---|
| LEARN | 1 - ? | Light 3 | ORDER IS STORED |
| LEARN | 5 - 2 - ? | Light 4 | STEP 3 COMES LAST |
| CHALLENGE | 4 - 2 - ? | Light 5 | SEQUENCE 4-2-5 |
| CHALLENGE | 3 - 1 - 4 - ? | Light 2 | SEQUENCE 3-1-4-2 |
| CHALLENGE | 5 - 3 - 1 - ? | Light 4 | SEQUENCE 5-3-1-4 |

These are *deliberately fixed teaching exercises*. They illustrate position and order, not a randomized memory assessment. Source tests verify each answer and progression. Avoid calling lesson completion a measure of long-term memory ability.

### Guided flow

1. **Predict:** show only the known prefix and five possible numbered light positions. Rotate the encoder clockwise/counterclockwise to choose; touch or short-press submits. No physical LEDs are lit. A wrong prediction displays TRY AGAIN while preserving the same stored sequence and question.
2. **Observe:** on a correct guess, reveal the missing last symbol, illuminate exactly the corresponding logical LED, and show the selected light position on the display.
3. **Explain:** leave the correct LED visible and display a concise explanation about stored order; press to continue.
4. **Complete:** clear physical LEDs; short-press or touch restarts the mode. A long encoder press (~900 ms) returns Home from any guided phase and clears the LEDs.

An input frame containing both press and touch advances **exactly once**. A frame containing rotary movement plus press rotates only, avoiding simultaneous change-and-submit. The source model is deterministic and reuses the existing, hardware-independent `SimonSequence` for storage and the expected final light.

## Development gates

- `MIKEY_ENABLE_SIMON_GUIDED_PREVIEW` defaults to **0** and only accepts **0 or 1**.
- A non-preview firmware build still offers **PLAY READY**, **LEARN SOON**, **CHALLENGE SOON** for Simon.
- A direct call to `SimonActivity::begin` in guided mode when preview is disabled must **fail closed** and clear LEDs.
- The Binary and Logic guided preview flags remain independent and default off.
- The actual accepted Simon PLAY flow, including its randomized sequence and timer transitions, remains separate and is natively regression-tested.
- Native CI checks both the model and the real activity with a fake display/LED/input surface, in Simon OFF, Simon ON only, and all-three-guided ON matrices.
- CI also builds default-off firmware, three-guided-preview firmware, the original Jackpot baseline, the offline terminal preview, and conservative 240×240 circular screen-layout tests for all guided frames.
- No new pins, network, storage, accounts, data collection, telemetry or child profile are introduced.

## Software-only terminal preview

See [MIKEY-010D](MIKEY-010D-LOGIC-GUIDED-PILOT.md) for the full g++ command, now including `SimonActivity.cpp`, `-DMIKEY_ENABLE_SIMON_GUIDED_PREVIEW=1`, and the latest branch. Launch the CLI and select **5** for Simon LEARN or **6** for Simon CHALLENGE. Use `a`/`d` to rotate and Enter/`p` or `t` to submit, `h` to Home.

The simulator prints the actual production text draws and logical LED cues in a terminal. It **does not** simulate pixel-perfect circles, real button interrupts, touchscreen geometry, NeoPixel timing or board wiring.

## Future physical acceptance — held until CrowPanel is available

- [ ] Record the exact feature head SHA, Arduino core/library versions, FQBN, preview flags and upload command; do not claim an unnamed build was tested.
- [ ] Default-off firmware: Simon LEARN/CHALLENGE display SOON and Simon PLAY retains the accepted behavior.
- [ ] Simon-only preview firmware: guided Simon READY, Binary/Logic still SOON, other activities unchanged.
- [ ] All-three preview firmware: Binary/Logic/Simon guided modes READY; all other guided modes SOON.
- [ ] LEARN prefix and numbered light selections match the source lesson table.
- [ ] Clockwise/counterclockwise direction, five-way wraparound, touch and short-press selection behave as intended.
- [ ] Wrong answer keeps the same case and does not light/reveal the missing symbol.
- [ ] Correct answer reveals and illuminates the *only* expected logical LED; explanation remains legible.
- [ ] Completion clears LEDs; replay begins at the first question.
- [ ] Long-press Home works at Predict/Observe/Explain/Complete and cannot double-submit.
- [ ] Text, five circles, selection highlight and bottom hints actually fit the round screen with appropriate contrast.
- [ ] Physical response is smooth; no stale LED or display state on exit or re-entry.
- [ ] Jackpot, Reaction, Simon PLAY, Dice, Binary PLAY, Logic PLAY and Debug regressions PASS.
- [ ] Capture owner-run PASS/FAIL for each criterion and any differing hardware build SHA.

**Promotion rule:** only after PRs #11 and #12 clear their respective acceptance and merge gates should this PR be rebased/retargeted onto accepted main and requalified. Do not enable modes by default without a separate authorized promotion.
