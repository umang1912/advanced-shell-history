# TODO(cpa): add a copyright here

EXE	:= ash_log
OBJ	:= $(addsuffix .o,${EXE}) database.o
TRASH	:= ${OBJ} ${EXE}
CPP	:= g++
FLAGS	:= -g -Wall -O2

SQLITE_DIR := sqlite-amalgamation-3070500
SQLITE_ZIP := ${SQLITE_DIR}.zip
SQLITE_URL := http://www.sqlite.org/${SQLITE_ZIP}


.PHONY:	all
all:	${EXE}

${EXE}:	sqlite3.o ${OBJ}
	${CPP} ${FLAGS} -o ${@} ${<} ${OBJ}

%.o:	%.cpp
	${CPP} -c ${FLAGS} -o ${@} ${<}

database.o: sqlite3.o database.hpp database.cpp
	${CPP} -c ${FLAGS} -o ${@} database.cpp

sqlite3.o:
	wget ${SQLITE_URL}
	unzip ${SQLITE_ZIP}
	mv ${SQLITE_DIR}/sqlite3.* .
	gcc -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_THREADSAFE=0 -c sqlite3.c
	rm -rf ${SQLITE_DIR} ${SQLITE_ZIP}


distclean:
	rm -f ${OBJ}

clean:
	rm -rf ${TRASH}

reallyclean: clean
	rm -rf sqlite3.o
