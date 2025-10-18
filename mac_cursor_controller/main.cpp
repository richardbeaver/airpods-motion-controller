#include "Server.hpp"

int main() {
  try {
    AirPodsMouseServer server;
    server.start();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }

  return 0;
}
