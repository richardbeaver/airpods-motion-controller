#pragma once
#include <tuple>

class MotionProcessor {
  double lastPitch = 0.0;
  double lastYaw = 0.0;
  double sensitivity = 20.0;

public:
  MotionProcessor() = default;

  // Update motion data, return delta x/y for mouse movement
  std::tuple<double, double> update(double pitch, double yaw) {
    auto dx = (yaw - lastYaw) * sensitivity * -1;
    auto dy = (pitch - lastPitch) * sensitivity;

    lastPitch = pitch;
    lastYaw = yaw;

    return {dx, dy};
  }
};
