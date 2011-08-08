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

#include "queries.hpp"

#include <map>
#include <string>

using namespace ash;
using namespace std;


bool Queries::initialized = false;
map<string, string> Queries::queries;


const string Queries::get(const string & name) {
  if (!initialized) {
    // TODO(cpa): load the saved queries files: /etc/ash/queries and ~/.ash/queries
    
    initialized = true;
  }

  if (queries.find(name) == queries.end()) return "";

  // TODO(cpa): return the SQL for the found query.
  return "select * from commands;";
}
