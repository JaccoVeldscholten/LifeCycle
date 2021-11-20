@ECHO OFF

echo (Commando to exectute:  %1)

if "%1"=="flash" (STM32_Programmer_CLI -c port=SWD -w bin\nucleo-l452re\lifecycle.bin 0x08000000 --start 0x08000000)

if "%1"=="build" (
    rmdir /s /q bin  
    docker cp . riot_toolchain:/RIOT/projects
    docker exec -i riot_toolchain bash -c "cd /RIOT/projects/ && make BOARD=nucleo-l452re"
    docker cp riot_toolchain:/RIOT/projects/bin .
)

if "%1"=="buildflash" (
    rmdir /s /q bin     
    docker cp . riot_toolchain:/RIOT/projects
    docker exec -i riot_toolchain bash -c "cd /RIOT/projects/ && make BOARD=nucleo-l452re"
    docker cp riot_toolchain:/RIOT/projects/bin .
    STM32_Programmer_CLI -c port=SWD mode=UR -w bin\nucleo-l452re\lifecycle.bin 0x08000000 --start 0x08000000
    REM   Old Version: Just for saving: 
    REM   STM32_Programmer_CLI -c port=SWD -w bin\nucleo-l452re\lifecycle.bin 0x08000000 --start 0x08000000
)

if "%1"=="clean" (
    rmdir /s /q bin
    docker exec -i riot_toolchain bash -c "rm -rf /RIOT/projects/"
    echo Target Cleaned      
)

if "%1"=="rundocker" (
    docker pull jjveldscholten/arm_riot_toolchain:latest 				
    docker run --name riot_toolchain -it jjveldscholten/arm_riot_toolchain:latest
)

if "%1"=="" (
    echo No Args given Error!
)

