#include "App.hpp"

int main() {
  try {
    MotionControlApp app;
    app.runInteractive();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
