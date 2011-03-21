#ifndef __ASH_DB_OBJECT__
#define __ASH_DB_OBJECT__

#include <list>
#include <map>
#include <string>

using std::list;
using std::map;
using std::string;

namespace ash {


/* abstract */
class DBObject {
  public:
    static const string quote(const char * value);
    static const string quote(const string & value);
    static const string get_create_tables();

  protected:
    static void register_table(const string & create_statement);

    static list<string> create_tables;

  protected:
    DBObject();
    virtual ~DBObject();

    virtual const string get_create_table() const = 0;  // abstract
    virtual const string get_name() const = 0;  // abstract
    virtual const string get_sql() const;

    map<string, string> values;

  private:
    DBObject(const DBObject & other);  // disabled
    DBObject & operator =(const DBObject & other);  // disabled
};


}  // namespace ash

#endif /* __ASH_DB_OBJECT__ */
