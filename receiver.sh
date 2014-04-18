#!/bin/sh



if [ $# -ne 1 ]
then
echo "Wrong number of argurments. Exiting...."
fi

filepath=$1

#  1 sender 
#  2 receiver 


./Transport_layer 3
r=$?
if [ $r -ne 0 ]
then
echo "Error in Transport Layer!!"
exit -1
fi


./mac_layer 2

r=$?
if [ $r -ne 0 ]
then
echo "Error in MAC Layer!!"
exit -1
fi


./network_layer 2

r=$?
if [ $r -ne 0 ]
then
echo "Error in Network Layer!!"
exit -1
fi


./Transport_layer 2
r=$?
if [ $r -ne 0 ]
then
echo "Error in Transport Layer2!!"
exit -1
fi


./application_layer 2 $filepath

r=$?
if [ $r -ne 0 ]
then
echo "Error in Application Layer!!"
exit -1
fi

