#!/usr/bin/env bash

if [[ -z "$NODES" ]]; then
    NODES="18.223.33.131"
    echo "Use default NODES: $NODES"
fi


for node in $NODES; do
    /tmp/tx-storm --num=1/1 --rate=1000 --accs-start=1000 --accs-count=100 --metrics --verbosity 5 "http://${node}:3000" &
done

wait

