#Simple C project skeleton

![travis-build-status](https://travis-ci.org/bmoar/c_skelly.svg?branch=master)

Features
========

Unit tests from a modified minunit

Zed Shaw's debug macros

bstrlib - better string library

Commands
=========

## build release
make all

## clean up generated stuff (compiled binaries, unit test log, etc)
make clean

##Run unit tests only
make test

##enable debug() statements (see debug.h)
make dev  

##check for evil functions
make check

Dir structure
==============

- bin/    = executable programs in project
- build/  = libraries built by project
- src/    = source code for libraries
- test/   = unit test code and log
