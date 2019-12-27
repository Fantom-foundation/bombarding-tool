#!/usr/bin/env bash

CFG_FILE="$1"
IPS=${@:2}
N=$#
let "N-=1"

source "$CFG_FILE"

i=0
for ip in $IPS; do
    let "i+=1"
    path=$(ssh -i "$PRIVATE_KEY_PATH" ubuntu@$ip "sudo docker inspect --format='{{.LogPath}}' node")
    ssh -i "$PRIVATE_KEY_PATH" ubuntu@$ip "sudo cp $path ~/node-$i.log && sudo chmod ugo+r ~/node-$i.log"
    scp -i "$PRIVATE_KEY_PATH" ubuntu@$ip:~/node-$i.log node-$i.log
done
