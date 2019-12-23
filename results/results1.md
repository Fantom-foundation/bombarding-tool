## Bombarding testnet of 3 nodes

### Launch testnet
  * Follow instructtion for each of 3 machines: https://github.com/Fantom-foundation/bombarding-tool/tree/master/docker
  * Machines are `t2.micro` instances of AWS

### Launch bombard node
  * Compile tx-storm with `start_storm.sh` script
  * Run `bombard.sh` to run it. Set proper ip (ips by spaces to run several tx-storms)
  * On each node execute `sudo cp $(sudo docker inspect --format='{{.LogPath}}' node) node-i.log` where i is an id of node
  * Run `fetch_logs.sh` on bombard machine to fetch logs from nodes
  * If compiled analyser is `main`, then run `./main -j node-i.log` to analyse node's logs
  * Same `t2.micro` instances of AWS

### Achieved tps on 3 nodes
Number of bombarders | node0 | node1 | node2
--- | --- | --- | ---
1 | 49.18 | 48.97 | 49.24
3 | 15.32 | 15.90 | 15.50
