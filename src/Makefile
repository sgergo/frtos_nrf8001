#******************************************************************************
#
# Makefile - Rules for building the FreeRTOS example.
#
# Copyright (c) 2012-2015 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 2.1.1.71 of the EK-TM4C123GXL Firmware Package.
#
#******************************************************************************

# Defines the part type that this project uses.
PART=TM4C123GH6PM

# Define project name here
PROJECT_NAME = frtos_nrf8001

# The base directory for TivaWare and other files
ROOT=..

# Include the common make definitions.
include ${ROOT}/makedefs


# Where to find source files that do not live in this directory.
VPATH=../third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F
VPATH+=../third_party/FreeRTOS/Source/portable/MemMang/
VPATH+=../third_party/FreeRTOS/Source
VPATH+=../third_party/BLE
VPATH+=../utils

# Where to find header files that do not live in the source directory.
IPATH=.
IPATH+=..
IPATH+=../third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F
IPATH+=../third_party/FreeRTOS
IPATH+=../third_party/FreeRTOS/Source/include
IPATH+=../third_party/BLE
IPATH+=../third_party


# Uploader tool path.
PROGRAMMER=lm4flash



# The default rule, which causes the FreeRTOS example to be built.
all: ${COMPILER}
all: ${COMPILER}/${PROJECT_NAME}.axf

# The rule to clean out all the build products
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

# The rule to load the project to the board (sudo may be needed if udev rule is not made).
install:
	@echo Loading the ${PROJECT_NAME}.bin
	${PROGRAMMER} ${COMPILER}/${PROJECT_NAME}.bin

# The rule to create the target directory.
${COMPILER}:
	@mkdir -p ${COMPILER}



# Rules for building the FreeRTOS example.
#-----------------------------------------------------------------------------------------
# Put your source files here (in alphabetic order)
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/board.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/board_ble.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/board_spi.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/board_uart.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/main.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/uartstdio.o
#-----------------------------------------------------------------------------------------
# FreeRTOS specific files
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/heap_2.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/list.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/port.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/queue.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/tasks.o
#-----------------------------------------------------------------------------------------
# BLE specific files
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/hal_aci_tl.o
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/lib_aci.o
# Startup: interrupt vectors etc.
${COMPILER}/${PROJECT_NAME}.axf: ${COMPILER}/startup_${COMPILER}.o
# Tivaware drivers
${COMPILER}/${PROJECT_NAME}.axf: ${ROOT}/driverlib/libdriver.a
# Linker script
${COMPILER}/${PROJECT_NAME}.axf: project.ld
SCATTERgcc_${PROJECT_NAME}=project.ld
ENTRY_${PROJECT_NAME}=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1

# Include the automatically generated dependency files.
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif