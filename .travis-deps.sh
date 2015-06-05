#!/bin/sh
set -e
set -x

export DEVKITPRO=/home/travis/devkitPro
export DEVKITARM=${DEVKITPRO}/devkitARM
export CTRULIB=${DEVKITPRO}/libctru

# Build and install devkitARM + ctrulib
wget http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/devkitARMupdate.pl
git clone https://github.com/smealum/ctrulib.git
sudo perl devkitARMupdate.pl
cd ctrulib/libctru && make ; cd -
sudo cp -rf ctrulib/libctru/ ${DEVKITPRO}
