#!/bin/bash

# To only run ctags if the tags file is at least 1 days old...
# ...collect both times in seconds-since-the-epoch...
filename=tags
one_day_ago=$(date -d 'now - 1 days' +%s)
file_time=$(date -r "$filename" +%s)

# ...and then just use integer math:
if (( file_time <= hundred_days_ago )); then
  echo "$filename is older than 1 day, generating ctags please be patient..."
  # Run ctags so we can use ctrl+] to jump for definitions and stuff
  ctags -R -f ./"$filename" .
  echo "...ctags done!"
fi
# Open all the stuff in vim - prepared nicely
vim -p makefile *.cpp *.h
