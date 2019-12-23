#!/usr/bin/env bash

scp -i .ssh/bombarder.pem ubuntu@3.133.86.27:~/bombarding-tool/docker/node-0.log .
scp -i .ssh/bombarder.pem ubuntu@3.134.107.215:~/bombarding-tool/docker/node-1.log .
scp -i .ssh/bombarder.pem ubuntu@3.134.90.185:~/bombarding-tool/docker/node-2.log .
