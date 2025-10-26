# Hook zstd for setting compression level and compression threads

A shared library to hook `ZSTD_CCtx* ZSTD_createCCtx(void)` for setting compression level and compression threads.

For `qemu-img` (>= **6.0**) or the similar programs without setting zstd compression level and compression threads options.

See also: [How to change zstd level for qcow2 image?](https://stackoverflow.com/questions/72562226/how-to-change-zstd-level-for-qcow2-image)

## Building

```shell
cd zstd-hook
cmake -DCMAKE_BUILD_TYPE=Release
make
```

## Usage

Set the following environment variables:

* `LD_PRELOAD` : the hook library `libzstdhook.so`
* `LD_LIBRARY_PATH` : the directory for the hook library `libzstdhook.so`
* `ZSTD_CLEVEL` : the zstd compression level between `1` and `22`
* `ZSTD_THREADS` : the zstd compression threads
* `ZSTD_STRATEGY` : the zstd strategy, between `1` and `9`
* `ZSTD_LDM=1` : enable LongDistanceMatching

For examples, hook `qemu-img` (>= **6.0**) converting source image to zstd compressed `qcow2` image by compression level `22` and the **maximum** compression threads on Ubuntu 22.04:

```shell
cd zstd-hook
LD_LIBRARY_PATH=. LD_PRELOAD=libzstdhook.so \
  ZSTD_CLEVEL=22 ZSTD_LDM=1 ZSTD_STRATEGY=9 \
  ZSTD_THREADS=$( grep -c '^processor' /proc/cpuinfo ) \
  qemu-img convert -c -o compression_type=zstd -O qcow2 <origin-image> <compressed-qcow2>
```
