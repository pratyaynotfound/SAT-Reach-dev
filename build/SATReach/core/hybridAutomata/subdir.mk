################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/hybridAutomata/hybridAutomata.cpp \
../core/hybridAutomata/location.cpp \
../core/hybridAutomata/transition.cpp \
../core/hybridAutomata/varToIndexMap.cpp 

OBJS += \
./SATReach/core/hybridAutomata/hybridAutomata.o \
./SATReach/core/hybridAutomata/location.o \
./SATReach/core/hybridAutomata/transition.o \
./SATReach/core/hybridAutomata/varToIndexMap.o 

CPP_DEPS += \
./SATReach/core/hybridAutomata/hybridAutomata.d \
./SATReach/core/hybridAutomata/location.d \
./SATReach/core/hybridAutomata/transition.d \
./SATReach/core/hybridAutomata/varToIndexMap.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/core/hybridAutomata/%.o: ../core/hybridAutomata/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


