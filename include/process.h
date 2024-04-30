#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  float CalculateCpuUtilization(long totalDiffJiffies);
  std::string Ram();
  long int UpTime();
  bool operator<(const Process& other) const;

  // TODO: Declare any necessary private members
 private:
  int pid_;
  float cpu_;
  long prevTotalJiffies;
  long prevActiveJiffies;
};

#endif