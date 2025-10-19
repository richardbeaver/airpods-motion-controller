#pragma once
#include <algorithm>
#include <tuple>

class MotionSmoother {
  double smoothPitch = 0.0;
  double smoothYaw = 0.0;
  bool initialized = false;

  // Base responsiveness limits
  // - alpha == smoothing factor in range [0,1]
  // - lower alpha => smoother motion
  double minAlpha = 0.3;
  double maxAlpha = 0.9;
  double adaptScale = 2.0;

public:
  MotionSmoother() = default;

  // Apply adaptive smoothing to pitch & yaw
  std::tuple<double, double> smooth(double pitch, double yaw) {
    if (!initialized) {
      smoothPitch = pitch;
      smoothYaw = yaw;
      initialized = true;
      return {pitch, yaw};
    }

    // Measure change intensity
    double deltaPitch = std::abs(pitch - smoothPitch);
    double deltaYaw = std::abs(yaw - smoothYaw);
    double delta = deltaPitch + deltaYaw;

    // Adaptive smoothing factor:
    // - small delta = lower alpha (smoother)
    // - large delta = higher alpha (faster)
    double alpha =
        std::clamp(minAlpha + (delta * adaptScale), minAlpha, maxAlpha);

    // Apply exponential smoothing
    smoothPitch = alpha * pitch + (1.0 - alpha) * smoothPitch;
    smoothYaw = alpha * yaw + (1.0 - alpha) * smoothYaw;

    return {smoothPitch, smoothYaw};
  }
};
