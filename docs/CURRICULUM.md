# Curriculum

## Phase 1 goal

Build a practical mental model of a computer using the ESP32 without requiring IDE use.

## Core Seven

### 01 — Jackpot Wheel

**Primary concept:** Input → process → output

Mikey stops a rotating light. The device reads an input, compares the stopped position with the jackpot position, and produces a result on the display/lights.

Useful follow-on ideas:

- repeated updates create apparent motion;
- speed changes when the wait interval changes;
- RUNNING and STOPPED are different states;
- a comparison decides WIN versus MISS.

Suggested prompts:

- Is one light physically moving around the board?
- What do you predict will happen if the speed increases?
- What information does the computer need before it can decide whether you won?

### 02 — Reaction Racer

**Primary concept:** Time and measurement

Wait for a cue, then respond as quickly as possible. Display reaction time in milliseconds.

Concepts: clocks, measurement, precision, early input, repeatability.

### 03 — Simon Lights

**Primary concept:** Memory and sequences

Watch and reproduce an increasingly long sequence.

Concepts: ordered data, memory, sequence length, replay.

### 04 — Digital Dice

**Primary concept:** Randomness and probability

Press to generate a die result. Later, run repeated trials and count outcomes.

Concepts: pseudo-random numbers, frequency, fairness, data collection.

### 05 — Binary Lab

**Primary concept:** Bits and data representation

Use LEDs or on-screen cells to show small numbers in binary and convert between ordinary numbers and bit patterns.

Start small: 0–15 with four bits.

Concepts: on/off states, place value, representation, the idea that information can be encoded numerically.

### 06 — Logic Lab

**Primary concept:** Conditions and Boolean logic

Two or more inputs determine an output using simple rules.

Begin with ordinary language:

- IF A AND B, turn on the light.
- IF A OR B, turn on the light.
- IF NOT A, show the symbol.

Concepts: true/false, AND, OR, NOT, conditions.

### 07 — Debug Detective

**Primary concept:** Debugging and engineering reasoning

The device intentionally behaves incorrectly. Mikey compares expected and observed behavior and chooses or explains the most likely rule that is wrong.

Examples:

- a traffic light skips yellow;
- Simon repeats the wrong color;
- a binary display has one incorrect bit;
- a timer advances at the wrong rate.

Concepts: hypothesis, evidence, isolation, expected versus observed behavior.

## Phase 2 candidates

After the Core Seven are stable:

- Color Mixer — RGB and pixels
- Coordinate Treasure Hunt — x/y coordinates
- Pixel Artist — displays and raster graphics
- Traffic Light Engineer — state machines
- Secret Combination — sequences and authentication concepts
- Morse Messenger — encoding and communication
- Sensor Detective — physical sensing and analog/digital values
- Voltage Target — ADC and measurement
- Wireless Message — radio, packets, sender/receiver
- Packet Delivery — reliability and retransmission

## Phase 3: Code connection

Do not begin with a blank IDE.

Use a known activity and reveal one relevant piece of code at a time:

1. familiar setting → variable;
2. familiar rule → `if` condition;
3. repeated movement → loop/update cycle;
4. remembered sequence → array/list;
5. PLAY/STOPPED behavior → state.

The first code exercises should modify known behavior rather than build an application from scratch.
