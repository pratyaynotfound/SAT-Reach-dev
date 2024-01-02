################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/continuous/approxModel/approxModel.cpp \
../core/continuous/approxModel/fbInterpol.cpp 

OBJS += \
./SATReach/core/continuous/approxModel/approxModel.o \
./SATReach/core/continuous/approxModel/fbInterpol.o 

CPP_DEPS += \
./SATReach/core/continuous/approxModel/approxModel.d \
./SATReach/core/continuous/approxModel/fbInterpol.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/core/continuous/approxModel/%.o: ../core/continuous/approxModel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


