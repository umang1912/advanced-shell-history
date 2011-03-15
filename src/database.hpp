#ifndef __ASH_DATABASE__
#define __ASH_DATABASE__


class sqlite3;  // Forward declaration.


class Database {
  public:
    Database(const char * filename);
    virtual ~Database();
    int get_session_id();

  private:
    void init_db();
    int select_int(const char * query) const;

  private:
    const char * db_filename;
    sqlite3 * db;
};


#endif /* __ASH_DATABASE__ */
