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

#ifndef __ASH_CONFIG__
#define __ASH_CONFIG__


#include<map>
#include<string>

using std::map;
using std::string;


namespace ash {

class Config {
  public:
    static void load(const string & filename);
    static Config & instance();
    // TODO(cpa): add an operator [] for accessing the config values

  private:
    map<string, string> values;
};

} // namespace ash

#endif /* __ASH_CONFIG__ */
