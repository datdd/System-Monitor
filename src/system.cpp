#include "system.h"

#include <dirent.h>
#include <ncurses.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

// A vector of all Process objects in the system
vector<Process>& System::Processes() {
  // Get current process IDs
  vector<int> pids = LinuxParser::Pids();
  set<int> curPidSet(pids.begin(), pids.end());

  // Last set of PIDs
  set<int> lastPidSet;
  for (const Process& process : processes_) {
    lastPidSet.insert(process.Pid());
  }

  // Remove processes not in the current list of PIDs
  processes_.erase(remove_if(processes_.begin(), processes_.end(),
                             [&](const Process& p) {
                               return curPidSet.find(p.Pid()) ==
                                      curPidSet.end();
                             }),
                   processes_.end());

  // Add new processes
  Processor processor = Cpu();
  for (int pid : pids) {
    if (lastPidSet.find(pid) == lastPidSet.end()) {
      processes_.emplace_back(Process(pid));
    }
  }
  for (Process& p : processes_) {
    p.CalculateCpuUtilization(processor.DiffJiffies());
  }

  std::sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }
long int System::UpTime() { return LinuxParser::UpTime(); }