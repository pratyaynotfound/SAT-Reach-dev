################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../io/ioUtility.cpp \
../io/loadModel.cpp \
../io/parser.cpp \
../io/xmlParser.cpp

OBJS += \
./SATReach/io/ioUtility.o \
./SATReach/io/loadModel.o \
./SATReach/io/parser.o \
./SATReach/io/xmlParser.o

CPP_DEPS += \
./SATReach/io/ioUtility.d \
./SATReach/io/loadModel.d \
./SATReach/io/parser.d \
./SATReach/io/xmlParser.d


# Each subdirectory must supply rules for building sources it contributes
SATReach/io/%.o: ../io/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


