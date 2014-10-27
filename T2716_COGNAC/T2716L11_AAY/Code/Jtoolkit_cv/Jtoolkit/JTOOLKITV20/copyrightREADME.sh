#!/bin/sh
#
# shell to put copyright information in release/java/demo/*/README files 
# This shell requires that copyrightREADME.txt exist on the current directory
#
# Usage:
# to add copyright information to a single file
# copyrightREADME.sh directory filename
# copyrightREADME.sh awt README
#
# to add copyright information to all README files in a directory tree
# copyrightREADME.sh directory "*/README"
# copyrightREADME.sh release/java/demo "*/README"
#
# Note:
# to change the copyright information, modify copyrightREADME.txt
#
echo "$1"
for i in `find $1 -name "$2"`
do   
       cat $i copyrightREADME.txt > tmp
       cp -p tmp $i
       chmod 444 $i
done
