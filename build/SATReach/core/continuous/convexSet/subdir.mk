################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/continuous/convexSet/createPolytopeFromConvexSet.cpp \
../core/continuous/convexSet/transMinkPoly.cpp 

OBJS += \
./SATReach/core/continuous/convexSet/createPolytopeFromConvexSet.o \
./SATReach/core/continuous/convexSet/transMinkPoly.o 

CPP_DEPS += \
./SATReach/core/continuous/convexSet/createPolytopeFromConvexSet.d \
./SATReach/core/continuous/convexSet/transMinkPoly.d 


# Each subdirectory must supply rules for building sources it contributes
SATReach/core/continuous/convexSet/%.o: ../core/continuous/convexSet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


