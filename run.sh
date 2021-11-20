# Current bug: File has to be run as Super User. (SUDO) This needs to be fixed 
# So it can't be used in VsCode. Use Shell Terminal instead.
# If issues exist with uploading try the following:
# Hold Reset button. Start this file. 
# Release Reset button.
# Start again this file

PATH="$PATH:/usr/local/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"                            # Set Path for STM32Cube Env

rm -r -f bin                                                                                            # Remove bin if exist
docker cp . riot_toolchain:/RIOT/projects                                                               # Copy project folder to RIOT Docker
docker exec -i riot_toolchain bash -c "cd /RIOT/projects/ && make BOARD=nucleo-l452re"                  # Compile on Docker
docker cp riot_toolchain:/RIOT/projects/bin .                                                           # Copy output back to Client

STM32_Programmer_CLI -c port=SWD mode=UR -w bin/nucleo-l452re/lifecycle.bin 0x08000000 --start 0x08000000       # Flash With UR
#STM32_Programmer_CLI -c port=SWD -w bin/nucleo-l452re/lifecycle.bin 0x08000000 --start 0x08000000              # Flash Without UR