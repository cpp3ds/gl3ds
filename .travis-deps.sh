#!/bin/sh
set -e
set -x

# Build and install devkitARM + ctrulib
wget http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/devkitARMupdate.pl
perl devkitARMupdate.pl

# Get latest ctrulib and overwrite bundled one
git clone https://github.com/smealum/ctrulib.git
cd ctrulib/libctru && git checkout great-refactor && make -j4 install && cd -
