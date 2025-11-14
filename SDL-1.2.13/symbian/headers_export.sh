#!/bin/sh -e

DEST=$EPOCROOT/epoc32/include/SDL
cd ..
SRC=include
for f in `ls $SRC`; do
    echo ..\\$SRC\\$f SDL\\$f >> symbian/exports.txt
done

