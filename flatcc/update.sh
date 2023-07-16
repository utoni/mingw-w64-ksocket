#!/usr/bin/env sh

set -e
set -x

git subtree pull --squash --prefix=flatcc https://github.com/dvidelabs/flatcc.git master
