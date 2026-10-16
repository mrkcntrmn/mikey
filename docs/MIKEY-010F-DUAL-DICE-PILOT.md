# MIKEY-010F — Dual Dice PLAY and guided probability pilot

**Status:** feature-branch SOURCE CANDIDATE, **not physically accepted**. This change includes an explicit owner-requested redesign: Digital Dice now rolls **TWO independent six-sided dice** per event, not one.

**Dependencies:** This source candidate is stacked on [MIKEY-010E Simon PR #13](https://github.com/mrkcntrmn/mikey/pull/13) → [MIKEY-010D Logic PR #12](https://github.com/mrkcntrmn/mikey/pull/12) → [MIKEY-010C Binary PR #11](https://github.com/mrkcntrmn/mikey/pull/11). Normal firmware on `main` remains unchanged. Do not merge out of sequence or claim this rewrite inherits the physical acceptance of the previous one-die PLAY mode.

## Dual Dice PLAY specification

- Show **two distinct pip faces**, left and right, side by side on the 240×240 round screen. Each face independently has 1–6 pips.
- Start both dice rolling on one short encoder press or screen tap. Use the existing **nine-frame nonblocking animation**; each animation frame makes **two separate bounded RNG calls** (`randomRange(0,6)`), not one reused face.
- After the last frame, **commit exactly one completed pair**. Calculate the total 2–12. `4 + 4 = 8` is visibly different from `1 + 6 = 7`.
- Show the **sum and how many times that sum occurred** this session. Session state also records per-die face frequencies and doubles count. The whole session resets on re-entry.
- Use the five existing physical LEDs: **0/1 = left die's face color**, **3/4 = right die's face color**, and **2 = cyan doubles indicator** only when both dice match. No new pins or wiring.
- A press/tap during the animation cannot start a second roll. A frame containing both short press and touch counts as **one event**. Long-press Home always overrides and clears LEDs. No network, accounts, cloud, telemetry, persistence, or child identifiers.
- The stable internal activity ID remains `ActivityId::Dice`, with visible label **DUAL DICE**.

The model `DualDiceSession` handles validity, sums, completed-pair counting, per-face counts, double-counting, and the exact number of ordered outcomes per sum. It does **not** claim random outcomes are predetermined or that a handful of observed rolls must show the theoretical frequencies. The unchanged historical single-die `DiceSession` file remains available for baseline tests.

## Educational LEARN / CHALLENGE proposal

**Important:** The question screens use explicitly labeled **FIXED SAMPLE** pairs. These samples illustrate a concept; they are **not** live random rolls. This avoids misleading a learner about randomness.

| Mode | Prediction question | Correct | Fixed illustrative pair | Explanation |
|---|---|---|---|---|
| LEARN | Can dice match? | YES | 4+4=8 | Doubles can happen |
| LEARN | Can total be 1? | NO | 1+1=2 | Minimum total is 2 |
| LEARN | Can total be 7? | YES | 3+4=7 | 3 plus 4 is 7 |
| CHALLENGE | Must total be 7? | NO | 2+4=6 | Other sums occur |
| CHALLENGE | Can total be 12? | YES | 6+6=12 | 6 plus 6 is 12 |
| CHALLENGE | Always even sum? | NO | 1+2=3 | 1 plus 2 is odd |
| CHALLENGE | More ways for 7 than 2? | YES | 3+4=7 | 7 has six ordered pairs; 2 has one |

A single fixed example may illustrate the stated mathematical concept without constituting a statistical experiment. In particular, the **"Must total be 7?"** case is a conceptual question; the sample of 2+5=7 is *one possibility*, not evidence that 7 must or must not recur. The explanation describes the full possible total range. For a probability observation, use real PLAY rolls and ask what is possible versus what must occur.

Guided mode follows PREDICT → OBSERVE → EXPLAIN → COMPLETE. Rotate to select YES/NO and submit with either encoder press or touchscreen. Wrong predictions stay on the same question and keep all LEDs dark. Correct answers reveal the fixed sample, show both dice and the LED mapping, explain the concept, and continue. Completion permits replay; Home works from any phase.

`MIKEY_ENABLE_DICE_GUIDED_PREVIEW` is independent of the existing Binary/Logic/Simon preview flags, defaults to **0**, and accepts only 0 or 1. Ordinary firmware keeps guided Dice LEARN/CHALLENGE at **SOON**. Direct guided entry also fails closed when the flag is not enabled.

## Automated software qualification

The following checks must PASS on the **exact PR HEAD**:

1. Enumerate all **36 ordered dice pairs**, confirm totals 2–12 and expected ways (1,2,3,4,5,6,5,4,3,2,1); test rejected invalid faces and resets.
2. Verify separate RNG calls for both dice per animation frame, nine-frame animation, one pair recorded per completed roll, sum/count presentation, doubles LED cue, long-press priority, and idle/re-entry behavior with the **real DiceActivity class and fake hardware**.
3. Validate every LEARN/CHALLENGE key, retries, non-leaking predictions, first corrected answer, progression, and replay with preview OFF, Dice-only ON, and all four guided previews ON.
4. Compile regular default-off Arduino firmware, all-four-preview firmware, immutable Jackpot baseline, and the legacy single-die model tests.
5. Run native Binary, Logic, Simon, Debug, Dice and other existing tests; check real guided frame text and pip circles in a conservative round-screen geometry approximation.
6. Compile and smoke-test the offline terminal controller with the new **7 = Dice LEARN / 8 = Dice CHALLENGE** options. The simulation is not a device screenshot or hardware verification.

## CrowPanel acceptance — deferred

- [ ] Identify **exact flashed SHA**, target ESP32 board configuration, preview flags, and actual upload evidence.
- [ ] Default-off build: PLAY says DUAL DICE, LEARN/CHALLENGE remain SOON.
- [ ] Both dice visibly appear fully inside round display with correct 1–6 pip patterns; sum is readable.
- [ ] Both dice animate and independently change (different faces must be possible); a roll finishes in the expected nonblocking time.
- [ ] A simultaneous press/tap is never double-counted; pressing during roll is ignored; encoder long press exits cleanly.
- [ ] Recorded sum counts increase once per completed pair and reset after re-entry.
- [ ] Left/right LED color groups match their respective dice; center LED lights only on doubles.
- [ ] Both Dice guided modes show expected question, FIXED SAMPLE cue, retry, answer and explanation when explicitly preview-enabled.
- [ ] All existing seven PLAY activities regress without crashes, stale lights, clipped text, or navigation failures.
- [ ] No default-on guided promotion, merge of stacked PRs, or new hardware sign-off without a separately recorded authorization.

**Decision:** Software development is permitted without a device; merging or releasing changed Dice PLAY and any new guided mode remains held for physical verification.
