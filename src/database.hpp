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

#ifndef __ASH_DATABASE__
#define __ASH_DATABASE__


class sqlite3;  // Forward declaration.

namespace ash {


class Database {
  public:
    Database(const char * filename);
    virtual ~Database();

    void exec(const char * query) const;
    int select_int(const char * query) const;

  private:
    void init_db();

  private:
    const char * db_filename;
    sqlite3 * db;
};


}  // namespace ash

#endif /* __ASH_DATABASE__ */
