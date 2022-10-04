#!/usr/bin/sh

cd $(dirname $(realpath $0))/../test-data
../prototype/MdFmt.pm input.md | diff --color=always - expected.md

([ $? = 0 ] && echo Test Successful.) || echo Test Failed!
perl -v | perl -ne 'print, exit if /\S/'
