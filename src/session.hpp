#ifndef __ASH_SESSION__
#define __ASH_SESSION__

#include <map>
#include <string>

using std::map;
using std::string;


class Session {
  public:
    Session();
    string get_sql() const;

  private:
    map<string, string> values;
};


#endif /* __ASH_SESSION__ */

