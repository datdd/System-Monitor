#include "format.h"

#include <iomanip>
#include <iostream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  if (seconds < 0) {
    std::cerr << "Uptime cannot be negative" << std::endl;
    return std::string();
  }

  // Format the string using stringstream
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << seconds / 3600 << ":" << std::setw(2)
     << (seconds % 3600) / 60 << ":" << std::setw(2) << seconds % 60;

  return ss.str();
}