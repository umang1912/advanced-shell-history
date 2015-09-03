# Introduction #

To give you an idea of the different formatters, I'll present a sample session displayed in each (including the standard bash history output).


## `bash builtin history` ##
```
me@my_host:~/code/advanced-shell-history/trunk/src$ builtin history
...
  996  1317183663 source /usr/lib/advanced_shell_history/bash 
  997  1317184129 ls
  998  1317184149 setsid gedit formatter.*pp 
  999  1317184368 source /usr/lib/advanced_shell_history/bash
 1000  1317198492 ls
 1001  1317274932 cd code/
 1002  1317274948 ls
 1003  1317274955 ash_query --version
 1004  1317274958 ls
 1005  1317274983 ash_query -Q
 1006  1317274990 ash_query -q SH
 1007  1317275002 cd advanced-shell-history/
 1008  1317275006 ash_query -q SH
 1009  1317275014 ash_query -q SH
 1010  1317275028 ls
 1011  1317275030 ash_query -q SH
 1012  1317275083 ls
 1013  1317275086 cd trunk/
 1014  1317275090 cd src/
 1015  1317275091 ls
 1016  1317275154 ls
 1017  1317275159 history
 1018  1317275205 ls
 1019  1317275296 nano TODO 
 1020  1317275480 nano ../files/etc/ash/queries 
 1021  1317275825 ash_query -q SESSION
 1022  1317275836 ash_query -q SESSION
 1023  1317275936 nano ../files/etc/ash/queries 
 1024  1317275977 nano ../files/etc/ash/queries 
 1025  1317276374 ash_query -q ME
 1026  1317276379 nano ../files/etc/ash/queries 
 1027  1317276413 nano ~/.ash/ash.log 
 1028  1317276455 nano ../files/etc/ash/queries 
 1029  1317276527 ash_query -q ME
 1030  1317276532 nano ~/.ash/ash.log 
 1031  1317276560 nano ../files/etc/ash/queries 
 1032  1317276580 ash_query -q ME
 1033  1317276635 nano ../files/etc/ash/queries 
 1034  1317276713 ash_query -q ME
 1035  1317276893 nano ../files/etc/ash/queries 
 1036  1317276926 ash_query -q ME
```
Notice that the first few lines shown are actually for a totally different session.  Shell builtin history has no way of distinguishing sessions, so all are shown interlaced.

## Formatters ##
The following examples are all generated from the same query:
```
ME: {
  description: "Select the history for just the current session."
  sql: {
    select
      c.cwd as 'current_working_dir',
      c.command_no as 'cmd',
      datetime(c.start_time, 'unixepoch', 'localtime') as start_time,
      c.duration as 'secs',
      c.rval as exit,
      c.command
    from
      commands as c
      left outer join sessions as s
        on c.session_id = s.id
    where
      c.session_id = ${ASH_SESSION_ID:-0}
    order by
      c.id
    ;
  }
}
```
The only difference in each example below is the formatter.


### `auto` ###
> The most sophisticated formatter (and the default); it tries to group identical values in the leftmost columns and print the unique bits of data aligned.  The point is to try to reduce the total 'area' of printed space.

```
me@my_host:~/code/advanced-shell-history/trunk/src$ ash_query -q ME -f auto
current_working_dir
    cmd     start_time             secs    exit    command
/home/me
    1001    2011-09-28 22:42:12    0       0       cd code/
/home/me/code
    1002    2011-09-28 22:42:28    0       0       ls
    1003    2011-09-28 22:42:35    0       0       ash_query --version
    1004    2011-09-28 22:42:38    0       0       ls
    1005    2011-09-28 22:43:03    0       0       ash_query -Q
    1006    2011-09-28 22:43:10    0       0       ash_query -q SH
    1007    2011-09-28 22:43:22    0       0       cd advanced-shell-history/
/home/me/code/advanced-shell-history
    1008    2011-09-28 22:43:26    0       0       ash_query -q SH
    1009    2011-09-28 22:43:34    0       0       ash_query -q SH
    1010    2011-09-28 22:43:48    0       0       ls
    1011    2011-09-28 22:43:50    0       0       ash_query -q SH
    1012    2011-09-28 22:44:43    0       0       ls
    1013    2011-09-28 22:44:46    0       0       cd trunk/
/home/me/code/advanced-shell-history/trunk
    1014    2011-09-28 22:44:50    0       0       cd src/
/home/me/code/advanced-shell-history/trunk/src
    1015    2011-09-28 22:44:51    0       0       ls
    1016    2011-09-28 22:45:54    0       0       ls
    1017    2011-09-28 22:45:59    0       0       history
    1018    2011-09-28 22:46:45    0       0       ls
    1019    2011-09-28 22:48:16    83      0       nano TODO
    1020    2011-09-28 22:51:20    337     0       nano ../files/etc/ash/queries
    1021    2011-09-28 22:57:05    0       1       ash_query -q SESSION
    1022    2011-09-28 22:57:16    0       0       ash_query -q SESSION
    1023    2011-09-28 22:58:56    23      0       nano ../files/etc/ash/queries
    1024    2011-09-28 22:59:37    382     0       nano ../files/etc/ash/queries
    1025    2011-09-28 23:06:14    0       1       ash_query -q ME
    1026    2011-09-28 23:06:19    23      0       nano ../files/etc/ash/queries
    1027    2011-09-28 23:06:53    33      0       nano ~/.ash/ash.log
    1028    2011-09-28 23:07:35    64      0       nano ../files/etc/ash/queries
    1029    2011-09-28 23:08:47    0       1       ash_query -q ME
    1030    2011-09-28 23:08:52    25      0       nano ~/.ash/ash.log
    1031    2011-09-28 23:09:20    12      0       nano ../files/etc/ash/queries
    1032    2011-09-28 23:09:40    0       0       ash_query -q ME
    1033    2011-09-28 23:10:35    70      0       nano ../files/etc/ash/queries
    1034    2011-09-28 23:11:53    0       0       ash_query -q ME
    1035    2011-09-28 23:14:53    30      0       nano ../files/etc/ash/queries
    1036    2011-09-28 23:15:26    0       0       ash_query -q ME
```

### `aligned` ###
> Prints the values left-aligned and with fixed widths large enough to hold the widest value (with a maximum width of 80 per column).

```
me@my_host:~/code/advanced-shell-history/trunk/src$ ash_query -q ME -f aligned
current_working_dir                               cmd     start_time             secs    exit    command
/home/me                                          1001    2011-09-28 22:42:12    0       0       cd code/
/home/me/code                                     1002    2011-09-28 22:42:28    0       0       ls
/home/me/code                                     1003    2011-09-28 22:42:35    0       0       ash_query --version
/home/me/code                                     1004    2011-09-28 22:42:38    0       0       ls
/home/me/code                                     1005    2011-09-28 22:43:03    0       0       ash_query -Q
/home/me/code                                     1006    2011-09-28 22:43:10    0       0       ash_query -q SH
/home/me/code                                     1007    2011-09-28 22:43:22    0       0       cd advanced-shell-history/
/home/me/code/advanced-shell-history              1008    2011-09-28 22:43:26    0       0       ash_query -q SH
/home/me/code/advanced-shell-history              1009    2011-09-28 22:43:34    0       0       ash_query -q SH
/home/me/code/advanced-shell-history              1010    2011-09-28 22:43:48    0       0       ls
/home/me/code/advanced-shell-history              1011    2011-09-28 22:43:50    0       0       ash_query -q SH
/home/me/code/advanced-shell-history              1012    2011-09-28 22:44:43    0       0       ls
/home/me/code/advanced-shell-history              1013    2011-09-28 22:44:46    0       0       cd trunk/
/home/me/code/advanced-shell-history/trunk        1014    2011-09-28 22:44:50    0       0       cd src/
/home/me/code/advanced-shell-history/trunk/src    1015    2011-09-28 22:44:51    0       0       ls
/home/me/code/advanced-shell-history/trunk/src    1016    2011-09-28 22:45:54    0       0       ls
/home/me/code/advanced-shell-history/trunk/src    1017    2011-09-28 22:45:59    0       0       history
/home/me/code/advanced-shell-history/trunk/src    1018    2011-09-28 22:46:45    0       0       ls
/home/me/code/advanced-shell-history/trunk/src    1019    2011-09-28 22:48:16    83      0       nano TODO
/home/me/code/advanced-shell-history/trunk/src    1020    2011-09-28 22:51:20    337     0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1021    2011-09-28 22:57:05    0       1       ash_query -q SESSION
/home/me/code/advanced-shell-history/trunk/src    1022    2011-09-28 22:57:16    0       0       ash_query -q SESSION
/home/me/code/advanced-shell-history/trunk/src    1023    2011-09-28 22:58:56    23      0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1024    2011-09-28 22:59:37    382     0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1025    2011-09-28 23:06:14    0       1       ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src    1026    2011-09-28 23:06:19    23      0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1027    2011-09-28 23:06:53    33      0       nano ~/.ash/ash.log
/home/me/code/advanced-shell-history/trunk/src    1028    2011-09-28 23:07:35    64      0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1029    2011-09-28 23:08:47    0       1       ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src    1030    2011-09-28 23:08:52    25      0       nano ~/.ash/ash.log
/home/me/code/advanced-shell-history/trunk/src    1031    2011-09-28 23:09:20    12      0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1032    2011-09-28 23:09:40    0       0       ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src    1033    2011-09-28 23:10:35    70      0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1034    2011-09-28 23:11:53    0       0       ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src    1035    2011-09-28 23:14:53    30      0       nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src    1036    2011-09-28 23:15:26    0       0       ash_query -q ME
```

### `csv` ###
```
me@my_host:~/code/advanced-shell-history/trunk/src$ ash_query -q ME -f csv
current_working_dir,cmd,start_time,secs,exit,command
/home/me,1001,2011-09-28 22:42:12,0,0,cd code/
/home/me/code,1002,2011-09-28 22:42:28,0,0,ls
/home/me/code,1003,2011-09-28 22:42:35,0,0,ash_query --version
/home/me/code,1004,2011-09-28 22:42:38,0,0,ls
/home/me/code,1005,2011-09-28 22:43:03,0,0,ash_query -Q
/home/me/code,1006,2011-09-28 22:43:10,0,0,ash_query -q SH
/home/me/code,1007,2011-09-28 22:43:22,0,0,cd advanced-shell-history/
/home/me/code/advanced-shell-history,1008,2011-09-28 22:43:26,0,0,ash_query -q SH
/home/me/code/advanced-shell-history,1009,2011-09-28 22:43:34,0,0,ash_query -q SH
/home/me/code/advanced-shell-history,1010,2011-09-28 22:43:48,0,0,ls
/home/me/code/advanced-shell-history,1011,2011-09-28 22:43:50,0,0,ash_query -q SH
/home/me/code/advanced-shell-history,1012,2011-09-28 22:44:43,0,0,ls
/home/me/code/advanced-shell-history,1013,2011-09-28 22:44:46,0,0,cd trunk/
/home/me/code/advanced-shell-history/trunk,1014,2011-09-28 22:44:50,0,0,cd src/
/home/me/code/advanced-shell-history/trunk/src,1015,2011-09-28 22:44:51,0,0,ls
/home/me/code/advanced-shell-history/trunk/src,1016,2011-09-28 22:45:54,0,0,ls
/home/me/code/advanced-shell-history/trunk/src,1017,2011-09-28 22:45:59,0,0,history
/home/me/code/advanced-shell-history/trunk/src,1018,2011-09-28 22:46:45,0,0,ls
/home/me/code/advanced-shell-history/trunk/src,1019,2011-09-28 22:48:16,83,0,nano TODO
/home/me/code/advanced-shell-history/trunk/src,1020,2011-09-28 22:51:20,337,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1021,2011-09-28 22:57:05,0,1,ash_query -q SESSION
/home/me/code/advanced-shell-history/trunk/src,1022,2011-09-28 22:57:16,0,0,ash_query -q SESSION
/home/me/code/advanced-shell-history/trunk/src,1023,2011-09-28 22:58:56,23,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1024,2011-09-28 22:59:37,382,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1025,2011-09-28 23:06:14,0,1,ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src,1026,2011-09-28 23:06:19,23,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1027,2011-09-28 23:06:53,33,0,nano ~/.ash/ash.log
/home/me/code/advanced-shell-history/trunk/src,1028,2011-09-28 23:07:35,64,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1029,2011-09-28 23:08:47,0,1,ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src,1030,2011-09-28 23:08:52,25,0,nano ~/.ash/ash.log
/home/me/code/advanced-shell-history/trunk/src,1031,2011-09-28 23:09:20,12,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1032,2011-09-28 23:09:40,0,0,ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src,1033,2011-09-28 23:10:35,70,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1034,2011-09-28 23:11:53,0,0,ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src,1035,2011-09-28 23:14:53,30,0,nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src,1036,2011-09-28 23:15:26,0,0,ash_query -q ME
```

### `null` ###
```
me@my_host:~/code/advanced-shell-history/trunk/src$ ash_query -q ME -f null
current_working_dir^@cmd^@start_time^@secs^@exit^@command
/home/me^@1001^@2011-09-28 22:42:12^@0^@0^@cd code/
/home/me/code^@1002^@2011-09-28 22:42:28^@0^@0^@ls
/home/me/code^@1003^@2011-09-28 22:42:35^@0^@0^@ash_query --version
/home/me/code^@1004^@2011-09-28 22:42:38^@0^@0^@ls
/home/me/code^@1005^@2011-09-28 22:43:03^@0^@0^@ash_query -Q
/home/me/code^@1006^@2011-09-28 22:43:10^@0^@0^@ash_query -q SH
/home/me/code^@1007^@2011-09-28 22:43:22^@0^@0^@cd advanced-shell-history/
/home/me/code/advanced-shell-history^@1008^@2011-09-28 22:43:26^@0^@0^@ash_query -q SH
/home/me/code/advanced-shell-history^@1009^@2011-09-28 22:43:34^@0^@0^@ash_query -q SH
/home/me/code/advanced-shell-history^@1010^@2011-09-28 22:43:48^@0^@0^@ls
/home/me/code/advanced-shell-history^@1011^@2011-09-28 22:43:50^@0^@0^@ash_query -q SH
/home/me/code/advanced-shell-history^@1012^@2011-09-28 22:44:43^@0^@0^@ls
/home/me/code/advanced-shell-history^@1013^@2011-09-28 22:44:46^@0^@0^@cd trunk/
/home/me/code/advanced-shell-history/trunk^@1014^@2011-09-28 22:44:50^@0^@0^@cd src/
/home/me/code/advanced-shell-history/trunk/src^@1015^@2011-09-28 22:44:51^@0^@0^@ls
/home/me/code/advanced-shell-history/trunk/src^@1016^@2011-09-28 22:45:54^@0^@0^@ls
/home/me/code/advanced-shell-history/trunk/src^@1017^@2011-09-28 22:45:59^@0^@0^@history
/home/me/code/advanced-shell-history/trunk/src^@1018^@2011-09-28 22:46:45^@0^@0^@ls
/home/me/code/advanced-shell-history/trunk/src^@1019^@2011-09-28 22:48:16^@83^@0^@nano TODO
/home/me/code/advanced-shell-history/trunk/src^@1020^@2011-09-28 22:51:20^@337^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1021^@2011-09-28 22:57:05^@0^@1^@ash_query -q SESSION
/home/me/code/advanced-shell-history/trunk/src^@1022^@2011-09-28 22:57:16^@0^@0^@ash_query -q SESSION
/home/me/code/advanced-shell-history/trunk/src^@1023^@2011-09-28 22:58:56^@23^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1024^@2011-09-28 22:59:37^@382^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1025^@2011-09-28 23:06:14^@0^@1^@ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src^@1026^@2011-09-28 23:06:19^@23^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1027^@2011-09-28 23:06:53^@33^@0^@nano ~/.ash/ash.log
/home/me/code/advanced-shell-history/trunk/src^@1028^@2011-09-28 23:07:35^@64^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1029^@2011-09-28 23:08:47^@0^@1^@ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src^@1030^@2011-09-28 23:08:52^@25^@0^@nano ~/.ash/ash.log
/home/me/code/advanced-shell-history/trunk/src^@1031^@2011-09-28 23:09:20^@12^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1032^@2011-09-28 23:09:40^@0^@0^@ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src^@1033^@2011-09-28 23:10:35^@70^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1034^@2011-09-28 23:11:53^@0^@0^@ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src^@1035^@2011-09-28 23:14:53^@30^@0^@nano ../files/etc/ash/queries
/home/me/code/advanced-shell-history/trunk/src^@1036^@2011-09-28 23:15:26^@0^@0^@ash_query -q ME
/home/me/code/advanced-shell-history/trunk/src^@1037^@2011-09-28 23:17:15^@0^@0^@ash_query -q ME
```