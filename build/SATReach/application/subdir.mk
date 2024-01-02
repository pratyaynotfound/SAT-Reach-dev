################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../application/readCommandLine.cpp \
../application/sfDirections.cpp \
../application/sfUtility.cpp \
../application/themeSelector.cpp \
../application/userOptions.cpp \
../application/SAT-Reach.cpp 

OBJS += \
./SATReach/application/readCommandLine.o \
./SATReach/application/sfDirections.o \
./SATReach/application/sfUtility.o \
./SATReach/application/themeSelector.o \
./SATReach/application/userOptions.o \
./SATReach/application/SAT-Reach.o 

CPP_DEPS += \
./SATReach/application/readCommandLine.d \
./SATReach/application/sfDirections.d \
./SATReach/application/sfUtility.d \
./SATReach/application/themeSelector.d \
./SATReach/application/userOptions.d \
./SATReach/application/SAT-Reach.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/application/%.o: ../application/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


