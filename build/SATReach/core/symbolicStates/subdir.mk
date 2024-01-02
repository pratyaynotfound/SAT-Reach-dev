################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/symbolicStates/initialState.cpp \
../core/symbolicStates/symbolicStates.cpp \
../core/symbolicStates/symbolicStatesUtility.cpp 

OBJS += \
./SATReach/core/symbolicStates/initialState.o \
./SATReach/core/symbolicStates/symbolicStates.o \
./SATReach/core/symbolicStates/symbolicStatesUtility.o 

CPP_DEPS += \
./SATReach/core/symbolicStates/initialState.d \
./SATReach/core/symbolicStates/symbolicStates.d \
./SATReach/core/symbolicStates/symbolicStatesUtility.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/core/symbolicStates/%.o: ../core/symbolicStates/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


