#pragma once
#include <atomic>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

class RecalibrationController {
public:
  using RecalibrateFn = std::function<void()>;

  explicit RecalibrationController(RecalibrateFn onRecalibrate)
      : running_(true), onRecalibrate_(std::move(onRecalibrate)),
        thread_(&RecalibrationController::run, this) {}

  ~RecalibrationController() {
    running_ = false;
    if (thread_.joinable()) {
      thread_.join();
    }
  }

private:
  std::atomic<bool> running_;
  std::function<void()> onRecalibrate_;
  std::thread thread_;

  void run() {
    std::cout << "[Input] Press Enter to recalibrate (Ctrl+D to quit).\n";
    std::string line;
    while (running_) {
      // `getline` fails on EOF (Ctrl+D) or input stream closed
      if (!std::getline(std::cin, line)) {
        std::cout << "[Input] Exiting input loop.\n";
        running_ = false;
        break;
      }

      // Just pressing Enter gives an empty line - recalibrate
      if (line.empty() && onRecalibrate_) {
        std::cout << "[Input] Recalibrating...\n";
        onRecalibrate_();
      }
    }
  }
};
