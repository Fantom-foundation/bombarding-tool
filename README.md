Blomb - Blockchain Bombarding Tool
================================

[![Build Status](https://travis-ci.com/salamantos/private-blomb.svg?token=ijvcHaFVsscKJfw27NS8&branch=master)](https://travis-ci.com/salamantos/private-blomb)

[![Build Status](https://img.shields.io/badge/dynamic/json?color=brightgreen&label=TPS&query=message&url=http%3A%2F%2F157.230.33.232%2Ftps)](https://travis-ci.com/salamantos/private-blomb)
[![Build Status](https://img.shields.io/badge/dynamic/json?color=brightgreen&label=CPU%20usage&query=message&suffix=%25&url=http%3A%2F%2F157.230.33.232%2Fcpu)](https://travis-ci.com/salamantos/private-blomb)
[![Build Status](https://img.shields.io/badge/dynamic/json?color=brightgreen&label=Memory%20usage&query=message&suffix=%25&url=http%3A%2F%2F157.230.33.232%2Fmemory)](https://travis-ci.com/salamantos/private-blomb)

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
