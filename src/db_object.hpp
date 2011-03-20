#ifndef __ASH_DB_OBJECT__
#define __ASH_DB_OBJECT__

#include <map>
#include <string>

using std::map;
using std::string;

namespace ash {


/* abstract */
class DBObject {
  protected:
    DBObject();
    virtual ~DBObject();

    virtual string get_name() const = 0;  // abstract
    virtual string get_sql() const;

  protected:
    map<string, string> values;

  private:
    DBObject(const DBObject & other);  // disabled
    DBObject & operator =(const DBObject & other);  // disabled
};


}  // namespace ash

#endif /* __ASH_DB_OBJECT__ */
