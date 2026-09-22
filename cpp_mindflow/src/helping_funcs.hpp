#include "includes.hpp"

template<typename T>
T getNumber(const std::string_view prompt) {
  T value;
  while (true) {
    std::print("{}", prompt);
    if (std::cin >> value) {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return value;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    println("Error! Enter correct number!");
  }
}

inline std::string getString(const std::string_view prompt) {
  std::print("{}", prompt);
  std::string line;
  if (!std::getline(std::cin, line)) return "";
  return line;
}