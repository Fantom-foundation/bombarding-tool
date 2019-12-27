#!/usr/bin/env bash

NODES="$@"


for node in $NODES; do
    /app/tx-storm --num=1/1 --rate=1000 --accs-start=1000 --accs-count=100 --metrics --verbosity 5 "http://${node}:3000" &
done

wait
