#include "linux_parser.h"

#include <array>
#include <filesystem>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

using std::array;
using std::equal;
using std::getline;
using std::ifstream;
using std::invalid_argument;
using std::istringstream;
using std::replace;
using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::path;

string readRow(const string file, const string key) {
  ifstream s(file);
  if (!s.is_open()) return "";

  string line;
  while (getline(s, line)) {
    if (equal(key.begin(), key.end(), line.begin()))
      return line;
  }

  return "";
}

string readRow(const string file, const int row) {
  ifstream s(file);
  if (!s.is_open()) return "";

  int i;
  string line;
  for (i = 0; i <= row && getline(s, line); ++i)
    ;

  return i < row ? "" : line;
}

template <typename T>
T pickCol(const string line, const int col) {
  istringstream iss(line);

  string ignore;
  for (int i = 0; i < col && iss >> ignore; ++i)
    ;

  T val;
  iss >> val;
  return val;
}

template <typename T, int N>
array<T, N> pickCols(const string line, const int skip = 0) {
  istringstream iss(line);

  string ignore;
  for (int i = 0; i < skip; ++i) iss >> ignore;

  array<T, N> vals;
  for (int i = 0; i < N && iss >> vals[i]; ++i)
    ;
  return vals;
}

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), ' ', '_');
      replace(line.begin(), line.end(), '=', ' ');
      replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  return pickCol<string>(readRow(kProcDirectory + kVersionFilename, 0), 2);
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  directory_iterator proc{path(kProcDirectory)};
  for (const path& path : proc) {
    if (!is_directory(path)) continue;
    try {
      pids.push_back(stoi(path.filename().string()));
    } catch (const invalid_argument& e) {
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  const auto meminfoPath = kProcDirectory + kMeminfoFilename;
  int tot = pickCol<int>(readRow(meminfoPath, "MemTotal:"), 1),
      avail = pickCol<int>(readRow(meminfoPath, "MemAvailable:"), 1);
  return 1 - (avail / (float)tot);
}

long LinuxParser::UpTime() {
  return pickCol<long>(readRow(kProcDirectory + kUptimeFilename, 0), 0);
}

long LinuxParser::Jiffies() {
  string row = readRow(kProcDirectory + kStatFilename, "cpu");
  array<long, 10> data = pickCols<long, 10>(row, 1);

  auto [user, nice, system, idle, iowait, irq, softirq, steal, guest,
        guestnice] = data;

  return user                // user time
    + nice                   // nice time
    + system + irq + softirq // system all time
    + idle + iowait          // idle all time
    + steal;                 // steal time
}

long LinuxParser::ActiveJiffies(int pid) {
  string row = readRow(kProcDirectory + to_string(pid) + kStatFilename, 0);
  array<long, 4> data = pickCols<long, 4>(row, 13);
  return std::accumulate(data.begin(), data.end(), 0L);
}

long LinuxParser::ActiveJiffies() {
  string row = readRow(kProcDirectory + kStatFilename, "cpu");
  array<long, 10> data = pickCols<long, 10>(row, 1);
  
  auto [user, nice, system, idle, iowait, irq, softirq, steal, guest,
        guestnice] = data;
  
  return user                 // user time
    + nice                    // nice time
    + system + irq + softirq  // system all time
    + steal;                  // steal time
}

long LinuxParser::IdleJiffies() {
  return pickCol<long>(readRow(kProcDirectory + kStatFilename, "cpu"), 4);
}

int LinuxParser::TotalProcesses() {
  return pickCol<int>(readRow(kProcDirectory + kStatFilename, "processes"), 1);
}

int LinuxParser::RunningProcesses() {
  return pickCol<int>(
    readRow(kProcDirectory + kStatFilename, "procs_running"), 1);
}

string LinuxParser::Command(int pid) {
  return readRow(kProcDirectory + to_string(pid) + kCmdlineFilename, 0);
}

string LinuxParser::Ram(int pid) {
  return pickCol<string>(
    readRow(kProcDirectory + to_string(pid) + kStatusFilename, "VmSize:"), 1);
}

string LinuxParser::Uid(int pid) {
  return pickCol<string>(
    readRow(kProcDirectory + to_string(pid) + kStatusFilename, "Uid:"), 1);
}

string LinuxParser::User(string uid) {
  ifstream s(kPasswordPath);
  if (!s.is_open()) return "<err>";

  string line, user, passwd, cuid;
  while(getline(s, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream linestream(line);
    linestream >> user >> passwd >> cuid;
    if (cuid == uid)
      return user;
  }
  exit(0);
  
  return string("<none>");
}

long LinuxParser::UpTime(int pid) {
  long uptime = pickCol<long>(
    readRow(kProcDirectory + to_string(pid) + kStatFilename, 0), 21);
  return uptime / sysconf(_SC_CLK_TCK);
}
