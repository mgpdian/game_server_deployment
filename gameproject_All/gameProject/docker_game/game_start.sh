#!/bin/bash

./game

#MY_ROOM_NO="room_no"$MY_ROOM_NO

while [ a == a ]
do
	sleep 10
	ps aux | grep -w game | grep -v grep
	if [ 0 -ne $? ]
	then
		#del redis a piece of record
		redis-cli -h 192.168.81.128 del $MY_ROOM_NO
		exit 0
	fi
done
