#!/bin/sh

set -eu

export RACK_DIR=${GITHUB_WORKSPACE}/Rack-SDK
export RACK_USER_DIR=${GITHUB_WORKSPACE}

export CC=x86_64-w64-mingw32-gcc-posix
export CXX=x86_64-w64-mingw32-g++-posix
export STRIP=x86_64-w64-mingw32-strip

curl -L https://vcvrack.com/downloads/Rack-SDK-${RACK_SDK_VERSION}.zip -o rack-sdk.zip
unzip -o rack-sdk.zip
rm rack-sdk.zip

make dist
