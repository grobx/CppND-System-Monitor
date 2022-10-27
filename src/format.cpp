#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::ostringstream;
using std::string;

string Format::ElapsedTime(long seconds) {
  ostringstream os;
  int hh = seconds / 3600, mm = seconds % 3600 / 60, ss = seconds % 60;
  os << hh << ":" << std::setw(2) << std::setfill('0') << mm << ":"
     << std::setw(2) << std::setfill('0') << ss;
  return os.str();
}
