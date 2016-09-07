#!/bin/sh
make bin
st-flash erase
st-flash write *.bin 0x8000000
