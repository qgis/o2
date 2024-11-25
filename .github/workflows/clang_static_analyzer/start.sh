#!/bin/bash

set -e

sudo apt update

DEBIAN_FRONTEND=noninteractive sudo apt-get install -y --no-install-recommends \
    libtool \
    g++ \
    make \
    jq \
    clang-tools \
    qtbase5-dev \
    qt5keychain-dev \
    qtscript5-dev \
    qtwebengine5-dev

NPROC=$(nproc)
echo "NPROC=${NPROC}"
export MAKEFLAGS="-j ${NPROC}"

mkdir csa_build
cd csa_build

scan-build -o scanbuildoutput -plist -v cmake -Do2_BUILD_EXAMPLES:BOOL=ON \
    -Do2_SHOW_TRACE:BOOL=ON \
    -Do2_WITH_DROPBOX:BOOL=ON \
    -Do2_WITH_FACEBOOK:BOOL=ON \
    -Do2_WITH_FLICKR:BOOL=ON \
    -Do2_WITH_GOOGLE:BOOL=ON \
    -Do2_WITH_HUBIC:BOOL=ON \
    -Do2_WITH_KEYCHAIN:BOOL=ON \
    -Do2_WITH_MSGRAPH:BOOL=ON \
    -Do2_WITH_OAUTH1:BOOL=ON \
    -Do2_WITH_QT5:BOOL=ON \
    -Do2_WITH_SKYDRIVE:BOOL=ON \
    -Do2_WITH_SMUGMUG:BOOL=ON \
    -Do2_WITH_SPOTIFY:BOOL=ON \
    -Do2_WITH_SURVEYMONKEY:BOOL=ON \
    -Do2_WITH_TWITTER:BOOL=ON \
    -Do2_WITH_UBER:BOOL=ON \
    -Do2_WITH_VIMEO:BOOL=ON \
    ..

scan-build -o scanbuildoutput -sarif -v -enable-checker alpha.unix.cstring.OutOfBounds,alpha.unix.cstring.BufferOverlap,optin.cplusplus.VirtualCall,optin.cplusplus.UninitializedObject make

rm -f filtered_scanbuild.txt
files=$(find scanbuildoutput -name "*.sarif")
for f in $files; do
    jq '.runs[].results[] | (if .locations[].physicalLocation.fileLocation.uri | (index("_generated_parser") ) then empty else { "uri": .locations[].physicalLocation.fileLocation.uri, "msg": .message.text, "location": .codeFlows[-1].threadFlows[-1].locations[-1] } end)' < $f > tmp.txt
    if [ -s tmp.txt ]; then
        echo "Errors from $f: "
        cat $f
        echo ""
        cat tmp.txt >> filtered_scanbuild.txt
    fi
    rm -f tmp.txt
done
if [ -s filtered_scanbuild.txt ]; then
    echo ""
    echo ""
    echo "========================"
    echo "Summary of errors found:"
    cat filtered_scanbuild.txt
    /bin/false
fi
