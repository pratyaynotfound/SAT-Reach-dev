################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../io/resetParser/lex.reset.cpp \
../io/resetParser/reset.tab.cpp 

OBJS += \
./SATReach/io/resetParser/lex.reset.o \
./SATReach/io/resetParser/reset.tab.o 

CPP_DEPS += \
./SATReach/io/resetParser/lex.reset.d \
./SATReach/io/resetParser/reset.tab.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/io/resetParser/%.o: ../io/resetParser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


