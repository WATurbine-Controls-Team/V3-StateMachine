#include "TurbineStateMachine.h"
#include <cassert>
using namespace turbine;

int main() {
    const Thresholds limits = {100, 50, 20, 150, 80, 0, 90};
    const Measurements normal = {100, 50, 20, 0};
    StateMachine machine(limits);
    assert(machine.update(normal) == State::Normal);
    Measurements warning = normal;
    warning.windSpeed = 21;
    for (int i = 0; i < 25; ++i) assert(machine.update(warning) == State::Normal);
    assert(machine.update(warning) == State::Warning);
    assert(machine.update(normal) == State::Normal);
    assert(machine.warningCount() == 0);
    Measurements emergency = normal;
    emergency.pitchAngle = 91;
    for (int i = 0; i < 15; ++i) assert(machine.update(emergency) == State::Normal);
    assert(machine.update(emergency) == State::Emergency);
    assert(machine.update(normal) == State::Normal);
    emergency = {151, 81, 21, -1};
    for (int i = 0; i < 30; ++i) machine.update(emergency);
    assert(machine.state() == State::Emergency);
    // A cleared emergency returns to Normal, then an ongoing warning is evaluated.
    assert(machine.update(warning) == State::Normal);
    assert(machine.update(warning) == State::Warning);
}
