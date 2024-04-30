#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include <ncurses.h>
#include <process.h>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    : pid_(pid), prevTotalJiffies(0), prevActiveJiffies(0) {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return cpu_; }

float Process::CalculateCpuUtilization(long totalDiffJiffies) {
  // Calculate the jiffies of the current process
  long activeJiffies = LinuxParser::ActiveJiffies(Pid());
  // Calculate the differences
  long activeDiff = activeJiffies - prevActiveJiffies;
  // Update the previous jiffies
  prevActiveJiffies = activeJiffies;
  // Calculate the CPU utilization
  cpu_ = totalDiffJiffies > 0
             ? static_cast<float>(activeDiff) / totalDiffJiffies
             : 0.0;

  return cpu_;
}

string Process::Command() { return LinuxParser::Command(Pid()); }
string Process::Ram() { return LinuxParser::Ram(Pid()); }
string Process::User() { return LinuxParser::User(Pid()); }
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }
bool Process::operator<(const Process& other) const {
  // Prioritize comparing by CPU utilization
  if (CpuUtilization() < other.CpuUtilization()) {
    return true;
  } else if (CpuUtilization() > other.CpuUtilization()) {
    return false;
  }

  // If CPU utilization is equal, compare by process ID (PID)
  return Pid() < other.Pid();
}
