fix readme below:
## STM32 Bluepill Template Project

Template project for STM32 bluepill Development
using CubeMx, CMake and C++17.
* The main program is located in [main.cpp](Project/main.cpp).
* Peripheral configurations are found in [main.hpp](Project/main.hpp).
* You can modify [ioc file](bluepill.ioc) using CubeMx as needed.

### Prerequisites
1. GNU ARM toolchain
    Follow the installation instructions on on
    https://askubuntu.com/questions/1243252/how-to-install-arm-none-eabi-gdb-on-ubuntu-20-04-lts-focal-fossa

2. CMake
    ```bash
    sudo apt install cmake
    ```
3. st-link
    ```bash
    git clone https://github.com/stlink-org/stlink
    cd stlink
    mkdir build
    cmake -B build
    sudo cmake --build build --target install
    ```
4. [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
   (optional)


### Setup project
    ``` bash
    git clone --recurse-submodules https://github.com/aufam/bluepill 
    ```

### Build project
```bash
mkdir build
cmake -B build
cmake --build build
```

### Flash (ST-Link)
```bash
cmake --build build --target flash
```