#ifndef __ASH_FLAGS_H__
#define __ASH_FLAGS_H__


#include <stdlib.h>  /* for atoi */
#include <getopt.h>
#include <string>
#include <vector>
#include <map>

#include <iostream>
using std::cout;
using std::endl;

using std::vector;
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

      Flag(const char * name, const int has_arg, const char short_name, const char * description);
      struct option get_option() const;

      const char * name;
      const int has_arg;
      const char short_name;
      const char * description;
      Value value;
    };

  public:
    Flags() : next_key(0) {}
    ~Flags();

    void parse(int argc, char ** argv);
    void usage() const;

    void define_bool(const char * name, const char short_name, const bool default_value, const char * description);
    void define_string(const char * name, const char short_name, const char * default_value, const char * description);
    void define_int(const char * name, const char short_name, const char * default_value, const char * description);

    // long names:
    string get_string(const char * name) const { return string(get_value(name).c); }
    int get_int(const char * name) const { return get_value(name).i; }
    bool get_bool(const char * name) const { return get_value(name).b; }

    // short names:
    string get_string(const char short_name) const { return string(get_value(short_name).c); }
    int get_int(const char short_name) const { return get_value(short_name).i; }
    bool get_bool(const char short_name) const { return get_value(short_name).b; }

  private:
    struct option * get_long_options();
    void set_value(const char * name, Flag::Value value);
    Flag::Value get_value(const char short_name) const;
    Flag::Value get_value(const char * name) const;
    Flag::Value & new_value(const char * name, const int has_arg, const char short_name, const char * description);

  private:
    typedef map<const char *, int> key_map;
    typedef map<const int, Flag *> flag_map;
    typedef key_map::iterator key_iterator;
    typedef key_map::const_iterator const_key_iterator;
    typedef flag_map::iterator flag_iterator;
    typedef flag_map::const_iterator const_flag_iterator;

    string getopt_str;
    key_map keys;
    flag_map flags;
    int next_key;
};


void Flags::usage() const {
  // TODO: dump flag descriptions and quit.
  // TODO: add a string arg for a message?
}


void Flags::define_bool(const char * name, const char short_name, const bool default_value, const char * description) {
  new_value(name, 0, short_name, description).b = default_value;
}


void Flags::define_string(const char * name, const char short_name, const char * default_value, const char * description) {
  new_value(name, default_value ? 2 : 1, short_name, description).c = default_value;
}


void Flags::define_int(const char * name, const char short_name, const char * default_value, const char * description) {
  new_value(name, default_value ? 2 : 1, short_name, description).i = atoi(default_value);
}


struct option Flags::Flag::get_option() const {
  struct option opt = {name, has_arg, 0, short_name};
  return opt;
}


struct option * Flags::get_long_options() {
  vector<struct option> long_opts;
  for (flag_iterator i = flags.begin(); i != flags.end(); ++i) {
    if (i -> second -> name != 0) {
      long_opts.push_back(i -> second -> get_option());
    }
  }
  if (long_opts.empty()) return 0;
  struct option * rval = new struct option[long_opts.size()];
  for (unsigned int i = 0, end = long_opts.size(); i < end; ++i) {
    rval[i] = long_opts[i];
  }
  return rval;
}


void Flags::parse(int argc, char ** argv) {
  static struct option * long_options = get_long_options();
  while (1) {
    int index = 0;
    int c = getopt_long(argc, argv, getopt_str.c_str(), long_options, &index);
cout << "c = " << c << endl;
    const char * name = 0;
    switch (c) {
      case -1: return;
      case '?': cout << "WOAH" << endl; break;
      case 0: name = long_options[index].name;
        break;
      default: name = (string("-") + ((char) c)).c_str();
        break;
    }
cout << "name = '" << name << "'" << endl;
    if (name && optarg) {
      get_value(name).c = optarg;
    }
  }
}


Flags::Flag::Value & Flags::new_value(const char * name, const int has_arg, const char short_name, const char * description) {
  const char * short_n = short_name == 0 ? 0 : (string("-") + short_name).c_str();
  if (name == 0 && short_name == 0) {
    // TODO: emit an error message - no arg name.
    exit(1);
  }
  int key = next_key++;
  if (name) keys[name] = key;
  if (short_n) keys[name] = key;
  Flags::Flag * new_flag = new Flags::Flag(name, has_arg, short_name, description);
  flags[key] = new_flag;
  return new_flag -> value;
}


Flags::~Flags() {
  for (flag_iterator i = flags.begin(); i != flags.end(); ++i) {
    if (i -> second != 0)
      delete i -> second;
    i -> second = 0;
  }
}


Flags::Flag::Value Flags::get_value(const char * name) const {
  const_key_iterator i = keys.find(name);
  if (i == keys.end()) {
cout << "key not found for key: '" << name << "'" << endl;
    // TODO: emit an error message here.
    exit(1);
  }
  int key = i -> second;
  const_flag_iterator j = flags.find(key);
  if (j == flags.end()) {
    // TODO: emit an error.
    exit(1);
  }
  return j -> second -> value;
}


Flags::Flag::Value Flags::get_value(const char short_name) const {
  if (short_name == 0) {
    // TODO: emit an error.
    exit(1);
  }
  return get_value((string("-") + short_name).c_str());
}


// GLOBAL SINGLETON INSTANCE:
Flags FLAGS = Flags();


Flags::Flag::Flag(const char * n, const int h, const char s, const char * d)
  :name(n), has_arg(h), short_name(s), description(d)
{
  if (short_name != 0) {
    if (FLAGS.getopt_str.find(short_name) == string::npos) {
      FLAGS.getopt_str += short_name;
      if (has_arg != 0)
        FLAGS.getopt_str += ':';
    } else {
      // TODO: echo an error message and die.
      exit(1);
    }
  }
}



#endif /* __ASH_FLAGS_H__ */

