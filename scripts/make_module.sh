#!/bin/bash

set -e

MODULE=$1
HP=$2

if [ "x$MODULE" == "x" ] || [ "x$HP" == "x" ]
then
  echo "Usage: $0 <module name> <width in HP>" 1>&2
  exit 1
fi

./scripts/svg_make_stub.rb --module --hp=$HP $MODULE
./scripts/svg_preprocess.rb ./res-src/$MODULE-src.svg
./scripts/svg_render.rb ./res-pp/$MODULE-pp.svg
./scripts/make_stub.sh $MODULE $HP
