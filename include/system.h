#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::string;
using std::vector;

class System {
 public:
  System();
  void Update();
  Processor& Cpu();
  vector<Process>& Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  string Kernel();
  string OperatingSystem();

 private:
  Processor cpu_ = {};
  vector<Process> processes_ = {};
  string kernel_;
  string os_;
  float mem_;
  int proc_run_, proc_tot_;
  long int time_;
};

#endif
