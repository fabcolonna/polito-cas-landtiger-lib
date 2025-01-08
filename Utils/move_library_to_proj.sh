#!/bin/bash

[[ $# -ne 1 ]] && {
    echo "Usage: $0 <base-proj_dir>"
    exit 1
}

PROJ_DIR=$1
THIS_DIR=$(dirname $(pwd))

cd $PROJ_DIR/Keil/Libs || {
    echo "Project directory (or Keil/Libs) not found"
    exit 1
}

rm -rf Peripherals/ || true
mv $THIS_DIR/*.tar.gz .
tar -xzf *.tar.gz && mv Lib Peripherals
rm *.tar.gz

echo "Uploaded library to project: $PROJ_DIR/Keil/Libs/Peripherals"






