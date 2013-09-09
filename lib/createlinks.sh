#!/bin/sh

mode=${1:-debug}
find .. -wholename "*$mode/*.so*" -exec ln -fs {} \;
