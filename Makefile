### Name Application for Compilation file ### 
APPLICATION = lifecycle

### Absolute path for compile RIOT ### 
RIOTBASE ?= $(CURDIR)/..

### Debug Mode for  LoRa or UART ### 
DEVELHELP ?= 1

### Enable Default modules needed for Debug  ### 
USEMODULE += saul_default
USEMODULE += printf_float

### Default Modules Used ###
FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_i2c
FEATURES_REQUIRED += periph_gpio_irq
FEATURES_REQUIRED += periph_rtc
USEMODULE += xtimer

### Oled Modules Used ###
USEPKG += u8g2
USEPKG += minmea

# Buzzer Modules Used ###
FEATURES_REQUIRED = periph_pwm

# MMA Modules Used ###
USEMODULE += mma8x5x

### LoRa Modules Used ###
LORA_REGION ?= EU868
USEPKG += semtech-loramac
USEMODULE += sx1276
USEMODULE += fmt

### External Modules Intergrated in DIRS ###
BIN_DIRS += Tests          # Unity Tests
BIN_USEMODULE += Tests                  
BIN_USEMODULE += $(APPLICATION_MODULE)  
DIST_FILES += Makefile

BIN_DIRS += Unity         # Unity Lib from ThrowTheSwitch
BIN_USEMODULE += Unity                  
BIN_USEMODULE += $(APPLICATION_MODULE)  
DIST_FILES += Makefile

### Compiler Flags ###
CFLAGS=-Wunknown-pragmas

### Base MakeFile for RIOT Compilation ###
include $(RIOTBASE)/Makefile.include
