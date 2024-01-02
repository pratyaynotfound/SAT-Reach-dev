################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#-include ../../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SATReach/utilities/memUtilities/subdir.mk
-include SATReach/utilities/cpuUtilities/subdir.mk
-include SATReach/utilities/subdir.mk
-include SATReach/io/resetParser/subdir.mk
-include SATReach/io/linExpParser/subdir.mk
-include SATReach/io/flowParser/subdir.mk
-include SATReach/io/subdir.mk
-include SATReach/counterExample/subdir.mk
-include SATReach/core/symbolicStates/subdir.mk
-include SATReach/core/reachability/subdir.mk
-include SATReach/core/math/point/subdir.mk
-include SATReach/core/math/numeric/subdir.mk
-include SATReach/core/math/lpSolver/subdir.mk
-include SATReach/core/math/glpkLpSolver/subdir.mk
-include SATReach/core/math/subdir.mk
-include SATReach/core/pwl/subdir.mk
-include SATReach/core/hybridAutomata/subdir.mk
-include SATReach/core/discrete/discreteSet/subdir.mk
-include SATReach/core/continuous/polytope/subdir.mk
-include SATReach/core/continuous/convexSet/subdir.mk
-include SATReach/core/continuous/approxModel/subdir.mk
-include SATReach/benchmarks/waterlevelController/subdir.mk
-include SATReach/benchmarks/vehiclePlatoon/subdir.mk
-include SATReach/benchmarks/ttEthernet/subdir.mk
-include SATReach/benchmarks/oscillatorModel/subdir.mk
-include SATReach/benchmarks/constantDynamics/subdir.mk
-include SATReach/benchmarks/navigation/subdir.mk
-include SATReach/benchmarks/subdir.mk
-include SATReach/application/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SAT-Reach

# Tool invocations
SAT-Reach: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++ -std=c++11 -L/usr/local/lib/ -o "SAT-Reach" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) SAT-Reach
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
