#include "TurbineStateMachine.h"

namespace turbine {
namespace {
// Saturation avoids overflow during a sustained fault. Clearing the condition
// resets the count, so each count represents consecutive samples.
uint16_t nextCount(uint16_t count, bool active) {
    if (!active) return 0;
    return count == UINT16_MAX ? count : static_cast<uint16_t>(count + 1);
}
} // namespace

StateMachine::StateMachine(const Thresholds& thresholds)
    : thresholds_(thresholds), state_(State::Normal), warningCount_(0), emergencyCount_(0) {}

State StateMachine::update(const Measurements& m) {
    // Each category combines its triggers with OR: multiple simultaneous
    // triggers still count as one sample, and different triggers can keep the
    // same consecutive count active across successive samples.
    // Wind speed is a warning trigger in the diagram; pitch is an emergency
    // trigger. Both are included even though they were marked unimplemented.
    const bool warning = m.rpm > thresholds_.warningRpm ||
                         m.temperature > thresholds_.warningTemperature ||
                         m.windSpeed > thresholds_.maxWindSpeed;
    const bool emergency = m.rpm > thresholds_.maxRpm ||
                           m.temperature > thresholds_.maxTemperature ||
                           m.pitchAngle < thresholds_.minPitchAngle ||
                           m.pitchAngle > thresholds_.maxPitchAngle;

    // Update both counters in every state so an emergency can develop while
    // already in Warning. A clear sample resets its category's counter.
    warningCount_ = nextCount(warningCount_, warning);
    emergencyCount_ = nextCount(emergencyCount_, emergency);

    // Strict comparisons follow the diagram: equality alone does not enter or
    // leave a state. Emergency takes precedence if both limits are exceeded.
    if (emergencyCount_ > EmergencyLimit) {
        transitionTo(State::Emergency);
    } else if (state_ == State::Emergency) {
        // Recover to Normal as drawn. An ongoing warning is evaluated on the
        // next update, rather than skipping directly to Warning.
        if (emergencyCount_ < EmergencyLimit) transitionTo(State::Normal);
    } else if (warningCount_ > WarningLimit) {
        transitionTo(State::Warning);
    } else if (state_ == State::Warning && warningCount_ < WarningLimit) {
        transitionTo(State::Normal);
    }
    return state_;
}

State StateMachine::state() const { return state_; }
uint16_t StateMachine::warningCount() const { return warningCount_; }
uint16_t StateMachine::emergencyCount() const { return emergencyCount_; }

void StateMachine::transitionTo(State next) {
    // Entry/exit actions run only on a change; repeatedly sampling a condition
    // must not repeatedly initialize or shut down the same hardware.
    if (next == state_) return;
    onExit(state_);
    state_ = next;
    onEnter(state_);
}

void StateMachine::onEnter(State state) {
    // TODO: implement state-specific hardware actions.
    (void)state;
}

void StateMachine::onExit(State state) {
    // TODO: implement state-specific cleanup if needed.
    (void)state;
}
} // namespace turbine
