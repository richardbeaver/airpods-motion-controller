#include "App.hpp"
#include "EnvLoader.hpp"

int main() {
  EnvLoader::load(".env");
  auto portStr = EnvLoader::get("MOTION_SERVER_PORT", "9999");
  unsigned port = std::stoi(portStr);

  std::cout << "[Server] Starting on port: " << port << "\n";

  try {
    MotionControlApp app(port);
    app.runInteractive();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
