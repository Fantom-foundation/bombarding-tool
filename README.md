Blomb - Blockchain Bombarding Tool
================================

### How to build

```sh
git submodule init
git submodule update
mkdir build && cd build

cmake ..
cmake --build .
```

### How to use
See [config example](example_config.yml) to bomb your favorite blockchain.

Run `./blomb -h` to find out command line arguments.

Example:
```sh
./blomb -c ../example_config.yml -n 3 -x 9096 -t 30
```
