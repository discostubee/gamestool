################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
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
all: libOSX_dt.a

dependents:
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug_threading && $(MAKE) clean all

# Tool invocations
libOSX_dt.a: $(OBJS) $(USER_OBJS) /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug_threading/libgamestool_dt.a
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "libOSX_dt.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) libOSX_dt.a
	-@echo ' '

post-build:
	-cp /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug_threading/libOSX_dt.a /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/../../../includes/libOSX_dt.a
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug_threading/libgamestool_dt.a:

-include ../makefile.targets
