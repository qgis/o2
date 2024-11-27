#!/bin/bash

set -e

export CCACHE_CPP2=yes

ccache -M 200M
ccache -s

export CC="ccache clang"
export CXX="ccache clang++"
export CFLAGS="-Werror -O2"
export CXXFLAGS="-Werror -O2"

mkdir -p build
cd build
cmake -GNinja \
    -Do2_BUILD_EXAMPLES:BOOL=ON \
    -Do2_WITH_TESTS=ON \
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
ninja

ccache -s
