#!/usr/bin/env bash

BOMB_ID=$1
INSTANCE_IDS=${@:2}

source utils.sh
set -e

INSTANCE_IPS=$(get_instance_ips $INSTANCE_IDS)

echo "Ids:" $INSTANCE_IDS
echo "Ips:" $INSTANCE_IPS

BOMB_IP=$(get_instance_ips $BOMB_ID)

echo "Bombarder id:" $BOMB_ID
echo "Bombarder ip:" $BOMB_IP

# TODO: fetch logs here

aws ec2 stop-instances --instance-ids $INSTANCE_IDS $BOMB_ID || true
aws ec2 terminate-instances --instance-ids $INSTANCE_IDS $BOMB_ID

# TODO: run log analyzer here
