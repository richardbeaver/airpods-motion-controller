#pragma once
#include "MotionProcessor.hpp"
#include "MotionSmoother.hpp"
#include "MouseController.hpp"
#include "Server.hpp"
#include <atomic>
#include <iostream>
#include <thread>

class MotionControlApp {
  Server server;
  MotionSmoother smoother;
  MotionProcessor processor;
  MouseController mouseController;

  std::atomic_bool running{false};
  std::thread appThread;

public:
  explicit MotionControlApp(unsigned udpPort = 9999) : server(udpPort) {}

  ~MotionControlApp() { stop(); }

  void runInteractive() {
    start();
    std::cout
        << "[Main] Press Enter to recalibrate, Ctrl+C or Ctrl+D to exit.\n";

    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) {
        recalibrate();
      }
    }

    stop();
  }

private:
  void start() {
    if (running.exchange(true)) {
      return;
    }

    server.start();
    appThread = std::thread(&MotionControlApp::appLoop, this);

    std::cout << "[App] Motion control started.\n";
  }

  void stop() {
    if (!running.exchange(false)) {
      return;
    }

    server.stop();
    if (appThread.joinable()) {
      appThread.join();
    }

    std::cout << "[App] Motion control stopped.\n";
  }

  void recalibrate() {
    auto [pitch, yaw] = server.waitForLatest();
    std::cout << "[Main] Recalibrating...\n";
    processor.recalibrate(pitch, yaw);
    mouseController.moveToCenter();
    std::cout << "[Main] Recalibration complete.\n";
  }

  void appLoop() {
    while (running) {
      auto [pitch, yaw] = server.waitForLatest();
      auto [smoothPitch, smoothYaw] = smoother.smooth(pitch, yaw);
      auto [dx, dy] = processor.update(smoothPitch, smoothYaw);
      mouseController.moveRelative(dx, dy);
    }
  }
};
