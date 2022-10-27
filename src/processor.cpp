#include "processor.h"
#include "linux_parser.h"

Processor::Processor() {
  prevActive = LinuxParser::ActiveJiffies();
  prevTotal = LinuxParser::Jiffies();
}

float Processor::Utilization() {
  long active = LinuxParser::ActiveJiffies();
  long total = LinuxParser::Jiffies();

  int deltaActive = active - prevActive;
  int deltaTotal = total - prevTotal;

  prevActive = active;
  prevTotal = total;

  return deltaActive / (float)deltaTotal;
}
