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
