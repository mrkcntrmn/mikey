# MIKEY-010D — Logic Lab LEARN / CHALLENGE Source Pilot

**Status:** software-only stacked development candidate, **not physically accepted**.
**Dependency:** [MIKEY-010C / PR #11](https://github.com/mrkcntrmn/mikey/pull/11) Binary guided pilot. Do not merge this proposal ahead of its parent or imply that any guided mode is accepted on the CrowPanel.

## Goal

Teach Boolean decisions using existing Logic Lab hardware and the already-tested `LogicState` model. Demonstrate rule → input → output and allow predictable, evidence-based explanations. Keep accepted PLAY intact; do not add networking, cloud, telemetry, accounts, storage, or new GPIO mappings.

## Experiences

**LEARN** asks three short predictions:

| Rule | A | B | Correct output | Explanation |
|---|---|---|---|---|
| AND | 1 | 0 | 0 | AND NEEDS BOTH |
| OR | 1 | 0 | 1 | OR NEEDS EITHER |
| NOT A | 0 | 1 | 1 | NOT A FLIPS A |

**CHALLENGE** asks six more predictions, covering positive and negative cases of AND, OR and NOT A:

- AND(1,1) → 1; OR(0,0) → 0; NOT A(1,1) → 0.
- AND(0,1) → 0; OR(0,1) → 1; NOT A(0,0) → 1.

During **PREDICT**, show rule, A, B, and two outputs (0/OFF and 1/ON). Encoder rotation selects the prediction; encoder press or screen tap submits it exactly once. Wrong answers remain on the same case, display TRY AGAIN, and keep LEDs dark so physical cues do not leak the answer. Correct predictions advance to **OBSERVE**, lighting logical LED 0 for A, LED 1 for B, and LED 4 (green=1/red=0) for output. LED 2 and LED 3 remain dark. An explanation screen follows; then one next question or completion with optional replay. Long-press Home exits any phase.

The implementation uses a hardware-independent `LogicGuidedSession`, deriving expected values through the existing `LogicState` API instead of duplicating Boolean logic. No difficulty is artificially imposed through faster animations or punitive retries.

## Gate boundaries

Normal builds stay **SOON** for Binary and Logic guided modes. The independent compile-time gate `MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW` defaults to 0, rejects values other than 0/1, and leaves the accepted PLAY path unchanged. `LogicActivity::begin` fails closed if guided mode is invoked directly with the preview gate disabled.

The existing Binary pilot gate `MIKEY_ENABLE_BINARY_GUIDED_PREVIEW` remains independently default-OFF.

### Source checks (no hardware required)

The proposed workflow checks:

- All existing Core Seven native rule suites and Binary guided regression tests.
- Logic guided model and real activity rendering with fake `Hardware`, for (Binary OFF, Logic OFF), (Binary OFF, Logic ON), and (Binary ON, Logic ON).
- Correct AND/OR/NOT A answers from the existing LogicState truth table; both correct and wrong paths for every question.
- Input toggling, retry without advancing, no answer LED leaks, step progression, completion/replay, long-press priority, fresh session on re-entry.
- A display-boundary **approximation**, on every Binary/Logic LEARN/CHALLENGE frame, using a 6×8 pixel default-font estimate and a circular 240×240 screen. This test is **not** a screenshot or physical visual acceptance. The Binary guided title is moved slightly downward to provide estimated round-screen margin.
- Standard ESP32 firmware compile, explicitly preview-enabled ESP32 firmware compile (both flags enabled), and immutable Jackpot baseline compile.

### Build commands for later physical evaluation

Ordinary release-like build, guided modes OFF:

```bash
arduino-cli compile \
  --fqbn 'esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi' \
  firmware/mikey-tech-lab/MikeyTechLab
```

Explicit dual-pilot build for device acceptance only:

```bash
arduino-cli compile \
  --fqbn 'esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi' \
  --build-property 'compiler.cpp.extra_flags=-DMIKEY_ENABLE_BINARY_GUIDED_PREVIEW=1 -DMIKEY_ENABLE_LOGIC_GUIDED_PREVIEW=1' \
  firmware/mikey-tech-lab/MikeyTechLab
```

Do not describe a green compile as physically flashed or verified. Capture exact source SHA and build parameters when hardware becomes available.

## Physical acceptance (deferred)

- [ ] Default build: Logic LEARN/CHALLENGE remain SOON; Binary gating unchanged.
- [ ] Dual-preview build: Binary/Logic LEARN and CHALLENGE READY; other five activities still SOON.
- [ ] Logic LEARN displays correct rule and A/B values; short text fits the circular panel at all phases.
- [ ] Rotate both directions and submit with encoder or touch. Combined press/tap never advances two steps.
- [ ] Wrong answer stays on question and LEDs remain off; correct answer transitions to Observe exactly once.
- [ ] For all three LEARN prompts, physical LEDs show A, B, and output consistently with screen text; other LEDs dark.
- [ ] For all six CHALLENGE questions, displayed predictions and explanatory messages are correct.
- [ ] NOT A demonstrably ignores B.
- [ ] Replay and re-entry start clean, without stale LEDs or incorrect question indexes.
- [ ] Long-press Home takes priority from Predict/Observe/Explain/Complete.
- [ ] Jackpot, Reaction Racer, Simon Lights, Digital Dice, Binary PLAY, Logic PLAY, and Debug Detective regressions all pass.
- [ ] Record screen contrast, perceived responsiveness, touch/encoder feel, unexpected double events, and unexpected resets.

## Promotion sequencing

1. Keep [PR #11](https://github.com/mrkcntrmn/mikey/pull/11) as the parent of this branch, preserving its physical-acceptance requirement.
2. Source-qualify this PR at an immutable commit and record exact-head CI.
3. Do not merge or enable Logic guided modes as an accepted feature until their device checks pass.
4. After parent approval, rebase/retarget this PR and rerun exact-head CI; check no loss of accepted PLAY behavior.
5. Only a separate authorized promotion may make either pilot available in normal builds. Gather mentor observations outside the public repo; do not post personal data or assume IDE readiness.
