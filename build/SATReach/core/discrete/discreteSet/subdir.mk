################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/discrete/discreteSet/discreteSet.cpp 

OBJS += \
./SATReach/core/discrete/discreteSet/discreteSet.o 

CPP_DEPS += \
./SATReach/core/discrete/discreteSet/discreteSet.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/core/discrete/discreteSet/%.o: ../core/discrete/discreteSet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


