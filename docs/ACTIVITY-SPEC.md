# Activity Specification

Use this template when adding a learning activity.

## Required metadata

- **Name**
- **Primary concept**
- **Secondary concepts**
- **Recommended readiness**
- **Inputs used**
- **Outputs used**
- **Estimated play/challenge duration**

## Required modes

### PLAY

The activity works as a game with minimal teaching interruption.

### LEARN

The activity exposes one concept at a time and connects the visible behavior to ordinary-language technology terms.

### CHALLENGE

The learner predicts, selects, reproduces, diagnoses, or explains something objectively checkable.

## Required learning loop

Where practical, a challenge follows:

1. **PREDICT** — choose or say what should happen.
2. **ACT** — run the experiment or provide the input.
3. **OBSERVE** — inspect the actual result.
4. **EXPLAIN** — connect the outcome to the rule.

Not every PLAY interaction needs all four phases. LEARN and CHALLENGE should use them whenever they improve the concept.

## Difficulty

Prefer changes in reasoning complexity rather than simply making animations faster or sequences longer.

Suggested levels:

- **Discover** — identify cause and effect.
- **Predict** — choose the expected outcome.
- **Explain** — explain the rule.
- **Debug** — diagnose why expected and observed behavior differ.

## Feedback

Good feedback should describe what happened and why.

Prefer:

> The light stopped at position 3. The jackpot is position 0, so the positions do not match.

Avoid feedback that only says:

> Wrong.

## Failure behavior

Failure should normally invite another experiment, reveal a useful clue, or compare expected versus observed behavior. Avoid punitive lives, long lockouts, or grinding.

## Technical boundary

Activity logic should not directly own board-specific pins or libraries. Hardware input/output should be provided through adapters so curriculum logic can survive a future board change.
