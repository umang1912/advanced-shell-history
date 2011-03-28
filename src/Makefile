# TODO(cpa): add a copyright here

EXE	:= ash_log
OBJ	:= $(addsuffix .o,${EXE}) command.o database.o db_object.o flags.o session.o unix.o util.o
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

%.o:	%.cpp %.hpp
	${CPP} -c ${FLAGS} -o ${@} ${<}

sqlite3.c:
	wget ${SQLITE_URL}
	unzip ${SQLITE_ZIP}
	mv ${SQLITE_DIR}/sqlite3.* .
	rm -rf ${SQLITE_DIR} ${SQLITE_ZIP}
	
sqlite3.o: sqlite3.c
	gcc -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_THREADSAFE=0 -c sqlite3.c


new:	clean all

distclean:
	rm -f ${TRASH} sqlite3.o

clean:
	rm -f ${TRASH}

reallyclean: clean
	rm -f sqlite3.*
