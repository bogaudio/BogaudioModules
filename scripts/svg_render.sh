#!/bin/bash

INKSCAPE="/Applications/Inkscape.app/Contents/MacOS/inkscape"

if [ "x$1" = "x" ]
then
  echo "Usage: $0 <IN-src.svg>"
  exit 1
fi

if [ ! -f "$1" ]
then
  echo "No such file: $1"
  exit 1
fi

OUT_FILE=''
if echo "$1" | perl -e '$_ = <stdin>; exit(!/-src\.svg$/i)'
then
  OUT_FILE=`echo "$1" | perl -e '$_ = <stdin>; s/-src\.svg$/.svg/i; s/res-src\//res\//; print'`
else
  echo "Input file must be named *-src.svg: $1"
  exit 1
fi

TMP_FILE="/tmp/svg_render_tmp.svg"

< "$1" perl -e '$s = do { local $/; <STDIN> }; $s =~ s/<use[^<>\/]+id="\w+_(PARAM|INPUT|OUTPUT|LIGHT|WIDGET)"[^<>\/]+(\/\>|<\/use>)//gs; print $s' > "$TMP_FILE" && \
  "$INKSCAPE" -g --actions='EditSelectAll;SelectionUnGroup;EditSelectAll;EditUnlinkClone;EditSelectAll;ObjectToPath;FileSave;FileQuit' "$TMP_FILE" 2> /dev/null && \
  cp "$TMP_FILE" "$OUT_FILE" && \
  rm "$TMP_FILE"
