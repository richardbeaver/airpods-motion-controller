#include "App.hpp"

int main() {
  try {
    MotionControlApp app;
    app.start();

    // Keep alive until user quits
    std::cout << "[Main] Press Ctrl+C or Ctrl+D to exit.\n";
    std::string line;
    while (std::getline(std::cin, line)) {
    }

    app.stop();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
