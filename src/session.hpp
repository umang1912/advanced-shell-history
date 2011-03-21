#ifndef __ASH_SESSION__
#define __ASH_SESSION__

#include <map>
#include <string>

#include "db_object.hpp"

using std::map;
using std::string;

namespace ash {


class Session : public DBObject {
  public:
    static const string get_create_table();

  public:
    Session();
    virtual ~Session();

    virtual const string get_sql() const;
    virtual const string get_name() const;
};


}  // namespace ash

#endif /* __ASH_SESSION__ */

