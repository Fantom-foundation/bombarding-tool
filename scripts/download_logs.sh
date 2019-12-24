#!/usr/bin/env bash

N=$#

i=0
for ip in $@; do
    let "i+=1"
    path=$(ssh -i ~/.ssh/bombarder.pem ubuntu@$ip "sudo docker inspect --format='{{.LogPath}}' node" 2> /dev/null)
    ssh -i ~/.ssh/bombarder.pem ubuntu@$ip "sudo cp $path ~/node-$i.log && sudo chmod 777 ~/node-$i.log"
    scp -i ~/.ssh/bombarder.pem ubuntu@$ip:~/node-$i.log node-$i.log
done
