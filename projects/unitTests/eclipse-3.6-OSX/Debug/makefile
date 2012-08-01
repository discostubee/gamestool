################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include gt_base/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: unitTests_d

# Tool invocations
unitTests_d: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++ -L"/Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTests/eclipse-3.6-OSX/../../../includes" -L"/Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTests/eclipse-3.6-OSX/../../shared/eclipse-3.6-OSX/lib_links/boost" -o "unitTests_d" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) unitTests_d
	-@echo ' '

post-build:
	-cp /Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTests/eclipse-3.6-OSX/Debug/unitTests_d /Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTests/eclipse-3.6-OSX/../../../bin/unitTests_d
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
