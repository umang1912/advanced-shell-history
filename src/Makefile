#
#   Copyright 2011 Carl Anderson
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#

VERSION := 0.1.r54
LOGGER	:= ash_log
QUERIER	:= ash_query
EXES	:= ${LOGGER} ${QUERIER}
OBJ_L	:= ${LOGGER}.o command.o config.o database.o flags.o logger.o session.o unix.o util.o
OBJ_Q	:= ${QUERIER}.o config.o flags.o logger.o
OBJS	:= ${OBJ_L} ${OBJ_Q}
CPPS	:= $(shell ls *.cpp)
TRASH	:= ${OBJS} ${EXES} core
CPP	:= g++
C	:= gcc
FLAGS	:= -g -Wall -O2 -DASH_VERSION="\"${VERSION}\"" -ansi -pedantic

.PHONY:	all clean distclean new reallyclean
all:	${EXES}

${QUERIER}: sqlite3.o ${OBJ_Q}
	${CPP} ${FLAGS} -o ${@} ${<} ${OBJ_Q}

${LOGGER}: sqlite3.o ${OBJ_L}
	${CPP} ${FLAGS} -o ${@} ${<} ${OBJ_L}

%.o:	%.cpp %.hpp
	${CPP} -c ${FLAGS} -o ${@} ${<}

sqlite3.c:
	./fetch_sqlite.sh
	
sqlite3.o: sqlite3.c
	${C} -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_THREADSAFE=0 -c sqlite3.c


new:	clean all

distclean:
	rm -f ${TRASH} sqlite3.o

clean:
	rm -f ${TRASH}

reallyclean: clean
	rm -f sqlite3.*

# This awesome target attempts to inject the exactly-right CPP dependencies
# into this Makefile whenever a CPP file is edited.
#
# This is because, by default, the generic %.o build rule will rebuild only
# when the similarly named hpp or cpp files are changed.  For example, if
# util.hpp has a new function added to it and we really want to rebuild
# anything that #includes "util.hpp" - make has no way of knowing which files
# include util.hpp unless build rules are maintained by hand.
#
# This handy target does some minor magic to represent the include dependencies
# and keep the Makefile up-to-date.
#
Makefile: ${CPPS}
	sed -i -e '/^# DEPENDENCIES:/q' Makefile
	grep '^#include "' *.cpp \
	  | sed -e 's/:#include / /;s:"::g' \
	  | awk '{ \
	      if (cpp != "" && $$1 != cpp) { \
	        print cpp ":" includes; \
	        includes = ""; \
	      } \
	      cpp = $$1; \
	      includes = includes " " $$2; \
	    }' \
	  | sed -e 's/.cpp:/.o:/' >> Makefile
#
#     Note: the dependencies below are maintained AUTOMATICALLY.
#   0/      Do NOT edit or add anyting below this line - it will be DELETED!
#  <Y
#  / \
#
# DEPENDENCIES: (Do not edit this line!)
ash_log.o: ash_log.hpp command.hpp config.hpp flags.hpp logger.hpp session.hpp unix.hpp
ash_query.o: ash_query.hpp flags.hpp
command.o: ash_log.hpp command.hpp unix.hpp util.hpp
config.o: config.hpp
database.o: database.hpp config.hpp logger.hpp sqlite3.h
flags.o: flags.hpp
logger.o: logger.hpp config.hpp
session.o: session.hpp ash_log.hpp unix.hpp
unix.o: unix.hpp database.hpp util.hpp
