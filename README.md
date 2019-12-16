This is a (quick-and-dirty) log analyzer for [go-lachesis](https://github.com/Fantom-foundation/go-lachesis).

# Building

`cmake . && make`

# Usage

The general usage is `./main [-f <datetime>] [-c <ratio>] <JSON log file>...`

 * `-f` is for specifying the earliest point from which to consider events; for example, `2019-12-15T11:41:51.432300132Z`.

 * `-c` is the cutoff ratio for TPS calcuation; it is the minimum fraction of all the events that should be included to the TPS. Defaults to `0.33`.

* `<JSON log file>...` means one or more Docker log files in JSON format. To find out their locations, run, for example,

  ```
  docker inspect --format='{{.LogPath}}' node0 node1 node2
  ```

# Example output (single-machine setup)

```
Total: 5631 events
TPS: 149.8324 (from 1858 events)
```

# TODO

Time-to-finality calculation.
