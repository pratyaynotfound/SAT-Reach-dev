################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../utilities/gradient.cpp \
../utilities/postAssignment.cpp \
../utilities/statistics.cpp \
../utilities/stringParser.cpp \
../utilities/templatePolyhedra.cpp \
../utilities/vectorOperations.cpp 

OBJS += \
./SATReach/utilities/gradient.o \
./SATReach/utilities/postAssignment.o \
./SATReach/utilities/statistics.o \
./SATReach/utilities/stringParser.o \
./SATReach/utilities/templatePolyhedra.o \
./SATReach/utilities/vectorOperations.o 

CPP_DEPS += \
./SATReach/utilities/gradient.d \
./SATReach/utilities/postAssignment.d \
./SATReach/utilities/statistics.d \
./SATReach/utilities/stringParser.d \
./SATReach/utilities/templatePolyhedra.d \
./SATReach/utilities/vectorOperations.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/utilities/%.o: ../utilities/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


