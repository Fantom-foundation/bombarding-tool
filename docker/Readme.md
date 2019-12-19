## To run lachesis follow:

```
sudo docker build -t node .
sudo docker run --name=node node --fakenet=i/N --bootnodes="enode"
```
Where
  * `i` is a number on your node of `N` nodes in a network, `i=1..N`
  * `enode` is url of a bootnode with pubpey, e.g. `--bootnodes="enode://f9cdc94e49c7bebb716a24bf1929e4b6943686b985c585fdcc4ea21cf10a55ee4f0a88b94c86d02e853ae26552782e663cd56872a68d150b25f40bb47e479625@3.133.86.27:5050"`

Run in a bootnode to get its url
```
sudo docker exec -i node /lachesis attach --exec "admin.nodeInfo.enode" http://0.0.0.0:3000
```
This returns, e.g.
```
enode://f9cdc94e49c7bebb716a24bf1929e4b6943686b985c585fdcc4ea21cf10a55ee4f0a88b94c86d02e853ae26552782e663cd56872a68d150b25f40bb47e479625@127.0.0.1:5050
```
Replace `127.0.0.1` with its ip