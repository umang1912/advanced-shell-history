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

REV := $(shell svn up | cut -d' ' -f3 | cut -d. -f1 | sed -e 's:^:.r:' )
VERSION  := 0.1
RVERSION := ${VERSION}${REV}
TMP_ROOT := /tmp
TMP_DIR  := ${TMP_ROOT}/ash-${VERSION}
TMP_FILE := ${TMP_DIR}.tar.gz
SRC_DEST := ..


.PHONY: all build clean install mrproper src_tarball src_tarball_minimal uninstall version
all:	version build

new:	clean all

version:
	sed -i -e "/^VERSION :=/s/:= .*/:= ${RVERSION}/" src/Makefile

build:
	cd src && make
	chmod 555 src/ash_log src/ash_query
	cp -af src/ash_log src/ash_query files/usr/local/bin

install: uninstall build
	cd files && sudo find -type f \
	  | grep -v '\.svn' \
	  | xargs -I{} dirname {} \
	  | xargs -I{} mkdir -p /{}
	cd files && sudo find -type f \
	  | grep -v '\.svn' \
	  | xargs -I{} cp -fa {} /{}

uninstall:
	sudo rm -rf /etc/ash /usr/lib/advanced_shell_history
	sudo rm -f /usr/local/bin/ash_{log,query}

src_tarball_minimal: mrproper src_tarball

src_tarball: clean
	mkdir -p ${TMP_DIR}
	rsync -Ca * ${TMP_DIR}
	cd ${TMP_ROOT} && tar -czpf ${TMP_FILE} ./ash-${VERSION}/
	rm -rf ${TMP_DIR}
	mv ${TMP_FILE} ${SRC_DEST}/ash-${RVERSION}.tar.gz

mrproper: clean
	rm -f src/sqlite3.*

clean:	version
	cd src && make distclean
	rm -f files/usr/local/bin/ash_*
	rm -rf ${TMP_DIR} ${TMP_FILE}
