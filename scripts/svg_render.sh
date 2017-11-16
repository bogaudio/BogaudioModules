#!/bin/bash

INKSCAPE="/Applications/Inkscape.app/Contents/Resources/bin/inkscape"

if [ "x$1" = "x" -o "x$2" = "x" ]
then
  echo "Usage: $0 <in.svg> <out.svg>"
  exit 1
fi

if [ ! -f "$1" ]
then
  echo "No such file: $1"
  exit 1
fi

TMP_FILE="/tmp/svg_render_tmp.svg"

< "$1" perl -e '$s = do { local $/; <STDIN> }; $s =~ s/<use[^<>\/]+id="\w+_(PARAM|INPUT|OUTPUT|LIGHT)"[^<>\/]+(\/\>|<\/use>)//gs; print $s' > "$TMP_FILE" && \
  "$INKSCAPE" -f "$TMP_FILE" \
  --verb EditSelectAll --verb SelectionUnGroup \
  --verb EditSelectAll --verb EditUnlinkClone \
  --verb EditSelectAll  --verb ObjectToPath \
  --verb FileSave --verb FileQuit && \
  cp "$TMP_FILE" "$2" && \
  rm "$TMP_FILE"
