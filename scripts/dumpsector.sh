#!/bin/sh

VAL=`expr $1 + 1`
hexdump -v drive/storage.img | head -n `expr $VAL \* 32` | tail -n 32
    