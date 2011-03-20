#include <sstream>
#include <string>

#include "ash_log.hpp"


using namespace ash;
using namespace std;


const string DBObject::quote(const char * value) {
  return value ? quote(string(value)) : "null";
}


const string DBObject::quote(const string & in) {
  if (in.empty()) return "null";
  string out = "'";
  for (string::const_iterator i = in.begin(), e = in.end(); i != e; ++i) {
    out.push_back(*i);
    if (*i == '\'') out.push_back('\'');
  }
  out.push_back('\'');
  return out;
}


DBObject::DBObject() {
  // Nothing to do here.
}


DBObject::~DBObject() {
  // Nothing to do here.
}


const string DBObject::get_sql() const {
  typedef map<string, string>::const_iterator c_iter;

  stringstream ss;
  ss << "INSERT INTO " << get_name() << " (";

  // Insert the column names.
  for (c_iter i = values.begin(), e = values.end(); i != e; ) {
    ss << (i -> first);
    if (++i == e) break;
    ss << ", ";
  }

  ss << ") VALUES (";
  // Insert the values.
  for (c_iter i = values.begin(), e = values.end(); i != e; ) {
    ss << (i -> second);
    if (++i == e) break;
    ss << ", ";
  }

  ss << "); ";

  return ss.str();
}
