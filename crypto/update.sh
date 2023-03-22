#!/usr/bin/env sh

set -e
set -x

MYDIR="$(dirname ${0})"
wget 'https://raw.githubusercontent.com/kokke/tiny-AES-c/master/aes.c' -O "${MYDIR}/aes.c"
wget 'https://raw.githubusercontent.com/kokke/tiny-AES-c/master/aes.h' -O "${MYDIR}/aes.h"
wget 'https://raw.githubusercontent.com/kokke/tiny-HMAC-c/main/src/hmac.c' -O "${MYDIR}/hmac.c"
wget 'https://raw.githubusercontent.com/kokke/tiny-HMAC-c/main/src/hmac.h' -O "${MYDIR}/hmac.h"
wget 'https://raw.githubusercontent.com/kokke/tiny-HMAC-c/main/src/sha1.c' -O "${MYDIR}/sha1.c"
wget 'https://raw.githubusercontent.com/kokke/tiny-HMAC-c/main/src/sha1.h' -O "${MYDIR}/sha1.h"
wget 'https://raw.githubusercontent.com/bonybrown/tiny-AES128-C/master/pkcs7_padding.c' -O "${MYDIR}/pkcs7_padding.c"
wget 'https://raw.githubusercontent.com/bonybrown/tiny-AES128-C/master/pkcs7_padding.h' -O "${MYDIR}/pkcs7_padding.h"
