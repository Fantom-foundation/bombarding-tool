#!/usr/bin/env bash

nodes=(
    3.133.86.27
    3.134.107.215
    3.134.90.185
)

N=3

attach_and_exec() {
    local IP=$1
    local CMD=$2

    for attempt in $(seq 20)
    do
        if (( attempt > 5 ));
        then
            echo "  - attempt ${attempt}: " >&2
        fi;

        res=$(ssh -i ~/.ssh/bombarder.pem ubuntu@IP CMD 2> /dev/null)
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
res=$(attach_and_exec ${nodes[0]} 'git clone https://github.com/Fantom-foundation/bombarding-tool.git && cd bombarding-tool')
res=$(attach_and_exec ${nodes[0]} 'sudo docker build -t node .')
res=$(attach_and_exec ${nodes[0]} "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node node --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=1/${N}")

enode=$(attach_and_exec ${nodes[0]} 'sudo docker exec -i node /lachesis attach --exec "admin.nodeInfo.enode" http://0.0.0.0:3000')
enode=$(echo ${enode} | sed "s/127.0.0.1/${nodes[0]}/")

for ((i=1;i<$N;i+=1))
do
    res=$(attach_and_exec ${nodes[$i]} 'git clone https://github.com/Fantom-foundation/bombarding-tool.git && cd bombarding-tool')
    res=$(attach_and_exec ${nodes[$i]} 'sudo docker build -t node .')
    res=$(attach_and_exec ${nodes[$i]} "sudo docker run -d -p 5050:5050 -p 3000:3000 --rm --name=node node --nousb --port 5050 --rpc --rpcaddr 0.0.0.0 --rpcport 3000 --rpccorsdomain='*' --rpcapi 'eth,debug,admin,web3' --verbosity=5 --metrics --fakenet=${i}/${N} --bootnodes=${enode}")
done

