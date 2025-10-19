#pragma once
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

class EnvLoader {
public:
  static void load(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
      std::cerr << "[Env] Could not open " << path << ", using defaults.\n";
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      trim(line);
      if (line.empty() || line[0] == '#')
        continue;

      auto pos = line.find('=');
      if (pos == std::string::npos)
        continue;

      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);

      trim(key);
      trim(value);

      // Add to process environment
      setenv(key.c_str(), value.c_str(), 1);
      std::cout << "[Env] Loaded " << key << "=" << value << "\n";
    }
  }

  static std::string get(const std::string &key,
                         const std::string &defaultValue = "") {
    const char *val = std::getenv(key.c_str());
    if (!val) {
      std::cerr << "[Env] Missing " << key
                << ", using default: " << defaultValue << "\n";
      return defaultValue;
    }
    return val;
  }

private:
  static void trim(std::string &s) {
    auto start = s.find_first_not_of(" \t\r\n");
    auto end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
      s.clear();
    else
      s = s.substr(start, end - start + 1);
  }
};
