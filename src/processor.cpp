#include "processor.h"

#include <ncurses.h>

#include "linux_parser.h"

// Initialize the static member variables
long Processor::prevTotalJiffies = 0;
long Processor::prevActiveJiffies = 0;

Processor::Processor() {
  CalculateJiffies();
  CalculateActiveJiffies();
  CalculateIdleJiffies();
}

long Processor::Jiffies() const { return totalJiffies; }
long Processor::CalculateJiffies() {
  totalJiffies = LinuxParser::Jiffies();
  return totalJiffies;
}

long Processor::ActiveJiffies() const { return activeJiffies; }
long Processor::CalculateActiveJiffies() {
  activeJiffies = LinuxParser::ActiveJiffies();
  return activeJiffies;
}

long Processor::IdleJiffies() const { return idleJiffies; }
long Processor::CalculateIdleJiffies() {
  idleJiffies = LinuxParser::IdleJiffies();
  return idleJiffies;
}

long Processor::DiffJiffies() const { return diffJiffies; }
long Processor::CalculateDiffJiffies() {
  diffJiffies = Jiffies() - prevTotalJiffies;
  return diffJiffies;
}

long Processor::DiffActiveJiffies() const { return diffActiveJiffies; }
long Processor::CalculateDiffActiveJiffies() {
  return ActiveJiffies() - prevActiveJiffies;
}

void Processor::UpdateCacheJiffies() {
  prevTotalJiffies = Jiffies();
  prevActiveJiffies = ActiveJiffies();
}

float Processor::Utilization() {
  CalculateJiffies();
  CalculateActiveJiffies();

  // Calculate the differences
  long totalDiff = CalculateDiffJiffies();
  long activeDiff = CalculateDiffActiveJiffies();

  // Update the previous jiffies
  UpdateCacheJiffies();

  // Calculate the CPU utilization
  return totalDiff > 0 ? static_cast<float>(activeDiff) / totalDiff : 0.0;
}