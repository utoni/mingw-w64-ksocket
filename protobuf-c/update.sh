#!/usr/bin/env sh

set -e
set -x

MYDIR="$(dirname ${0})"
wget 'https://raw.githubusercontent.com/protobuf-c/protobuf-c/master/protobuf-c/protobuf-c.c' -O "${MYDIR}/protobuf-c.c"
wget 'https://raw.githubusercontent.com/protobuf-c/protobuf-c/master/protobuf-c/protobuf-c.h' -O "${MYDIR}/protobuf-c.h"
