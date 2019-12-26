#!/usr/bin/env bash

CFG_FILE="$1"
IPS=${@:2}
N=$#
let "N-=1"



source utils.sh
source "$CFG_FILE"

echo -e "Launch $N nodes:\n"

i=0
enode=""
for ip in $IPS; do
    let "i+=1"
    if ((i==1)); then
        echo -e "Bootnode on $ip:"
    else
        echo -e "Node $i on $ip:"
    fi

    res=$(attach_and_exec $ip 'sudo apt-get -y update && sudo apt-get -y install docker.io')
    echo -e "Repo cloned"
    if ((i==1)); then
        res=$(attach_and_exec $ip "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node $DOCKER_IMAGE --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=$i/$N")
    else
        echo "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node $DOCKER_IMAGE --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=$i/$N --bootnodes=$enode"
        res=$(attach_and_exec $ip "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node $DOCKER_IMAGE --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=$i/$N --bootnodes=$enode")
    fi
    echo "$res"
    echo -e "Run!\n"

    if ((i==1)); then
        sleep 10
        enode=$(attach_and_exec $ip 'sudo docker exec -i node /lachesis attach --exec "admin.nodeInfo.enode" http://0.0.0.0:3000')
        enode=$(echo ${enode} | sed "s/127.0.0.1/$ip/")
        echo -e "${enode}\n"
    fi
done
