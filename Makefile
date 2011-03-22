# TODO(cpa): make a deb package
# TODO(cpa): add a deb_install target to make a deb and install it.

REV := $(shell svn up | cut -d' ' -f3 | cut -d. -f1 | sed -e 's:^:.r:' )
VERSION  := 0.1
RVERSION := ${VERSION}${REV}
TMP_ROOT := /tmp
TMP_DIR  := ${TMP_ROOT}/ash-${VERSION}
TMP_FILE := ${TMP_DIR}.tar.gz
SRC_DEST := ..


all:	build

build:
	cd src && make
	cp src/ash_log src/ash_exit files/usr/local/bin

install: build
	sudo rsync -Ca files/* /

src_tarball: clean
	mkdir -p ${TMP_DIR}
	rsync -Ca * ${TMP_DIR}
	cd ${TMP_ROOT} && tar -czpf ${TMP_FILE} ./ash-${VERSION}/
	rm -rf ${TMP_DIR}
	mv ${TMP_FILE} ${SRC_DEST}/ash-${RVERSION}.tar.gz

clean:
	cd src && make distclean
	rm -f files/usr/local/bin/ash_*
	rm -rf ${TMP_DIR} ${TMP_FILE}
