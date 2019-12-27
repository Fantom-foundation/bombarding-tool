#!/usr/bin/env bash

CFG_FILE=$1
BOMB_ID=$2
INSTANCE_IDS=${@:3}

source utils.sh
set -e

if [[ -z "$CFG_FILE" ]]; then
    die "Usage: ./bombard-fetch-and-stop.sh config.sh bombarder_node_id node_ids..."
fi
if [[ -z "$BOMB_ID" ]]; then
    die "Usage: ./bombard-fetch-and-stop.sh config.sh bombarder_node_id node_ids..."
fi
source "$CFG_FILE"

INSTANCE_IPS=$(get_instance_ips $INSTANCE_IDS)

echo "Ids:" $INSTANCE_IDS
echo "IPs:" $INSTANCE_IPS

BOMB_IP=$(get_instance_ips $BOMB_ID)

echo "Bombarder id:" $BOMB_ID
echo "Bombarder ip:" $BOMB_IP

set +e

# stop tx-storm process
attach_and_exec $BOMB_IP "sudo docker kill bomb"

./download_logs.sh "$CFG_FILE" $INSTANCE_IPS

if [[ -n "$DEFAULT_BOMB_ID" ]]; then
    BOMB_ID=""
fi

aws ec2 stop-instances --instance-ids $INSTANCE_IDS $BOMB_ID --output json
aws ec2 terminate-instances --instance-ids $INSTANCE_IDS $BOMB_ID --output json

# TODO: run log analyzer here
