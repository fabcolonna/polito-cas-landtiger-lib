#!/bin/bash

LIB_DIR=Lib
INC_DIR=Lib/Include

mkdir -p $LIB_DIR
cp Keil/Objects/*.lib $LIB_DIR || { echo "No .lib files found"; exit 1; }

# Moving .h files to include folder
mkdir -p $INC_DIR

cp Keil/Source/Libs/*.h $INC_DIR
for dir in Keil/Source/Libs/*; do
    [[ -d $dir ]] && cp $dir/*.h $INC_DIR
done

# Cleanup
rm -rf Keil/{Objects,Listings,RTE}

# Creating a tarball
tar -czf peripherals-lib.tar.gz $LIB_DIR
rm -rf $LIB_DIR