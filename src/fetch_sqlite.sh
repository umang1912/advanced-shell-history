#!/bin/bash
#
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
#
# This script attempts to fetch the SQLite3 amalgamation tarball using either
# wget or curl (whichever is available).
#
# Alternatively, you can download this file yourself and leave it in this
# directory to be unpacked by this script.
#
set -e
set -u


function fatal() {
  echo "${@}" >/dev/stderr
  exit 1
}


##
# 
#
function fetch() {
  if [ -z "${FETCH_UTIL:-}" ]; then
    if which wget &>/dev/null; then
      FETCH_UTIL="wget -q"
      FETCH_OPT="-O"

    elif which curl &>/dev/null; then
      FETCH_UTIL="curl -s"
      FETCH_OPT="-o"

    fi
  fi

  if [ -n "${FETCH_UTIL}" ]; then
    ${FETCH_UTIL} "${1}" ${FETCH_OPT} "${2:-/dev/stdout}"
  else
    fatal "This script requires either curl or wget be installed."
  fi
}


##
# 
#
function get_sqlite() {
  local download_url="http://sqlite.org/download.html"
  if [ -z "${SQLITE_VER:-}" ]; then
    echo "Inspecting ${download_url} to guess latest version..."
    SQLITE_VER="$( fetch ${download_url} \
      | grep -m1 'href=.sqlite-amalgamation-[0-9]*\.zip.' \
      | sed -e 's:.*href=.sqlite-amalgamation-\([0-9]*\)\.zip.*:\1:' \
    )"
  fi
  if [ -z "${SQLITE_VER:-}" ]; then
    fatal "Failed to find an amalgamation download here: ${download_url}"
  fi

  SQLITE_DIR="sqlite-amalgamation-${SQLITE_VER}"
  SQLITE_ZIP="${SQLITE_DIR}.zip"
  SQLITE_URL="http://www.sqlite.org/${SQLITE_ZIP}"

  fetch "${SQLITE_URL}" "${SQLITE_ZIP}"
}


# Download an amalgamation, if necessary.
if [ -e sqlite-amalgamation-*.zip ]; then
  SQLITE_ZIP="$( ls sqlite-amalgamation-*.zip | head -n1 )"
  SQLITE_DIR="${SQLITE_ZIP%.zip}"
else
  get_sqlite
fi

# Unpack the tarball and move the contents into this directory.
unzip "${SQLITE_ZIP}"
mv -f "${SQLITE_DIR}"/sqlite3.* .

# Clean up.
rm -rf "${SQLITE_DIR}" "${SQLITE_ZIP}"
