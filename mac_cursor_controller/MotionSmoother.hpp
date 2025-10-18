#pragma once
#include <tuple>

class MotionSmoother {
  double alpha; // Smoothing factor [0,1]
  double smoothPitch = 0.0;
  double smoothYaw = 0.0;
  bool initialized = false;

public:
  explicit MotionSmoother(double config_alpha = 0.2) : alpha(config_alpha) {}

  // Apply exponential smoothing to pitch & yaw
  std::tuple<double, double> smooth(double pitch, double yaw) {
    if (!initialized) {
      smoothPitch = pitch;
      smoothYaw = yaw;
      initialized = true;
      return {pitch, yaw};
    }

    smoothPitch = alpha * pitch + (1.0 - alpha) * smoothPitch;
    smoothYaw = alpha * yaw + (1.0 - alpha) * smoothYaw;

    return {smoothPitch, smoothYaw};
  }
};
