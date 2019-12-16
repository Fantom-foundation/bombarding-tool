#!/usr/bin/env bash

./main $(docker inspect --format='{{.LogPath}}' node0 node1 node2)
