#include <TurbineStateMachine.h>

// TODO: Replace every value with validated values in the units of sensors.
const turbine::Thresholds thresholds = {
    1000.0f, // warning RPM
    60.0f,   // warning temperature
    20.0f,   // maximum wind speed
    1500.0f, // maximum RPM
    80.0f,   // maximum temperature
    0.0f,    // minimum valid pitch angle
    90.0f    // maximum valid pitch angle
};
turbine::StateMachine machine(thresholds);

void setup() {
    // TODO: initialize sensors and actuators.
}

void loop() {
    // TODO: replace with fresh sensor readings and a fixed sampling interval.
    const turbine::Measurements readings = {0.0f, 0.0f, 0.0f, 0.0f};
    machine.update(readings);
    delay(100); // Example interval only; choose a rate for the real system.
}
