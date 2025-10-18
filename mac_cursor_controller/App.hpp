#pragma once
#include "MotionProcessor.hpp"
#include "MotionSmoother.hpp"
#include "MouseController.hpp"
#include "RecalibrationController.hpp"
#include "Server.hpp"
#include <chrono>
#include <iostream>
#include <thread>

class MotionControlApp {
  Server server;
  MotionSmoother smoother;
  MotionProcessor processor;
  MouseController mouseController;
  std::unique_ptr<RecalibrationController> recalibrator;

  double currentPitch = 0.0;
  double currentYaw = 0.0;

public:
  explicit MotionControlApp(int udpPort = 9999) : server(udpPort) {}

  ~MotionControlApp() { stop(); }

  void run() {
    // Launch recalibration controller
    recalibrator = std::make_unique<RecalibrationController>([&]() {
      processor.recalibrate(currentPitch, currentYaw);
      mouseController.moveToCenter();
    });

    // Run main app loop
    loop();
  }

private:
  void loop() {
    while (true) {
      // Drain all pending UDP packets
      server.pollLatest();
      // Get latest values
      std::tie(currentPitch, currentYaw) = server.getLatest();

      auto [smoothPitch, smoothYaw] = smoother.smooth(currentPitch, currentYaw);
      auto [dx, dy] = processor.update(smoothPitch, smoothYaw);
      mouseController.moveRelative(dx, dy);

      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  void stop() {
    recalibrator.reset();
    std::cout << "[App] Motion control stopped.\n";
  }
};
