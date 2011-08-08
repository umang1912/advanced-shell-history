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

#ifndef __ASH_QUERIES__
#define __ASH_QUERIES__

#include <map>
#include <string>
using std::map;
using std::string;

namespace ash {


/**
 * 
 */
class Queries {
  // STATIC:
  private:
    static bool initialized;
    static map<string, string> queries;

  public:
    static const string get(const string & query_name);
};


}  // namespace ash

#endif  /* __ASH_QUERIES__ */
