#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <unordered_set>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::find_if;
using std::greater;
using std::set;
using std::size_t;
using std::sort;
using std::string;
using std::vector;
using std::unordered_set;

System::System() {
  os_ = LinuxParser::OperatingSystem();
  kernel_ = LinuxParser::Kernel();
}

void System::Update() {
  // Update Processes
  unordered_set<int> done;
  for (auto pid : LinuxParser::Pids()) {
    vector<Process>::iterator prev =
      find_if(processes_.begin(), processes_.end(),
              [&pid](const Process& x) { return x.Pid() == pid; });
    if (prev != processes_.end()) {
      prev->Update();
    } else {
      processes_.emplace_back(Process(pid));
    }
    done.insert(pid);
  }
  
  // Remove Non Existant
  for(auto p = processes_.begin(); p != processes_.end(); ++p) {
    if (done.find(p->Pid()) == done.end())
      processes_.erase(p);
  }

  // Sort Processes Descending
  sort(processes_.begin(), processes_.end(), greater());

  // Memory Utilization
  mem_ = LinuxParser::MemoryUtilization();

  // Running Processes
  proc_run_ = LinuxParser::RunningProcesses();

  // Total Processes
  proc_tot_ = LinuxParser::TotalProcesses();

  // Up Time
  time_ = LinuxParser::UpTime();
}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { return processes_; }

string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return mem_; }

string System::OperatingSystem() { return os_; }

int System::RunningProcesses() { return proc_run_; }

int System::TotalProcesses() { return proc_tot_; }

long int System::UpTime() { return time_; }
