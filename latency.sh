#!/usr/bin/sh

URL=$1
echo "1000 requests with one core"
echo $(time python thor.py -r 1000 $URL | grep TOTAL)
echo "1000 requests with two cores"
echo $(time python thor.py -r 1000 -p 2 $URL | grep TOTAL)
echo "1000 requests with four cores"
echo $(time python thor.py -r 1000 -p 4 $URL | grep TOTAL)

