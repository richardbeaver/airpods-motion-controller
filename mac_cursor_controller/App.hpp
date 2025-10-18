#pragma once
#include "MotionProcessor.hpp"
#include "MotionSmoother.hpp"
#include "MouseController.hpp"
#include "RecalibrationController.hpp"
#include "Server.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

class MotionControlApp {
  Server server;
  MotionSmoother smoother;
  MotionProcessor processor;
  MouseController mouseController;
  std::unique_ptr<RecalibrationController> recalibrator;

  std::atomic_bool running{false};
  std::thread mainThread;

  double currentPitch = 0.0;
  double currentYaw = 0.0;

public:
  explicit MotionControlApp(int udpPort = 9999) : server(udpPort) {}

  ~MotionControlApp() { stop(); }

  void start() {
    if (running.exchange(true)) {
      return;
    }

    // Launch recalibration controller in another thread
    recalibrator = std::make_unique<RecalibrationController>([&]() {
      processor.recalibrate(currentPitch, currentYaw);
      mouseController.moveToCenter();
    });

    // Run main app loop in a second other thread
    mainThread = std::thread(&MotionControlApp::loop, this);

    std::cout << "[App] Motion control started.\n";
  }

  void stop() { cleanUp(); }

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

  void cleanUp() {
    if (!running.exchange(false)) {
      return;
    }
    if (mainThread.joinable()) {
      mainThread.join();
    }
    recalibrator.reset();
    std::cout << "[App] Motion control stopped.\n";
  }
};
