# Introduction #

The best part about having your history data saved in a database is being able to use SQL to select interesting data.  If you don't already know SQL, this can be troublesome.  No worries!  Fortunately this is a [relatively trivial DB schema](DBSchema.md) and this guide will help you get started.


## `ash_query` ##

You can use the `ash_query` tool to inspect any of the existing saved queries.
```
  $ ash_query -Q  # list the available saved queries
```
On my system, this prints the following:
```
Query              Description                                                   
CWD                Shows the history for the current working directory only.     
DEMO               Shows who, what, where and when (not WHY).                    
RCWD               Shows the history rooted at the current working directory.    
SESSION_HISTORY    Shows a session-by-session breakdown of history.              
sessions           Lists all sessions.         
```
Some of these queries are provided by default and some are written by me.

## Configuration ##
The queries listed by `ash_query -Q` are loaded from up to two separate configuration files:
  * `/etc/ash/queries` - this is the default location of the system-wide set of saved queries.  You probably won't be editing this for normal use.
  * `~/.ash/queries` - this is the default location of the individual user-level set of saved queries.  You can create new queries, name them and save them in this file for easy invocation later.

## Example ##
Let's take a look at an example query.
```
  $ ash_query -p CWD  # print the SQL for the 'CWD' query.
```
This prints the following to the terminal:
```
Query: CWD
Template Form:

    select
      datetime(c.start_time, 'unixepoch', 'localtime') as "when",
      c.command as "what"
    from
      commands as c
    where
      c.cwd = '${PWD}' or c.cwd = '/${PWD}' or c.cwd = '/${PWD#//}'
    ;
  
Actual SQL:

    select
      datetime(c.start_time, 'unixepoch', 'localtime') as "when",
      c.command as "what"
    from
      commands as c
    where
      c.cwd = '/home/me' or c.cwd = '//home/me' or c.cwd = '//home/me'
    ;
```

**Note:** _if you are curious about the_ `'/${PWD}'` _and_ `'/${PWD#//}'` _literals in the query above, take a peek at [this explanation](DoubleSlash.md)._

Notice that there is a **template form** and an **actual SQL** form.  If you look at the entry in `/etc/ash/queries`, you will find this:
```
...
CWD: {
  description: "Shows the history for the current working directory only."
  sql: {
    select
      datetime(c.start_time, 'unixepoch', 'localtime') as "when",
      c.command as "what"
    from
      commands as c
    where
      c.cwd = '${PWD}' or c.cwd = '${PWD}' or c.cwd = '/${PWD#//}'
    ;
  }
}
...
```

Because the SQL includes shell variables `${PWD`} - this is a _templatized_ query.  This means that before executing the query, the variables will be _expanded_.

The expanded form is also printed as the **Actual SQL** above - notice how the `${PWD`} variable was replaced with a literal value `/home/me`.

Variable expansion happens immediately before the query is executed.  Under the hood, this is performed by making a `popen` call and using a shell [here-document.](http://tldp.org/LDP/abs/html/here-docs.html)

Essentially, this looks like:
```
  cat <<EOF_ASH_SQL
    select
      datetime(c.start_time, 'unixepoch', 'localtime') as "when",
      c.command as "what"
    from
      commands as c
    where
      c.cwd = '${PWD}' or c.cwd = '/${PWD}' or c.cwd = '/${PWD#//}'
    ;
EOF_ASH_SQL
```
This expansion will only happen for the query you requested and it will happen right before it is printed or executed.


## Writing Queries ##

The best way to write new queries is to use the interactive SQL editor provided by [SQLite](http://www.sqlite.org/index.html).  You can start an interactive session with your history database like this:
```
  $ sqlite3 ~/.ash/history.db
```
You should see something like this:
```
SQLite version 3.7.2
Enter ".help" for instructions
Enter SQL statements terminated with a ";"
sqlite> 
```

Please refer to the [schema description](DBSchema.md) for more details about the individual data pieces.  Also, take a look at the `.help` ouput.  Finally, take a look at the various [SQLite documents.](http://www.sqlite.org/docs.html)

## Debugging ##

If you are having problems understanding why something is not working as expected, please see this guide regarding [how to view debug info.](HOWTO_Debug.md)


## References ##

[SQLite SQL Reference](http://www.sqlite.org/lang.html)


# [Next](HOWTO_Contribute.md) #

If you've been following along through the HOWTO documents, you're done with the tutorial!  If you have a fun query to share, [please feel free to share it.](HOWTO_Contribute.md)  Also, if you have an idea for a new feature or you found a bug in the code, [please share it!](HOWTO_Contribute.md)

This is a new program; if you have feedback, I would love to hear it.  [Raise an issue](http://code.google.com/p/advanced-shell-history/issues/entry) and I'll get back to you.
```
 0/ - Have fun storming the castle!
<Y
/ \
```