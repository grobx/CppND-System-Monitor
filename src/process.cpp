#include "process.h"

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::copy;
using std::isgreater;
using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid): pid_(pid) {
  cpu_ = 0.0;
  cpu_tot_ = LinuxParser::ActiveJiffies();
  cpu_act_ = LinuxParser::ActiveJiffies(pid);
  cmd_ = LinuxParser::Command(pid);
  ram_ = LinuxParser::Ram(pid);
  uid_ = LinuxParser::Uid(pid);
  user_ = LinuxParser::User(uid_);
  time_ = LinuxParser::UpTime(pid);
}

void Process::Update() {
  long cpu_tot = LinuxParser::ActiveJiffies(),
    cpu_act = LinuxParser::ActiveJiffies(pid_);
  cpu_ = (cpu_act - cpu_act_) / (float) (cpu_tot - cpu_tot_);
  cpu_tot_ = cpu_tot;
  cpu_act_ = cpu_act;
  ram_ = LinuxParser::Ram(pid_);
  time_ = LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return cpu_; }

string Process::Command() const { return cmd_; }

string Process::Ram() const { return ram_; }

string Process::User() const {
  if (user_.empty()) return "<none>";
  return user_.size() <= 6 ? user_ : user_.substr(0, 3) + "...";
}

string Process::Uid() const { return uid_; }

long int Process::UpTime() const { return time_; }

bool Process::operator>(Process const& a) const {
  return isgreater(cpu_, a.cpu_);
}
