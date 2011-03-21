#ifndef __ASH_COMMAND__
#define __ASH_COMMAND__

#include <string>

#include "db_object.hpp"

using std::string;

namespace ash {


class Command : public DBObject {
  public:
    static const string get_create_table();

  public:
    Command(const string command, int rval, int start, int end, int num);
    virtual ~Command();

    virtual const string get_name() const;
    virtual const string get_sql() const;
};


}  // namespace ash

#endif /* __ASH_COMMAND__ */
