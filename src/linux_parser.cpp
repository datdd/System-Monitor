#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  unsigned long long memTotal = 0;
  unsigned long long memFree = 0;
  std::string line;

  std::ifstream meminfoFile(kProcDirectory + kMeminfoFilename);
  if (meminfoFile.is_open()) {
    while (std::getline(meminfoFile, line)) {
      std::stringstream ss(line);
      std::string name;
      unsigned long long value;

      if (std::getline(ss, name, ':') && ss >> value) {
        name = name.substr(0, name.find(' '));

        if (name == "MemTotal") {
          memTotal = value;
        } else if (name == "MemFree") {
          memFree = value;
        }
      }
    }
  }

  return (memTotal > 0 && memFree > 0)
             ? static_cast<float>(memTotal - memFree) /
                   static_cast<float>(memTotal)
             : 0.0;
}

long LinuxParser::UpTime() {
  double uptime{0.0};
  std::string line;

  std::ifstream kUptimeFile(kProcDirectory + kUptimeFilename);
  if (kUptimeFile.is_open()) {
    std::getline(kUptimeFile, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return static_cast<long>(std::round(uptime));
}

long LinuxParser::Jiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  long activateJiffies = 0;

  for (const string& time : cpuUtilization) {
    activateJiffies += std::stol(time);
  }

  return activateJiffies;
}

long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  std::string token;
  long activeJiffies = 0;
  long int utime, stime, cutime, cstime;

  std::string pidString = std::to_string(pid);
  std::ifstream kStatFile(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (kStatFile.is_open()) {
    std::getline(kStatFile, line);
    std::istringstream linestream(line);

    for (int i = 0; i < 13; i++) {
      linestream >> token;
    }

    linestream >> utime >> stime >> cutime >> cstime;
    activeJiffies = utime + stime + cutime + cstime;
  }

  return activeJiffies;
}

long LinuxParser::ActiveJiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  long activateJiffies = 0;

  for (const string& time : cpuUtilization) {
    activateJiffies += std::stol(time);
  }

  // Exclude idle and iowait jiffies from the total
  activateJiffies -= std::stol(cpuUtilization[CPUStates::kIdle_]);
  activateJiffies -= std::stol(cpuUtilization[CPUStates::kIOwait_]);

  return activateJiffies;
}

long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  long idleJiffies = std::stol(cpuUtilization[CPUStates::kIdle_]);
  long iowaitJiffies = std::stol(cpuUtilization[CPUStates::kIOwait_]);
  
  return idleJiffies + iowaitJiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  std::string line;
  std::string token;
  std::vector<std::string> cpuUtilization;

  std::ifstream kStatFile(kProcDirectory + kStatFilename);
  if (kStatFile.is_open()) {
    while (std::getline(kStatFile, line)) {
      std::istringstream linestream(line);
      linestream >> token;
      if (token == "cpu") {
        for (int i = 0; i < kGuestNice_; i++) {
          linestream >> token;
          cpuUtilization.emplace_back(token);
        }
        break;
      }
    }
  }

  return cpuUtilization;
}

int LinuxParser::TotalProcesses() {
  std::string line;
  std::string key;
  int value = 0;
  int totalProcess = 0;

  std::ifstream kStatFile(kProcDirectory + kStatFilename);
  if (kStatFile.is_open()) {
    while (std::getline(kStatFile, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          totalProcess = value;
          break;
        }
      }
    }
  }

  return totalProcess;
}

int LinuxParser::RunningProcesses() {
  std::string line;
  std::string key;
  int value = 0;
  int runningProcess = 0;

  std::ifstream kStatFile(kProcDirectory + kStatFilename);
  if (kStatFile.is_open()) {
    while (std::getline(kStatFile, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          runningProcess = value;
          break;
        }
      }
    }
  }

  return runningProcess;
}

string LinuxParser::Command(int pid) {
  std::string line{};

  std::ifstream kCmdlineFile(kProcDirectory + std::to_string(pid) +
                             kCmdlineFilename);
  if (kCmdlineFile.is_open()) {
    std::getline(kCmdlineFile, line);
    kCmdlineFile.close();
  }

  return line;
}

string LinuxParser::Ram(int pid) {
  std::string line;
  std::string key;
  std::string value;
  std::string ramValue;

  std::ifstream kStatusFile(kProcDirectory + std::to_string(pid) +
                            kStatusFilename);
  if (kStatusFile.is_open()) {
    while (std::getline(kStatusFile, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmRSS:") {
          ramValue = value;
          break;
        }
      }
    }
  }

  return ramValue;
}

string LinuxParser::Uid(int pid) {
  std::string line;
  std::string key;
  std::string value;
  std::string pidUid = {};

  std::ifstream kStatusFile(kProcDirectory + std::to_string(pid) +
                            kStatusFilename);
  if (kStatusFile.is_open()) {
    while (std::getline(kStatusFile, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          pidUid = value;
          break;
        }
      }
    }
  }

  return pidUid;
}

string LinuxParser::User(int pid) {
  std::string line;
  std::string username = {};
  std::string password;
  std::string uid;

  std::ifstream kPasswordFile(kPasswordPath);
  if (kPasswordFile.is_open()) {
    while (std::getline(kPasswordFile, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> password >> uid) {
        if (uid == Uid(pid)) {
          return username;
        }
      }
    }
  }

  return username;
}

long LinuxParser::UpTime(int pid) {
  std::string line;
  std::string token;
  long long int startTime = 0;
  long upTime = 0;
  long clkTck = sysconf(_SC_CLK_TCK);

  std::ifstream kStatFile(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (kStatFile.is_open()) {
    std::getline(kStatFile, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; ++i) {
      std::getline(linestream, token, ' ');
    }
    linestream >> startTime;
  }
  upTime = startTime / clkTck;
  upTime = UpTime() - upTime;

  return upTime;
}
