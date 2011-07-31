/*
   Copyright 2011 Carl Anderson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "logger.hpp"

#include <fstream>
#include <string>


using namespace ash;
using namespace std;


Logger Logger::null_stream, Logger::log_stream;
Severity Logger::log_level = DEBUG;


/**
 * 
 */
void Logger::set_log_file(const char * filename) {
  log_stream.attach(filename);
}


/**
 * 
 */
void Logger::set_log_level(const Severity severity) {
  log_level = severity;
}


/**
 * 
 */
void Logger::init(const char * filename, const Severity severity) {
  log_level = severity;
  log_stream.attach(filename);
}


/**
 * 
 */
Severity Logger::parse(const char * text) {
  string input(text);
  if (input == "DEBUG") return DEBUG;
  if (input == "INFO") return INFO;
  if (input == "WARNING") return WARNING;
  if (input == "ERROR") return ERROR;
  if (input == "FATAL") return FATAL;
  return UNKNOWN;
}


/**
 * 
 */
ostream & Logger::log(const Severity severity) {
  // Suppress logged events below the current threshold.
  if (severity < log_level)
    return null_stream.get();

  // Log the severity and return the stream.
  ostream & out = log_stream.get();
  switch (severity) {
    case DEBUG: return out << "DEBUG: ";
    case INFO: return out << "INFO: ";
    case WARNING: return out << "WARNING: ";
    case ERROR: return out << "ERROR: ";
    case FATAL: return out << "FATAL: ";
    case UNKNOWN:  // fallthrough.
    default:
      return out << "UNKNOWN: ";
  }
}
