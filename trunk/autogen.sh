#!/bin/bash

aclocal &&
autoconf &&
automake --add-missing --copy &&
./configure $@ &&
make
