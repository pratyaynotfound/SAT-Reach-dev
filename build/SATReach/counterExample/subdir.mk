################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../counterExample/abstractCE.cpp \
../counterExample/abstractSymbolicState.cpp \
../counterExample/concreteCE.cpp \
../counterExample/nlpFunctions.cpp \
../counterExample/simulation.cpp \
../counterExample/bmc.cpp \
../counterExample/composition.cpp \
../counterExample/SATbmc.cpp 

OBJS += \
./SATReach/counterExample/abstractCE.o \
./SATReach/counterExample/abstractSymbolicState.o \
./SATReach/counterExample/concreteCE.o \
./SATReach/counterExample/nlpFunctions.o \
./SATReach/counterExample/simulation.o \
./SATReach/counterExample/bmc.o \
./SATReach/counterExample/composition.o  \
./SATReach/counterExample/SATbmc.o 

CPP_DEPS += \
./SATReach/counterExample/abstractCE.d \
./SATReach/counterExample/abstractSymbolicState.d \
./SATReach/counterExample/concreteCE.d \
./SATReach/counterExample/nlpFunctions.d \
./SATReach/counterExample/simulation.d \
./SATReach/counterExample/bmc.d \
./SATReach/counterExample/composition.d \
./SATReach/counterExample/SATbmc.d


# Each subdirectory must supply rules for building sources it contributes
SATReach/counterExample/%.o: ../counterExample/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include/ -I/usr/local/include/UnitTest++/ -I"../" -O3 -g -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


