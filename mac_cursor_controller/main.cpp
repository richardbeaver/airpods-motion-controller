#include "App.hpp"
#include "EnvLoader.hpp"

bool g_enableTiming = false;

int main(int argc, const char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--timing") == 0) {
      g_enableTiming = true;
      std::cout << "[Main] Timing output enabled\n";
    }
  }

  EnvLoader::load(".env");
  auto portStr = EnvLoader::get("MOTION_SERVER_PORT", "9999");
  unsigned port = std::stoi(portStr);

  try {
    MotionControlApp app(port);
    app.runInteractive();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
