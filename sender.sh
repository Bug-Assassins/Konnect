#!/bin/sh

if [ $# -ne 2 ]
then
echo "Wrong number of argurments. Exiting...."
fi

filepath=$1
ip=$2

#  1 sender 
#  2 receiver 

./application_layer 1 $filepath

r=$?
if [ $r -ne 0 ]
then
echo "Error in Application Layer!!"
exit -1
fi

./Transport_layer 1 $ip

r=$?
if [ $r -ne 0 ]
then
echo "Error in Transport Layer!!"
exit -1
fi

./network_layer 1 $ip

r=$?
if [ $r -ne 0 ]
then
echo "Error in Network Layer!!"
exit -1
fi

./mac_layer 1 $ip

r=$?
if [ $r -ne 0 ]
then
echo "Error in MAC Layer!!"
exit -1
fi


