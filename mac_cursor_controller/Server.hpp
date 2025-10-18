#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

class Server {
  unsigned PORT = 9999;
  int sockfd = -1;
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

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("bind");
      close(sockfd);
      return;
    }

    // Set non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "AirPodsMouse Server running on port " << PORT << "...\n";
  }

  ~Server() {
    if (sockfd >= 0) {
      close(sockfd);
    }
  }

  // Drain all available packets, leaving state set to latest pitch and yaw
  void pollLatest() {
    char buffer[526];
    sockaddr_in sender{};
    socklen_t senderLen = sizeof(sender);

    double newPitch{}, newYaw{};
    bool newData = false;

    while (true) {
      ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr *)&sender, &senderLen);
      if (len <= 0) {
        break;
      }

      buffer[len] = '\0';

      std::istringstream iss(buffer);
      double pitch, yaw;
      if (iss >> pitch >> yaw) {
        newPitch = pitch;
        newYaw = yaw;
        newData = true;
      }
    }

    // Only write once — and only if new packets were parsed
    if (newData) {
      latestPitch.store(newPitch);
      latestYaw.store(newYaw);
    }
  }

  std::tuple<double, double> getLatest() const {
    return {latestPitch.load(), latestYaw.load()};
  }
};
