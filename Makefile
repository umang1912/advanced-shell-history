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
MAN_DIR  := /usr/share/man/man1
SRC_DEST := ..


.PHONY: all build clean install man mrproper src_tarball src_tarball_minimal uninstall version
all:	version build man

new:	clean all

version:
	sed -i -e "/^VERSION :=/s/:= .*/:= ${RVERSION}/" src/Makefile

build:
	@ cd src && make
	chmod 555 src/_ash_log src/ash_query
	cp -af src/_ash_log src/ash_query files/usr/local/bin

man:
	sed -e "s:__VERSION__:Version ${RVERSION}:" man/_ash_log.1 \
	  | sed -e "s:__DATE__:$$( stat -c %y man/_ash_log.1 | cut -d' ' -f1 ):" \
	  | gzip -9 -c > ./files${MAN_DIR}/_ash_log.1.gz
	sed -e "s:__VERSION__:Version ${RVERSION}:" man/ash_query.1 \
	  | sed -e "s:__DATE__:$$( stat -c %y man/ash_query.1 | cut -d' ' -f1 ):" \
	  | gzip -9 -c > ./files${MAN_DIR}/ash_query.1.gz

install: uninstall build man
	@ echo "\nInstalling files:"
	@ cd files && \
	sudo tar -cpO $$( find -type f -o -type l | grep -v '\.svn' ) | sudo tar -xpvC /
	@ echo "\n 0/ - Install completed!\n<Y\n/ \\"

uninstall:
	sudo rm -rf /etc/ash /usr/lib/advanced_shell_history
	sudo rm -f /usr/local/bin/_ash_log /usr/local/bin/ash_query
	sudo rm -f ${MAN_DIR}/_ash_log.1.gz ${MAN_DIR}/ash_query.1.gz
	sudo rm -f ${MAN_DIR}/advanced_shell_history

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
	rm -f files/usr/local/bin/_ash_log
	rm -f files/usr/local/bin/ash_query
	rm -f files/usr/share/man/man1/_ash_log.1.gz
	rm -f files/usr/share/man/man1/ash_query.1.gz
	rm -rf ${TMP_DIR} ${TMP_FILE}
