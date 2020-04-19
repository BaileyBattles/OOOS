#!/bin/sh


hexdump -v drive/storage.img | head -n `expr $1 \* 32` | tail -n 32
    