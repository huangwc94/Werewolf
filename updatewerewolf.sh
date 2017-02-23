#!/bin/bash

cd $HOME/wof
git pull
cd $HOME/wof/server

pio run -e megaatmega2560 -t upload