#include <chrono>
#include <iostream>

#ifndef APP_FUNCTIONS_TIME
#define APP_FUNCTIONS_TIME

inline void outputLatency(long long clientTimestampMs,
                          const std::string &latencyMeasure) {
  auto now = std::chrono::system_clock::now();
  auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                   now.time_since_epoch())
                   .count();

  auto latency = nowMs - clientTimestampMs;
  std::cout << latencyMeasure << " latency: " << latency << " ms\n";
}

#endif
