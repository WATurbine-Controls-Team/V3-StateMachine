#pragma once

#include <stdint.h>

namespace turbine {

enum class State : uint8_t { Normal, Warning, Emergency };

// One set of fresh sensor readings per update. Values must use the same units
// as their corresponding thresholds; sensor validity is the caller's job.
struct Measurements {
    float rpm;
    float temperature;
    float windSpeed;
    float pitchAngle;
};

// Supplied by the application because the diagram does not define physical
// values or units. Pitch bounds are inclusive and must be ordered min <= max.
struct Thresholds {
    float warningRpm;
    float warningTemperature;
    float maxWindSpeed;
    float maxRpm;
    float maxTemperature;
    float minPitchAngle;
    float maxPitchAngle;
};

class StateMachine {
public:
    // The diagram specifies "> limit": entry occurs at samples 26 and 16.
    static constexpr uint16_t WarningLimit = 25;
    static constexpr uint16_t EmergencyLimit = 15;

    // Starts in Normal with empty counters; construction does not call onEnter.
    explicit StateMachine(const Thresholds& thresholds);

    // Call once per fixed sampling interval with fresh, valid sensor readings.
    // The interval determines the elapsed time represented by each count.
    State update(const Measurements& measurements);
    State state() const;
    uint16_t warningCount() const;
    uint16_t emergencyCount() const;

private:
    void transitionTo(State next);
    void onEnter(State state);
    void onExit(State state);

    Thresholds thresholds_;
    State state_;
    uint16_t warningCount_;
    uint16_t emergencyCount_;
};

} // namespace turbine
