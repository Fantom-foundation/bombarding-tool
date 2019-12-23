#!/usr/bin/env bash

nodes=(
    3.133.86.27
    3.134.107.215
    3.134.90.185
)

N=3

for ((i=0;i<$N;i+=1))
do
    path=$(ssh -i ~/.ssh/bombarder.pem ubuntu@${nodes[$i]} sudo cp $(sudo docker inspect --format='{{.LogPath}}' node) node-${i}.log 2> /dev/null)
    scp -i ~/.ssh/bombarder.pem ubuntu@${nodes[$i]}:~/bombarding-tool/docker/node-${i}.log .
done

# scp -i .ssh/bombarder.pem ubuntu@3.133.86.27:~/bombarding-tool/docker/node-0.log .
# scp -i .ssh/bombarder.pem ubuntu@3.134.107.215:~/bombarding-tool/docker/node-1.log .
# scp -i .ssh/bombarder.pem ubuntu@3.134.90.185:~/bombarding-tool/docker/node-2.log .
