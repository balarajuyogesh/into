#!/bin/sh -e

# A script for installing locally compiled Into binaries over the
# prebuilt ones.

MODE=${1:-debug}

# Must be copied
cp tools/demo/$MODE/demo ../demos/intodemo
for i in `find demos -type f -executable -wholename "*$MODE/*" | grep -v demoutil`; do
  path=$(echo $i | sed -e "s#/$MODE/.*##")
  ln -fs $(echo $path | sed -e "s#[^/]\+#..#g")/src/$i ../$path || true
done

cd ../lib
find ../src -wholename "*$MODE/*.so*" \! -regex ".*piideclarative.*\|.*piiscript.*" -exec ln -fs {} . \;
cd ..

cd bin
# Link scripting stuff
ln -fs ../src/tools/jsrunner/$MODE/jsrunner .
ln -fs ../../src/script/$MODE/libpiiscript.so script
ln -fs ../../src/declarative/$MODE/libpiideclarative.so declarative
cd ../src
