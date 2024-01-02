################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/math/2dGeometry.cpp \
../core/math/analyticOdeSol.cpp \
../core/math/matrix.cpp \
../core/math/matrixExponential.cpp \
../core/math/r8lib.cpp

OBJS += \
./SATReach/core/math/2dGeometry.o \
./SATReach/core/math/analyticOdeSol.o \
./SATReach/core/math/matrix.o \
./SATReach/core/math/matrixExponential.o \
./SATReach/core/math/r8lib.o

CPP_DEPS += \
./SATReach/core/math/2dGeometry.d \
./SATReach/core/math/analyticOdeSol.d \
./SATReach/core/math/matrix.d \
./SATReach/core/math/matrixExponential.d \
./SATReach/core/math/r8lib.d


# Each subdirectory must supply rules for building sources it contributes
SATReach/core/math/%.o: ../core/math/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


