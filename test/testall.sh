#!/bin/sh

if [ "$1" = "make" ];  then
  qmake -recursive
  make
fi

# Find subdirs in Makefile
SUBDIRS=$(sed -ne '/SUBTARGETS/{:a; n; s/^.*sub-\([^ ]\+\).*$/\1/; p; ta; q}' Makefile)
MODE=debug

for test in $SUBDIRS; do
  cd $test
  if [ -x $MODE/$test ]; then
    $MODE/$test -silent 
  else
    #echo "Test executable $test not found."
    qmake 
    make
    if [ -x $test ]; then
       $MODE/$test -silent
    else
      echo "Test executable $test not found."
    fi
  fi
  cd ..
done

