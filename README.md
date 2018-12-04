# RedisBigNum
A simple Redis module for setting and incrementing big numbers. Native Redis `INCRBY` and `HINCRBY` commands are limited to signed 64-bit integers.


# Usage

## Loading the module
Load the module through redis either through:

`redis-server --loadmodule /path/to/bignumber.so`

or inside redis.conf `loadmodule /path/to/bignumber.so`

or at runtime `MODULE LOAD /path/to/bignumber.so`

## Using the module
```$ redis-cli
127.0.0.1:6379> BIGNUM someKey SET 1
OK
127.0.0.1:6379> BIGNUM someKey INCRBY 10000000000000000000000000000000000000000000000000
OK
127.0.0.1:6379> GET someKey
"10000000000000000000000000000000000000000000000001"
127.0.0.1:6379> BIGNUM someKey INCRBY -10000000000000000000000000000000000000000000000000
OK
127.0.0.1:6379> GET someKey
"1"
127.0.0.1:6379> BIGNUM someKey INCRBY -1
OK
127.0.0.1:6379> GET someKey
"0"
```

# Building

RedisBigNum only requires one external dependency, `libgmp`.


On Mac: `brew install gmp`

On Debian Linux: `sudo apt-get install libgmp3-dev`

Then simply run: `$ make`

This will output `bignumber.so` which can then be loaded into redis.
