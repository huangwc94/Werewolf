#!/bin/bash

cd $HOME/wof
git pull
cd $HOME/wof/server
echo "#define DRIVER_TYPE_HARDWARE" > v0.2/hardware.h
pio run -e megaatmega2560 -t upload
echo "#define DRIVER_TYPE_COMPUTER" > v0.2/hardware.h