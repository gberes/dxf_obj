#!/bin/bash

# TODO: Only run ctags when there if some file is untouched maybe...
# Run ctags so we can use ctrl+] to jump for definitions and stuff
ctags -R -f ./tags .
# Open all the stuff in vim - prepared nicely
vim -p makefile *.cpp *.h
