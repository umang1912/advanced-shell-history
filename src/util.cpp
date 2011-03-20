#include <sstream>
#include <string>

#include "ash_log.hpp"


using namespace ash;
using namespace std;


string Util::to_string(int value) {
  static stringstream ss;
  ss.str("");
  ss << value;
  return ss.str();
}
