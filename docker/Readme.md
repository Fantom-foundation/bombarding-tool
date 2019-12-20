## To run lachesis follow:

```
sudo docker build -t node .
sudo docker run -p 5050:5050 -p 3000:3000 --rm --name=node node --nousb --port 5050 --rpc --rpcaddr "0.0.0.0" --rpcport 3000 --rpccorsdomain="*" --rpcapi "eth,debug,admin,web3" --verbosity=5 --metrics --fakenet=i/N --bootnodes="enode"
```
Where
  * `i` is a number on your node of `N` nodes in a network, `i=1..N`
  * `"enode"` is url of a bootnode with pubkey, e.g. `"enode://f9cdc94e49c7bebb716a24bf1929e4b6943686b985c585fdcc4ea21cf10a55ee4f0a88b94c86d02e853ae26552782e663cd56872a68d150b25f40bb47e479625@3.133.86.27:5050"`

To get `"enode"` param: run on the bootnode
```
sudo docker exec -i node /lachesis attach --exec "admin.nodeInfo.enode" http://0.0.0.0:3000
```
This returns, e.g.
```
enode://f9cdc94e49c7bebb716a24bf1929e4b6943686b985c585fdcc4ea21cf10a55ee4f0a88b94c86d02e853ae26552782e663cd56872a68d150b25f40bb47e479625@127.0.0.1:5050
```
Replace `127.0.0.1` with its ip
