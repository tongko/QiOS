#!/bin/bash
for f in $(find $SOURCEDIR -name '*.c' -or -name '*.asm'); do
    filename=$(basename "$f")
	ext="${filename##*.}"
    target="$OBJDIR"/${filename%.*}.o
	echo File name: "$filename"
	echo Extension: "$ext"
	echo File: "$target"
done