#!/bin/sh

if [ "$1" = "make" ];  then
    qmake -recursive
    make
fi

# Find subdirs in Makefile
subdirs()
{
    if [ -f Makefile ]; then
      sed -n '/SUBTARGETS/{:a; n; s/^.*sub-\([^ ]\+\).*$/\1/; p; ta; q}' Makefile
    fi
}

testall()
{
    for test in $*; do
        cd $test
        SUBDIRS=$(subdirs)
        if [ -z "$SUBDIRS" ]; then
            if [ -x $MODE/$test ]; then
                $MODE/$test -silent 
            else
                qmake MODE=$MODE
                make
                if [ -x $test ]; then
                    $MODE/$test -silent
                else
                    echo "Test executable $PWD/$MODE/$test not found."
                fi
            fi
        else
          testall $SUBDIRS
        fi
        cd ..
    done
}

MODE=debug

olddir="$PWD"
thisdir=$(dirname "$0")
export QML2_IMPORT_PATH=$(readlink -f "$thisdir/../qml")
cd "$thisdir"
testall $(subdirs)
cd "$olddir"
