#!/bin/bash
FILE=result.txt
if [ -f "$FILE" ] ; then rm $FILE
fi
grep "OK DOWNLOAD" cdlinux.ftp.log | cut -d'"' -f 2,4 | sort -u | sed "1,\$s#.*/##" | grep "\.iso" | sort >> $FILE
grep " 2[0-9][0-9] " cdlinux.www.log |cut -d ':' -f 2,5 |cut -d ' ' -f 1,7 |sort -u |sed "1, \$s#.*/##" |grep "\.iso$" |grep "^c" |sort >> $FILE
sort $FILE| uniq -c 
