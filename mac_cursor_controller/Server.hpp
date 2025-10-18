#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

class Server {
  unsigned PORT = 9999;
  int sockfd = -1;
  std::atomic_bool running{false};
  std::thread serverThread;

  std::atomic<double> latestPitch{0.0};
  std::atomic<double> latestYaw{0.0};

public:
  explicit Server(unsigned port = 9999) : PORT(port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      perror("socket");
      return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
      perror("bind");
      close(sockfd);
      return;
    }

    // Set non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "[Server] Listening on UDP port " << port << "\n";
  }

  ~Server() {
    stop();
    if (sockfd >= 0) {
      close(sockfd);
    }
  }

  void start() {
    if (running.exchange(true)) {
      return;
    }
    serverThread = std::thread(&Server::receiveLoop, this);
  }

  void stop() {
    if (!running.exchange(false)) {
      return;
    }
    if (serverThread.joinable()) {
      serverThread.join();
    }
  }

  std::tuple<double, double> getLatest() const {
    return {latestPitch.load(), latestYaw.load()};
  }

private:
  // Constantly update member pitch and yaw values from received packet data
  void receiveLoop() {
    char buffer[526];
    sockaddr_in sender{};
    socklen_t senderLen = sizeof(sender);

    while (true) {
      ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                             reinterpret_cast<sockaddr *>(&sender), &senderLen);

      if (len > 0) {
        buffer[len] = '\0';
        std::istringstream iss(buffer);

        double pitch, yaw;
        if (iss >> pitch >> yaw) {
          latestPitch.store(pitch);
          latestYaw.store(yaw);
        }
      } else {
        // No data ready
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }

    std::cout << "[Server] Receiver thread stopped.\n";
  }
};
