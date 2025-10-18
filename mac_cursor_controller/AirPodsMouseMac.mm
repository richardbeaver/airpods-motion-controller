#include "MotionProcessor.hpp"
#include "MouseController.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

class AirPodsMouseController {
  static inline unsigned PORT = 9999;

  MotionProcessor processor;
  MouseController mouse;

public:
  void start() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    std::cout << "AirPodsMouseController running on port " << PORT << "..."
              << '\n';

    char buffer[256];
    struct sockaddr_in sender{};
    socklen_t senderLen = sizeof(sender);

    while (true) {
      ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr *)&sender, &senderLen);
      if (n < 0) {
        perror("Could not receive in `recvfrom`");
        break;
      }
      if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);

        float pitch, yaw, roll;
        if (sscanf(buffer, "%f %f %f", &pitch, &yaw, &roll) == 3) {
          auto [dx, dy] = processor.update_pitch_yaw(pitch, yaw);
          mouse.moveCursor(dx, dy);
        }
      }
    }

    close(sockfd);
  }
};

int main() {
  try {
    AirPodsMouseController controller;
    controller.start();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
  return 0;
}
