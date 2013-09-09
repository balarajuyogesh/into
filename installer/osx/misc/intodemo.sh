#!/bin/sh

CURRENT_DIR=`dirname $0`
export DYLD_LIBRARY_PATH=$CURRENT_DIR/lib

$CURRENT_DIR/intodemo-bin
