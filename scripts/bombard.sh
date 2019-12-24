#!/usr/bin/env bash

# requires jq
# configure AWS CLI in advance: aws configure
# add key-pair in advance: aws ec2 import-key-pair --key-name bombarder --public-key-material file://path/to/bombarder.pub

source utils.sh
set -e

INSTANCE_IDS=$(run_instances $1)
INSTANCE_IPS=$(get_instance_ips $INSTANCE_IDS)

echo "Ids:" $INSTANCE_IDS
echo "Ips:" $INSTANCE_IPS

# TODO: add code to setup testnet on $INSTANCE_IPS here

BOMB_ID=$(run_instances 1)
BOMB_IP=$(get_instance_ips $BOMB_ID)

echo "Bombarder id:" $BOMB_ID
echo "Bombarder ip:" $BOMB_IP

# TODO: add code to setup bombarder on $BOMB_IP here

echo "Bombarding is successfully set up"
echo "To finish bombarding call"
echo "bombard-fetch-and-stop.sh" $BOMB_ID $INSTANCE_IDS
