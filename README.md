# pico-exercises
C programming exercises for Raspberry Pi Pico using the Pico SDK.


# Clone SDK (or use submodule)
```sh
git submodule add https://github.com/raspberrypi/pico-sdk external/pico-sdk
git submodule update --init --recursive
export PICO_SDK_PATH=$PWD/external/pico-sdk
```


# Build
```sh
mkdir -p build && cd build
cmake .. -DPICO_BOARD=pico # or pico2/picow
make -j
```
