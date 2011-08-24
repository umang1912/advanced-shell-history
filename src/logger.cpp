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

#include "config.hpp"

#include <stdlib.h>  /* for exit */

#include <string>

using namespace ash;
using namespace std;


/**
 * Converts a string representation of the enum value to the enum code.
 */
const Severity parse(const string & input) {
  if (input == "DEBUG") return DEBUG;
  if (input == "INFO") return INFO;
  if (input == "WARNING") return WARNING;
  if (input == "ERROR") return ERROR;
  if (input == "FATAL") return FATAL;
  return UNKNOWN;
}


/**
 * Converts a severity level to a string.
 */
const char * to_str(const Severity level) {
  switch (level) {
    case DEBUG: return "DEBUG";
    case INFO: return "INFO";
    case WARNING: return "WARNING";
    case ERROR: return "ERROR";
    case FATAL: return "FATAL";
    case UNKNOWN:  // fallthrough.
    default: return "UNKNOWN";
  }
}


/**
 * Returns the log file filename designated as the target for a given severity
 * level.  This relies on environment variables ASH_LOG_LEVEL and ASH_LOG_FILE
 * to be populated or it will return /dev/null as a default.
 */
string get_target(const Severity level) {
  Config & config = Config::instance();

  // Default to /dev/null if the visibility is too low for this Logger.
  const Severity visible = parse(config.get_string("LOG_LEVEL", "DEBUG"));
  if (level < visible) return "/dev/null";
  // Use the configured target, if configured.
  return config.get_string("LOG_FILE", "/dev/null");
}


/**
 * Constructs a logger and adds the severity to the output.
 */
Logger::Logger(const Severity lvl)
  : log(get_target(lvl).c_str(), fstream::out | fstream::app), level(lvl)
{
  log << to_str(level) << ": ";
}


/**
 * Destroys a Logger, flushing output and exiting the program if the severity
 * level was FATAL.
 */
Logger::~Logger() {
  log << endl;
  log.close();
  if (level == FATAL) exit(1);
}
