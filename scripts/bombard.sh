#!/usr/bin/env bash

# requires jq
# configure AWS CLI in advance: aws configure
# add key-pair in advance: aws ec2 import-key-pair --key-name bombarder --public-key-material file://path/to/bombarder.pub

source utils.sh
set -e

INSTANCE_IDS=$(run_instances $1)
INSTANCE_IPS=$(get_instance_ips $INSTANCE_IDS)
INSTANCE_IPS=`echo $INSTANCE_IPS` # to one line

echo "Ids:" $INSTANCE_IDS
echo "Ips:" $INSTANCE_IPS

# we cannot atomatically deploy bombarder yet
# BOMB_ID=$(run_instances 1)
# so hardcode
BOMB_ID="i-04a0dc2d116f32723"

BOMB_IP=$(get_instance_ips $BOMB_ID)

echo "Bombarder id:" $BOMB_ID
echo "Bombarder ip:" $BOMB_IP

echo "Wait 40 seconds"
sleep 40
echo "Start deployment"

./deploy.sh $INSTANCE_IPS
attach_and_exec $BOMB_IP "NODES=\"$INSTANCE_IPS\" ~/bombard.sh" &>storm.log &

echo "Bombarding is successfully set up"
echo "To finish bombarding call"
echo "bombard-fetch-and-stop.sh" $BOMB_ID $INSTANCE_IDS
