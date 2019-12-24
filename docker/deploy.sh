#!/usr/bin/env bash

N=$#

attach_and_exec() {
    local IP=$1
    local CMD=$2

    for attempt in $(seq 2)
    do
        if (( attempt > 5 ));
        then
            echo "  - attempt ${attempt}: " >&2
        fi;

        res=$(ssh -i ~/.ssh/bombarder.pem ubuntu@"${IP}" "${CMD}" 2> /dev/null)
        if [ $? -eq 0 ]
        then
            echo $res
            return 0
        else
            sleep 2
        fi
    done
    echo "Failed to attach to ${IP}" >&2
    return 1
}

echo -e "Launch $N nodes:\n"

i=0
enode=""
for ip in $@; do
    let "i+=1"
    if ((i==1)); then
        echo -e "Bootnode on $ip:"
    else
        echo -e "Node $i on $ip:"
    fi

    res=$(attach_and_exec $ip 'git clone https://github.com/Fantom-foundation/bombarding-tool.git && sudo apt-get -y update && sudo apt-get -y install docker.io')
    echo -e "Repo cloned"
    res=$(attach_and_exec $ip 'sudo docker build -t node ~/bombarding-tool/docker')
    echo -e "Docker built"
    if ((i==1)); then
        res=$(attach_and_exec $ip "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node node --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=$i/$N")
    else
        res=$(attach_and_exec $ip "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node node --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=$i/$N --bootnodes=$enode")
    fi
    echo -e "Run!\n"

    if ((i==1)); then
        sleep 10
        enode=$(attach_and_exec $ip 'sudo docker exec -i node /lachesis attach --exec "admin.nodeInfo.enode" http://0.0.0.0:3000')
        enode=$(echo ${enode} | sed "s/127.0.0.1/$ip/")
        echo -e "${enode}\n"
    fi
done