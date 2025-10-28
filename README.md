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

# Assignment PDF
The original exercise description can be found [here](./Harjoitustehtävät.pdf) 


# Exercises

You can try them directly on hardware or in the online [Wokwi](https://wokwi.com/) simulator.

1. Exercise [here](./exercises/exercise1) Try it in Wokwi. [Wokwi](https://wokwi.com/projects/445644573425380353)
1. Exercise [here](./exercises/exercise2) Try it in Wokwi. [Wokwi](https://wokwi.com/projects/445643035136983041)
1. Exercise [here](./exercises/exercise3) Try it in Wokwi. [Wokwi](https://wokwi.com/projects/445643534518201345)
1. Exercise [here](./exercises/exercise4) Try it in Wokwi. [Wokwi](https://wokwi.com/projects/444314869385369601)
1. Exercise [here](./exercises/exercise5) Try it in Wokwi. [Wokwi](https://wokwi.com/projects/446050294728616961) (use buttonkit library)

## ButtonKit Library [here](./buttonkit/README.md) 
**ButtonKit** is a lightweight library for handling push buttons on the Raspberry Pi Pico.  
It provides simple initialization and polling functions that automatically handle
debouncing and detection of short and long presses. 