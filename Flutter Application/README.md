<img src="https://i.imgur.com/ngv3hMQ.png" width="20%" height="20%" >

# LifeCycle

Lifecycle is a device that helps detect a fall from a bicycle. As soon as a fall has been registered, a report is made via KPN's LoRa network to the Cloud. From here, a set person can receive a notification through the app.

The product is developed on the STM32 Artchitecture with [RIOT OS](https://github.com/RIOT-OS/RIOT).

## Installation

#### Windows (Tested on Windows 10)
* [Visual Code](https://code.visualstudio.com/) as IDE 
* [Docker Desktop](https://www.docker.com/products/docker-desktop) for running the RIOT ToolChain
* [Termite Serial](https://www.compuphase.com/software_termite.htm) for UART (Serial) Usage (Tested)
* [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) for flashing the STM32
* (Optional) [Quick Task](https://marketplace.visualstudio.com/items?itemName=lkytal.quicktask) Quick Tasks for VsCode might come handy
#### Ubuntu (Tested on Ubuntu 20)
* [Visual Code](https://code.visualstudio.com/docs/setup/linux) as IDE 
* [Docker Engine for Ubuntu](https://docs.docker.com/engine/install/ubuntu/) for running the RIOT ToolChain
* [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) for flashing the STM32
Fixing SU issues with docker: [here](https://github.com/sindresorhus/guides/blob/main/docker-without-sudo.md)

## Docker
Use the following commands to setup the Docker [ToolChain for RIOT](https://hub.docker.com/r/jjveldscholten/arm_riot_toolchain) for running the compling

```bash
docker pull jjveldscholten/arm_riot_toolchain:latest                            # Pull the container to OS
docker run --name riot_toolchain -itd jjveldscholten/arm_riot_toolchain:latest  # Run the container
```

**Install Steps**
1. Install the above mentioned items and run them. 
2. Set enviroment variable for STM32_Programmer_CLI
3. Reboot PC
4. Test if programmer works by CMD/Terminal by running: `STM32_Programmer_CLI`


## Usage / compiling
1. Run in VSCode the Task options for compiling / Building/ Flashing
Manual:
```bash
    docker cp . riot_toolchain:/RIOT/projects
    docker exec -i riot_toolchain bash -c "cd /RIOT/projects/ && make BOARD=nucleo-l452re"
    docker cp riot_toolchain:/RIOT/projects/bin .
    STM32_Programmer_CLI -c port=SWD mode=UR -w bin\nucleo-l452re\lifecycle.bin 0x08000000 --start 0x08000000
```


## Documentation
Use the DoxyGen Folder for documentation.
For Writing Documentation use the following link: [Doxygen](https://www.doxygen.nl/manual/docblocks.html) 



## License
This repo belongs to Team LifeCycle. And there are no rights for using this repo.
