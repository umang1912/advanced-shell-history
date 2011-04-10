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

LOGGER	:= ash_log
QUERIER	:= ash_query
EXES	:= ${LOGGER} ${QUERIER}
OBJ_L	:= $(addsuffix .o,${LOGGER}) command.o database.o flags.o session.o unix.o util.o
OBJ_Q	:= $(addsuffix .o,${QUERIER}) database.o flags.o
OBJS	:= ${OBJ_L} ${OBJ_Q}
TRASH	:= ${OBJS} ${EXES}
CPP	:= g++
C	:= gcc
FLAGS	:= -g -Wall -O2

SQLITE_DIR := sqlite-amalgamation-3070500
SQLITE_ZIP := ${SQLITE_DIR}.zip
SQLITE_URL := http://www.sqlite.org/${SQLITE_ZIP}


.PHONY:	all
all:	${EXES}

${QUERIER}: sqlite3.o ${OBJ_Q}
	${CPP} ${FLAGS} -o ${@} ${<} ${OBJ_Q}

${LOGGER}: sqlite3.o ${OBJ_L}
	${CPP} ${FLAGS} -o ${@} ${<} ${OBJ_L}

%.o:	%.cpp %.hpp
	${CPP} -c ${FLAGS} -o ${@} ${<}

sqlite3.c:
	wget ${SQLITE_URL}
	unzip ${SQLITE_ZIP}
	mv ${SQLITE_DIR}/sqlite3.* .
	rm -rf ${SQLITE_DIR} ${SQLITE_ZIP}
	
sqlite3.o: sqlite3.c
	${C} -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_THREADSAFE=0 -c sqlite3.c


new:	clean all

distclean:
	rm -f ${TRASH} sqlite3.o

clean:
	rm -f ${TRASH}

reallyclean: clean
	rm -f sqlite3.*
