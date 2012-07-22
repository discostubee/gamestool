################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include gt_base/subdir.mk
-include gt_OSX/subdir.mk
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
all: unitTestOSX

dependents:
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug && $(MAKE) clean all
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug && $(MAKE) clean all

# Tool invocations
unitTestOSX: $(OBJS) $(USER_OBJS) /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug/libOSX_d.a /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug/liblibgamestool_d.a
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++ -L"/Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTestOSX/eclipse-3.6/../../../includes" -o "unitTestOSX" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) unitTestOSX
	-@echo ' '

post-build:
	-cp /Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTestOSX/eclipse-3.6/Debug/unitTestOSX /Users/stuandlou/Documents/workspace/gamestool_hg/projects/unitTestOSX/eclipse-3.6/../../../bin/unitTestOSX
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug/libOSX_d.a:
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug/liblibgamestool_d.a:

-include ../makefile.targets
