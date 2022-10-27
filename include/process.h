#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int pid);

  void Update();
  int Pid() const;
  string User() const;
  string Uid() const;
  string Command() const;
  float CpuUtilization() const;
  string Ram() const;
  long int UpTime() const;
  bool operator>(Process const& a) const;

 private:
  int pid_;
  long cpu_act_, cpu_tot_;
  float cpu_;
  string cmd_;
  string ram_;
  string user_;
  string uid_;
  long int time_;
};

#endif
