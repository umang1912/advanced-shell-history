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

#ifndef DISABLE_LOGGER
#ifndef __ASH_LOGGER__
#define __ASH_LOGGER__

#include <fstream>
#include <iostream>

using std::ofstream;
using std::ostream;

namespace ash {


enum Severity {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL,
  UNKNOWN
};


/**
 * This helper class wraps an ofstream and provides a safe and easy way to
 * change / assign a new file target without having to worry about memory
 * leaks.
 */
class Logger {
  // STATIC:
  public:
    static void set_log_file(const char * filename);
    static void set_log_level(const Severity severity);
    static void init(const char * filename=0, const Severity severity=DEBUG);
    static ostream & log(const Severity severity);
    static Severity parse(const char * text);

  private:
    static Logger null_stream, log_stream;
    static Severity log_level;


  // NON-STATIC:
  private:
    Logger()
      : out(NULL)
    {
      attach(NULL);
    }

    ~Logger() {
      detatch();
    }

    void detatch() {
      if (out) {
        out -> flush();
        out -> close();
        delete out;
        out = NULL;
      }
    }

    void attach(const char * filename) {
      if (out) detatch();
      out = new ofstream(filename ? filename : "/dev/null");
    }

  public:
    ostream & get() {
      return *out;
    }

  private:
    ofstream * out;

  // DISALLOWED:
  private:
    Logger(const Logger & other);
    Logger & operator = (const Logger & other);
};


#define LOG(level) (Logger::log(level))


}  // namespace: ash

#endif  /* __ASH_LOGGER__ */
#endif  /* DISABLE_LOGGER */
