#!/bin/bash
#
# Copyright (c) Carl Anderson, 2011.  All rights reserved.
#
set -e
set -u


SQLITE_DIR="sqlite-amalgamation-3070500"
SQLITE_ZIP="${SQLITE_DIR}.zip"
SQLITE_URL="http://www.sqlite.org/${SQLITE_ZIP}"


# Download the sqlite3 tarball.
if [[ ! -e "${SQLITE_ZIP}" ]]; then
  if which wget &>/dev/null; then
    wget "${SQLITE_URL}"

  elif which curl &>/dev/null; then
    curl -s "${SQLITE_URL}" -o "${SQLITE_ZIP}"

  else
    echo "Please download ${SQLITE_ZIP} and place in this directory."

  fi
fi


# Unpack the tarball and move the contents into this directory.
unzip "${SQLITE_ZIP}"
mv -f "${SQLITE_DIR}"/sqlite3.* .

# Clean up.
rm -rf "${SQLITE_DIR}" "${SQLITE_ZIP}"
