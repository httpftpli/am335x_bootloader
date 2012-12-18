#!/bin/bash
grep path .gitmodules | awk '{ print $3 }' > ./modulepath.tmp
 
# read
while read LINE
do
    echo $LINE
    (cd ./$LINE && git checkout master && git pull)
done < ./modulepath.tmp
rm -f ./modulepath.tmp