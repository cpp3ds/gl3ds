#!/bin/sh
set -e
set -x

# Build and install devkitARM + ctrulib
wget http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/devkitARMupdate.pl
git clone https://github.com/smealum/ctrulib.git
sudo perl devkitARMupdate.pl

# Get latest ctrulib and overwrite bundled one
# cd ctrulib/libctru && make ; cd -
# sudo cp -rf ctrulib/libctru/ ${DEVKITPRO}
