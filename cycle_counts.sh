#!/bin/bash

# $1 is the mnrl file
# $2 is the input file

./print_cycles $1 $2 | grep "^cycle:" | cut -d ':' -f 2 | uniq -c | awk '{ print $2 ", " $1}' | tail -n +2
