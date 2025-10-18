#include "MotionProcessor.hpp"
#include "MotionSmoother.hpp"
#include "MouseController.hpp"
#include "RecalibrationController.hpp"
#include "Server.hpp"

int main() {
  try {
    MotionProcessor processor;
    MotionSmoother smoother;
    MouseController mouse;
    Server server;

    double pitch = 0.0;
    double yaw = 0.0;

    // Input thread: press Enter to recalibrate
    RecalibrationController recalibrator([&]() {
      processor.recalibrate(pitch, yaw);
      mouse.moveToCenter();
    });

    std::cout << "[Main] Ready.\n";

    while (true) {
      // Drain all pending UDP packets
      server.pollLatest();
      // Get latest values
      std::tie(pitch, yaw) = server.getLatest();

      auto [smoothPitch, smoothYaw] = smoother.smooth(pitch, yaw);
      auto [dx, dy] = processor.update(smoothPitch, smoothYaw);
      mouse.moveRelative(dx, dy);

      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
