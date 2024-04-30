#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();

  long Jiffies() const;
  long CalculateJiffies();

  long ActiveJiffies() const;
  long CalculateActiveJiffies();

  long IdleJiffies() const;
  long CalculateIdleJiffies();

  long DiffJiffies() const;
  long CalculateDiffJiffies();
  
  long DiffActiveJiffies() const;
  long CalculateDiffActiveJiffies();
  
  float Utilization();
  void UpdateCacheJiffies();

  friend class Process;

 private:
  long totalJiffies;
  long activeJiffies;
  long idleJiffies;
  long diffJiffies;
  long diffActiveJiffies;
  
  // Cache the previous values of jiffies
  static long prevTotalJiffies;
  static long prevActiveJiffies;
};

#endif