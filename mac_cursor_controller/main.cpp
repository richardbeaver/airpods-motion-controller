#include "App.hpp"

int main() {
  try {
    MotionControlApp app;
    app.start();

    std::cout
        << "[Main] Press Enter to recalibrate, Ctrl+C or Ctrl+D to exit.\n";

    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) {
        app.recalibrate();
      }
    }

    app.stop();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
