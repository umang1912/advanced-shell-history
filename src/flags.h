#ifndef __ASH_FLAGS_H__
#define __ASH_FLAGS_H__


#include <getopt.h>
#include <string>
#include <map>


using std::string;
using std::map;


class Flags {
  protected:
    struct Flag {
      union Value {
        bool b;
        const char * c;
        int i;
      };

      Flag(const string & name, const int has_arg, const char short_name, const string & description);
      const struct option & get_option() const;

      const string name;
      const int has_arg;
      const char short_name;
      const string description;
      Value value;
    };

  public:
    void define_bool(const string & name, const char short_name, const bool default_value, const string & description);
    void define_string(const string & name, const char short_name, const string & default_value, const string & description);
    void define_int(const string & name, const char short_name, const int default_value, const string & description);

    string get_string(const string & name) const { return string(get_value(name).c); }
    int get_int(const string & name) const { return get_value(name).i; }
    bool get_bool(const string & name) const { return get_value(name).b; }

    void parse(int argc, char ** argv);

  private:
    struct option * get_long_options() const;
    Flag::Value get_value(const string & name) const;
    Flag::Value & new_value(const string & name, const int has_arg, const char short_name, const string & description);

  private:
    typedef map<string, Flag *> flag_map;
    typedef flag_map::iterator iterator;
    typedef flag_map::const_iterator const_iterator;

    string getopt_str;
    flag_map flags;
    // TODO: ctor, dtor, cctor
};


void Flags::define_bool(const string & name, const char short_name, const bool default_value, const string & description) {
  new_value(name, 0, short_name, description).b = default_value;
}


void Flags::define_string(const string & name, const char short_name, const string & default_value, const string & description) {
  flags[name] = new Flag(name, 0, short_name, description);
  (flags[name] -> value).c = default_value.c_str();
}


void Flags::define_int(const string & name, const char short_name, const int default_value, const string & description) {
  flags[name] = new Flag(name, 0, short_name, description);
  (flags[name] -> value).i = default_value;
}


void Flags::parse(int argc, char ** argv) {
  while (1) {
//    cout << "optind = " << optind << endl;
    int index = 0;
    static struct option long_options[] = {
      {"debug", 0, 0, 'd'},
      {"help", 0, 0, 'h'},
      {"get-session", 0, 0, 's'},
      {"version", 0, 0, 'v'}
    };
    int c = getopt_long(argc, argv, "dhsv", long_options, &index);
    switch (c) {
      case -1: return;
      case 0: break;
      case 'd': break;
      case 'h': break;  // TODO: dump all the valid flags.
      case 's': break;
      case 'v': break;
      default: break;
    }
//    cout << "c = " << c << endl;
  }
}


Flags::Flag::Value & Flags::new_value(const string & name, const int has_arg, const char short_name, const string & description) {
  Flags::Flag * new_flag = new Flags::Flag(name, has_arg, short_name, description);
  flags[name] = new_flag;
  return new_flag -> value;
}


Flags::Flag::Value Flags::get_value(const string & name) const {
  Flags::const_iterator i = flags.find(name);
  if (i == flags.end()) {
    // TODO: emit an error message here.
    exit(1);
  }
  return i -> second -> value;
}

/*
bool Flags::get_bool(const string & name) const {
  return get_value(name).b;
}
*/

// GLOBAL INSTANCE:
Flags FLAGS = Flags();


Flags::Flag::Flag(const string & n, const int h, const char s, const string & d)
  :name(n), has_arg(h), short_name(s), description(d)
{
  if (short_name != 0) {
    // TODO: also check to make sure short_name is not already inside getopt_str.
    FLAGS.getopt_str += short_name;
  }
}



#endif /* __ASH_FLAGS_H__ */
