#!/usr/bin/env bash

# requires jq
# configure AWS CLI in advance: aws configure
# add key-pair in advance: aws ec2 import-key-pair --key-name bombarder --public-key-material file://path/to/bombarder.pub

source utils.sh
set -e

CFG_FILE="$1"
N_NODES="$2"

if [[ -z "$CFG_FILE" ]]; then
    die "Usage: ./bombard.sh config.sh num_nodes"
fi
if [[ -z "$N_NODES" ]]; then
    die "Usage: ./bombard.sh config.sh num_nodes"
fi

source "$CFG_FILE"


INSTANCE_IDS=$(run_instances $N_NODES $INSTANCE_TYPE)
INSTANCE_IPS=$(get_instance_ips $INSTANCE_IDS)

echo "Ids:" $INSTANCE_IDS
echo "IPs:" $INSTANCE_IPS

if [[ -z "$DEFAULT_BOMB_ID" ]]; then
    BOMB_ID=$(run_instances 1)
else
    BOMB_ID="$DEFAULT_BOMB_ID"
fi

BOMB_IP=$(get_instance_ips $BOMB_ID)

echo "Bombarder id:" $BOMB_ID
echo "Bombarder ip:" $BOMB_IP

echo "Wait $BOOT_DELAY seconds"
sleep $BOOT_DELAY
echo "Start deployment"

./deploy.sh "$CFG_FILE" $INSTANCE_IPS

ENTRY_NODE_IP=`echo "$INSTANCE_IPS" | awk '{ print($1); exit; }' | tr -d '\n'`
attach_and_exec $BOMB_IP "NODES=\"$ENTRY_NODE_IP\" ~/bombard.sh" &>storm.log &

echo "Bombarding is successfully set up"
echo "To finish bombarding call"
echo "bombard-fetch-and-stop.sh" "$CFG_FILE" $BOMB_ID $INSTANCE_IDS
