#!/bin/bash

#htslib
SOURCE_HTSLIB="https://github.com/samtools/htslib/releases/download/1.7/htslib-1.7.tar.bz2"

get_distro () {
  EXT=""
  if [[ $2 == *.tar.bz2* ]] ; then
    EXT="tar.bz2"
  elif [[ $2 == *.zip* ]] ; then
    EXT="zip"
  elif [[ $2 == *.tar.gz* ]] ; then
    EXT="tar.gz"
  else
    echo "I don't understand the file type for $1"
    exit 1
  fi
  if hash curl 2>/dev/null; then
    curl -sS -o $1.$EXT -L $2
  else
    wget -nv -O $1.$EXT $2
  fi
}

get_file () {
# output, source
  if hash curl 2>/dev/null; then
    curl -sS -o $1 -L $2
  else
    wget -nv -O $1 $2
  fi
}

if [ "$#" -ne "1" ] ; then
  echo "Please provide an installation path  such as /opt/ICGC"
  exit 0
fi

CPU=1
ls /proc/cpuinfo >& /dev/null # very noddy attempt to figure out thread count
if [ $? -eq 0 ]; then
  CPU=`grep -c ^processor /proc/cpuinfo`
else
  CPU=`sysctl -a | grep machdep.cpu | grep thread_count | awk '{print $2}'`
  if [ $? -ne 0 ]; then
    # fall back to unthreaded core test
    CPU=`sysctl -a | grep machdep.cpu | grep core_count | awk '{print $2}'`
  fi
fi
if [ "$CPU" -gt "6" ]; then
  CPU=6
fi
echo "Max compilation CPUs set to $CPU"

set -e

INST_PATH=$1

# get current directory
INIT_DIR=`pwd`

# cleanup inst_path
mkdir -p $INST_PATH
cd $INST_PATH
INST_PATH=`pwd`
mkdir -p $INST_PATH/bin
cd $INIT_DIR

export PATH="$INST_PATH/bin:$PATH"

#create a location to build dependencies
SETUP_DIR=$INIT_DIR/install_tmp
mkdir -p $SETUP_DIR

echo -n "Get htslib ..."
if [ -e $SETUP_DIR/htslibGet.success ]; then
  echo " already staged ...";
else
  echo
  cd $SETUP_DIR
  get_distro "htslib" $SOURCE_HTSLIB
  touch $SETUP_DIR/htslibGet.success
fi

echo -n "Building htslib ..."
if [ -e $SETUP_DIR/htslib.success ]; then
  echo " previously installed ...";
else
  echo
  cd $SETUP_DIR
  mkdir -p htslib
  tar --strip-components 1 -C htslib -jxf htslib.tar.bz2
  cd htslib
  ./configure --enable-plugins --enable-libcurl --prefix=$INST_PATH
  make -j$CPU
  make install
  rm -f $INST_PATH/lib/libhts.so*
  cd $SETUP_DIR
  touch $SETUP_DIR/htslib.success
fi

export HTSLIB="$SETUP_DIR/htslib"

cd $INIT_DIR

echo -n "Building sequence-utils-c ..."
if [ -e $SETUP_DIR/seq-utils-c.success ]; then
  echo " previously installed ...";
else
  echo
  cd $INIT_DIR
  make clean
  env HTSLIB=$SETUP_DIR/htslib make -j$CPU prefix=$INST_PATH
  cp bin/context_counter $INST_PATH/bin/.
  touch $SETUP_DIR/seq-utils-c.success
  make clean
fi
