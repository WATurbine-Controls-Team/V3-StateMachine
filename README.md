# V3 State Machine

This repository contains the [state machine diagram](V3StateMachine.excalidraw) and a C++11 skeleton for an ESP32-S3 using the Arduino ESP32 core. The library has no hardware dependencies; sensor reading, actuator actions, and hardware initialization remain to be implemented.

## Integration

Add this repository as an Arduino library, install the Arduino ESP32 board core, and select your ESP32-S3 board. The example in `examples/TurbineStateMachine` shows how to construct the machine and call `update()` from `loop()`. Replace its illustrative thresholds, placeholder sensor readings, and sampling interval with validated values. Implement the state entry and exit actions in `src/TurbineStateMachine.cpp`. The application must supply fresh, valid sensor readings and handle sensor failures; this skeleton does not validate readings or drive safety hardware.

## Transition behavior

Each call to `update()` is one sample. Warning conditions are RPM or temperature above their warning thresholds, or wind speed above its maximum. Emergency conditions are RPM or temperature above their maximum, or pitch angle outside the inclusive configured range. Either counter increments once per sample if any of its conditions is present, resets to zero when all its conditions clear, and saturates at 65535. Both counters run in every state.

The diagram's strict limits are used: warning begins after more than 25 consecutive warning samples and emergency after more than 15 consecutive emergency samples. Emergency takes priority. A warning or emergency returns to Normal when its counter falls below its limit; with the reset behavior this occurs on the first clear sample. After emergency recovery, an ongoing warning is evaluated on the next update. These counter and recovery choices are assumptions because the diagram does not specify counter decay or sampling frequency. Choose the interval with care: it determines the time before a transition.

## Host test

The transition logic can be checked without ESP32 hardware:

```sh
g++ -std=c++11 -Wall -Wextra -pedantic -Isrc src/TurbineStateMachine.cpp tests/state_machine_test.cpp -o state_machine_test
./state_machine_test
```
