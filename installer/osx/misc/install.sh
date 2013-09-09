#!/bin/sh -e

# A script for installing locally compiled Into binaries over the
# prebuilt ones.

MODE=${1:-debug}

# Copy IntoDemo (save Info.plist)
cp ../demos/IntoDemo.app/Contents/Info.plist ../demos/IntoDemo.app/Contents/Info.plist.temp
cp -rf tools/demo/$MODE/demo.app/* ../demos/IntoDemo.app
mv ../demos/IntoDemo.app/Contents/MacOS/demo ../demos/IntoDemo.app/Contents/MacOS/intodemo-bin
mv ../demos/IntoDemo.app/Contents/Info.plist.temp ../demos/IntoDemo.app/Contents/Info.plist

# Copy demos (save Info.plist)
for i in `find demos -wholename "*$MODE/*.app"`; do
  dest=../`echo $i | sed -e "s#$MODE/##"`
  cp $dest/Contents/Info.plist $dest/Contents/Info.plist.temp
  cp -rf $i/* $dest
  mv $dest/Contents/Info.plist.temp $dest/Contents/Info.plist
done

cd ../lib
find ../src -wholename "*$MODE/*.dylib" \! -regex ".*piideclarative.*\|.*piiscript.*" -exec ln -fs {} . \;

cd ../bin
# Copy scripting stuff
ln -fs ../src/tools/jsrunner/$MODE/jsrunner.app/jsrunner .
ln -fs ../../src/script/$MODE/piiscript.dylib script
ln -fs ../../src/declarative/$MODE/piideclarative.dylib declarative

cd ../src
