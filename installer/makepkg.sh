#!/bin/bash -e

packages="libinto libinto-dev libinto-doc"
options="disabled_extensions disabled_plugins license debug_symbols"

license_dir=/exports/development/licenses
doc_dir=/exports/www/into_api

enabled_3rdparty_libs()
{
  cd 3rdparty
  local extlibs disabled_ext_re
  extlibs=$(ls -d */ | sed -e 's#/$##')
  if [ -n "$disabled_extensions" ]; then
    disabled_ext_re=`echo "$disabled_extensions" | sed -e 's/ /\\\\|/g'`
    extlibs=`echo "$extlibs" | grep -v "$disabled_ext_re"`
  fi
  echo "$extlibs" | sed -e 's#^#3rdparty/#'
  cd ..
}

enabled_plugins()
{
  cd plugins
  local disabled_plugin_re plugins
  plugins=$(ls -d */ | grep -v "gui/" | sed -e 's#/$##')
  if [ -n "$disabled_plugins" ]; then
    disabled_plugin_re=`echo "$disabled_plugins" | sed 's/[, ]/\\\\|/g'`
    plugins=`echo "$plugins" | grep -v "^\($disabled_plugin_re\)$"`
  fi
  for i in $plugins; do
    if [ -d gui/$i ]; then
      gui_plugins="$gui_plugins plugins/gui/$i"
    fi
  done
  echo "$plugins$gui_plugins" | sed -e 's#^#plugins/#'
  cd ..
}

# $1 - lib directory
# $2 - license file
copy_3rdparty_libraries()
{
  local extlibs shlibs
  extlibs=`enabled_3rdparty_libs`
  for i in $extlibs; do
    shlibs=$(find -L $i -wholename "$i/lib/*.so*")
    # There are libraries we need to include
    if [ -n "$shlibs" ]; then
      # Copy the libraries
      cp -a $shlibs $1
    fi
    # Add license information
    if [ -e $i/license ]; then
      cat <<EOF >> $2

**************************************************************************
 The source codes under $i are licensed under the following terms
**************************************************************************

EOF
      cat $i/license >> $2
    fi
  done
}

# $1 - lib directory
strip_libs()
{
  local shlibs
  # Make all the libraries convention-compliant
  shlibs=$(find $1 -type f | grep -v 'debug$')
  chmod 644 $shlibs
  strip $shlibs
}

create_libinto_structure()
{
  if [ -z "$QTDIR" ]; then
    # Try to deduce Qt directory from ldd output
    QTDIR=$(dirname $(ldd ydin/release/libpiiydin.so | awk '/libQtCore/{print $3}'))
    if [ ! -e $QTDIR/lib/libQtCore.so ]; then
      exit_fail "Cannot locate Qt libraries. Please set QTDIR correctly."
    fi
  fi
  
  mkdir -p $1/usr/lib/into
  # Copy into libraries and plug-ins
  # Ignore disabled plug-ins
  if [ -n "$disabled_plugins" ]; then
    disabled_plugin_re=`echo "plugins/$disabled_plugins" | sed -e 's# #\\\\|plugins/#g'`
    if [ -z "$debug_symbols" ]; then
      disabled_plugin_re='debug$\|'."$disabled_plugin_re"
    fi
  elif [ -z "$debug_symbols" ]; then
    disabled_plugin_re='debug$'
  fi
  #echo $disabled_plugin_re
  if [ -n "$disabled_plugin_re" ]; then
    intolibs=$(find ydin core gui plugins -wholename "*release/*.so*" | grep -v "$disabled_plugin_re")
    qtlibs=$(ls $QTDIR/lib/libQt{Core,Gui,Network,Sql,Xml,Svg,Script}.so* | grep -v 'debug$')
  else
    intolibs=$(find ydin core gui plugins -wholename "*release/*.so*")
    qtlibs=$(ls $QTDIR/lib/libQt{Core,Gui,Network,Sql,Xml,Svg,Script}.so*)
  fi
  
  cp -a $intolibs $1/usr/lib/into

  for i in script declarative; do
    mkdir -p $1/usr/lib/into/$i
    cp $i/release/libpii$i.so $1/usr/lib/into/$i
    if [ -n "$debug_symbols" ]; then
      cp $i/release/libpii$i.so.debug $1/usr/lib/into/$i
    fi
  done

  cp -a $qtlibs $1/usr/lib/into
  
  # Copy Qt plugins
  mkdir -p $1/usr/lib/into/sqldrivers
  mkdir -p $1/usr/lib/into/imageformats
  find $QTDIR/plugins/sqldrivers -name "*.so" -exec cp -a {} $1/usr/lib/into/sqldrivers \;
  find $QTDIR/plugins/imageformats -name "*.so" -exec cp -a {} $1/usr/lib/into/imageformats \;

  copy_3rdparty_libraries $1/usr/lib/into $1/usr/share/doc/$package/copyright

  strip_libs $1/usr/lib/into
}

# $1 - target dir
copy_docs()
{
  read -p "Run doxygen to create fresh documentation? [Y/n] " rundoxy
  case "$rundoxy" in
    n*|N*)
    ;;
    *)
      doxygen doc.cfg > /dev/null 2>&1
  esac

  if [ -e generated_doc/html ]; then
    find -L generated_doc/html -maxdepth 1 -type f -exec cp {} $1 \;
  fi
}

create_libinto_dev_structure()
{
  if [ -z "$license" ]; then
    echo "License must be specified. Use -o license=XXX."
    echo "License files are read from $license_dir/into-XXX.txt and"
    echo "$license_dir/into-XXX-header.txt"
    exit 1
  fi
  if [ ! -f "$license_dir/into-$license.txt" ]; then
    echo "Missing $license_dir/into-$license.txt"
    exit 1
  fi
  if [ ! -f "$license_dir/into-$license-header.txt" ]; then
    echo "Missing $license_dir/into-$license-header.txt"
    exit 1
  fi

  mkdir -p $1/into/bin/script
  mkdir -p $1/into/bin/declarative
  mkdir -p $1/into/lib
  mkdir -p $1/into/include
  mkdir -p $1/into/src/3rdparty
  mkdir -p $1/into/doc/html
  mkdir -p $1/into/demos

  enabled_plugins=`enabled_plugins`

  # Copy sources
  find core ydin gui demos tools/demo tools/jsrunner script declarative $enabled_plugins \
   -regex ".*\.\(c\|cc\|h\|pro\|pri\|ui\|qrc\|png\|svg\)" | \
   grep -v "ui_" | xargs -I@ cp --parents @ $1/into/src
  cp *.h *.pro *.pri $1/into/src
  cp plugins/*.pr? $1/into/src/plugins
  if [ -d $1/into/src/plugins/gui ]; then
    cp plugins/gui/gui.pro $1/into/src/plugins/gui
  fi
  cp 3rdparty/3rdparty.pro $1/into/src/3rdparty
  cp INSTALL $1/into
  cp installer/sh/install.sh $1/into/src
  chmod +x $1/into/src/install.sh

  # PENDING translations
  # Copy binaries
  find core ydin gui $enabled_plugins -wholename "*release/*.so*" | grep -v 'debug$' | \
   xargs -I@ cp -a @ $1/into/lib
  # Link header files
  olddir=$PWD
  cd $1/into/include
  find ../src -name windows -prune -o -name "Pii*.h" -exec ln -s {} . \;
  cd "$olddir"
  # Copy example program
  mkdir -p $1/into/doc/examples/threshold
  cp doc/examples/{Threshold.cc,threshold.pro} $1/into/doc/examples/threshold
  # Initial license text
  cp $license_dir/into-$license.txt $1/into/COPYING
  # Copy 3rd party libraries
  copy_3rdparty_libraries $1/into/lib $1/into/COPYING
  # Copy 3rd party sources
  find `enabled_3rdparty_libs` -regex ".*\(\.\(cc\|cpp\|c\|h\|hpp\|pro\|pri\)\|README\|license\)" | \
    grep -v "moc_" | xargs -I@ cp --parents @ $1/into/src
  # PENDING 3rdparty headers

  # Documentation
  #cp -ra $doc_dir/* $1/into/doc/html

  # Demo binaries
  for i in `find demos -name README`; do
    dirname=`dirname $i`
    demoname=`basename $dirname`
    destdir=$1/into/$dirname
    
    mkdir -p $destdir
    cp $i $destdir
    if [ -x $dirname/release/$demoname ]; then
      cp --parents $i $1/into
      cp $dirname/release/$demoname $1/into/$dirname
    fi
  done
  
  cp -a demos/utils/release/libdemoutils* $1/into/lib
  cp tools/demo/release/demo $1/into/demos/intodemo
  # Demo screenshots
  for i in /exports/development/intodemo/screenshots/*.png; do
    target=$1/into/demos/$(echo `basename $i` | sed 's#_#/#g')
    cp $i $target
  done
  # Demo images
  mkdir $1/into/demos/images
  cp -r /exports/development/intodemo/images/* $1/into/demos/images

  # Scripting stuff
  cp tools/jsrunner/release/jsrunner $1/into/bin
  cp script/release/libpiiscript.so $1/into/bin/script
  cp declarative/release/libpiideclarative.so $1/into/bin/declarative

  # intoenv script
  cp installer/sh/libinto-dev/intoenv $1/into/bin
  
  # Finally, install license note to source files
  find $1/into/src/{core,gui,ydin,plugins,demos,tools,declarative,script} -regex ".*\.\(h\|cc\)" \
    -exec tools/licensechanger/licensechanger $license_dir/into-$license-header.txt {} \;
}

create_libinto_doc_structure()
{
  mkdir -p $1/usr/share/doc/$package/html
  copy_docs $1/usr/share/doc/$package/html
}

. $(dirname $0)/makepackage.sh
