################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
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
all: example_d

dependents:
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug && $(MAKE) all
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgraphics/eclipse-3.6/Debug && $(MAKE) all
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/addonOSXGL/eclipse-3.6/Debug && $(MAKE) all
	-cd /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug && $(MAKE) all

# Tool invocations
example_d: $(OBJS) $(USER_OBJS) /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug/libgamestool_d.a /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgraphics/eclipse-3.6/Debug/libgraphics_d.a /Users/stuandlou/Documents/workspace/gamestool_hg/projects/addonOSXGL/eclipse-3.6/Debug/addonX11GL_d.dylib /Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug/libOSX_d.a
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++ -L"/Users/stuandlou/Documents/workspace/gamestool_hg/projects/exampleApp/eclipse-3.6/../../../includes" -o "example_d" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) example_d
	-@echo ' '

post-build:
	-cp /Users/stuandlou/Documents/workspace/gamestool_hg/projects/exampleApp/eclipse-3.6/Debug/example_d /Users/stuandlou/Documents/workspace/gamestool_hg/projects/exampleApp/eclipse-3.6/../../../bin/example_d
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgamestool/eclipse-3.6/Debug/libgamestool_d.a:
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/libgraphics/eclipse-3.6/Debug/libgraphics_d.a:
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/addonOSXGL/eclipse-3.6/Debug/addonX11GL_d.dylib:
/Users/stuandlou/Documents/workspace/gamestool_hg/projects/libOSX/eclipse-3.6/Debug/libOSX_d.a:

-include ../makefile.targets
