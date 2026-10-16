# MIKEY-010C — Binary Lab Guided Mode Pilot

**Development status:** feature-branch candidate. This is NOT a physically accepted or enabled-on-main release.

## Objective

Test whether a small, approachable Predict → Observe → Explain interaction helps the learner understand four-bit place value rather than simply memorizing button presses. Preserve all seven accepted PLAY games, the ESP32 hardware adapter, and the immutable Jackpot hardware oracle.

## Behavior

The existing Binary Lab PLAY experience remains unchanged.

- **LEARN:** two guided examples, decimal 5 (0101, 4 + 1) and 10 (1010, 8 + 2). Displays the 8/4/2/1 hint during prediction.
- **CHALLENGE:** four examples, decimal 3, 6, 9, and 12, without the on-screen hint on the prediction step.
- **Predict:** choose between options A and B using either direction of encoder rotation; press the encoder or tap the screen to submit exactly once. Answers deliberately alternate A/B, preventing a repeated-A shortcut.
- **Wrong prediction:** stay on the same question, display TRY AGAIN, do not reveal the answer through LEDs or progress.
- **Correct prediction → Observe:** show the correct four-bit representation on screen and on LEDs 0–3. LED 4 remains off.
- **Observe → Explain:** display the ordinary-language addition of the lit binary places.
- **Explain → next:** progress by exactly one question. After the last explanation, display completion; press/tap to replay.
- **Long-press Home:** remain available from any phase. Exit clears LEDs; re-entry resets the session.

The pilot uses deterministic `BinaryGuidedSession` with no board-specific pins, storage, network requests, accounts, telemetry, or dependency on lesson performance.

## Release gate — default OFF

`MIKEY_ENABLE_BINARY_GUIDED_PREVIEW` is **0** unless explicitly compiled with value **1**. The registry presents LEARN and CHALLENGE as **SOON** on the ordinary build. A direct guided `BinaryActivity::begin` when preview is disabled also fails closed.

**Do not change this default in the development PR.** The pilot can be physically tested with an explicitly flagged build. It must not be described as a released or accepted mode until the device checks below pass and a separate promotion decision is made.

### Ordinary build

```bash
arduino-cli compile \
  --fqbn 'esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi' \
  firmware/mikey-tech-lab/MikeyTechLab
```

### Preview build for adult-guided acceptance only

```bash
arduino-cli compile \
  --fqbn 'esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi' \
  --build-property 'compiler.cpp.extra_flags=-DMIKEY_ENABLE_BINARY_GUIDED_PREVIEW=1' \
  firmware/mikey-tech-lab/MikeyTechLab
```

**Important:** upload the exact candidate built with the flag and record its full commit SHA and build command. A normal, default-off build still displays SOON. No upload or device verification is performed by this PR.

## Automated tests

`tests/binary_guided_session_test.cpp` compiles and runs in CI with preview flag **0 and 1**, linking the production `BinaryActivity.cpp` to a fake hardware surface. Test cases cover:

- default-off gates for Binary and all other modes;
- answer keys, different target values, and answer-position alternation;
- wrong-answer no-advance and no-answer-LED-leak behavior;
- clockwise/counter-clockwise option toggling;
- correct Predict → Observe → Explain → Complete progression and replay;
- fifth LED always off;
- combined press+tap counts as one submission;
- long-press Home takes priority over other input and clears LEDs;
- re-entry begins at the first question;
- baseline Binary PLAY bit/LED behavior remains available.

CI additionally compiles the ordinary ESP32 app, the explicitly enabled preview firmware, and the immutable Jackpot baseline. The final PR must not be considered source-ready until every exact-head CI job passes.

## Physical acceptance checklist

- [ ] Default build: Binary LEARN and CHALLENGE still show SOON.
- [ ] Preview build: other six games' LEARN/CHALLENGE still show SOON.
- [ ] Preview build: Binary LEARN and CHALLENGE show READY; PLAY remains identical.
- [ ] LEARN starts at 1 OF 2, DECIMAL 5, and A/B options.
- [ ] Rotation in both directions toggles exactly once; press and touch submit once.
- [ ] Wrong choice stays at 1 OF 2 with TRY AGAIN and all LEDs off.
- [ ] Correct choice shows 0101 and only logical LEDs 1 and 3 illuminated.
- [ ] Next step explains 5 = 4 + 1.
- [ ] Second question uses correct answer B and explains 10 = 8 + 2.
- [ ] CHALLENGE runs four questions with correct 3/6/9/12 patterns.
- [ ] Completing each mode offers replay; replay restarts correctly.
- [ ] Long-press Home works from prediction, observation, explanation, and completion.
- [ ] Leaving and re-entering resets session and clears stale LEDs.
- [ ] Display text fits the 240×240 round screen; no cut-off, wrap, or low contrast.
- [ ] Jackpot, Reaction, Simon, Dice, Binary PLAY, Logic, and Debug regressions pass.
- [ ] No reset, freeze, accidental double submission, or stale display/LED frame.

Record pass/fail with the **exact flashed revision**. A physically verified build is not implied by green CI.

## Next decision

Collect a small amount of mentor observation separately from the public GitHub repository. Evaluate whether the learner can explain that lit 8/4/2/1 positions sum to the number. If successful, consider a new, separately reviewed promotion PR to expose Binary guided modes. If the lesson language or interaction is confusing, revise this pilot before broadening to other activities. Do not collect identifying information or automatically unlock Arduino IDE lessons.
