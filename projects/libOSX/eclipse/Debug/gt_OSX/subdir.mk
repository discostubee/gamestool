################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/source/gt_OSX/OSX_addon.cpp \
/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/source/gt_OSX/OSX_fileIO.cpp \
/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/source/gt_OSX/OSX_world.cpp 

OBJS += \
./gt_OSX/OSX_addon.o \
./gt_OSX/OSX_fileIO.o \
./gt_OSX/OSX_world.o 

CPP_DEPS += \
./gt_OSX/OSX_addon.d \
./gt_OSX/OSX_fileIO.d \
./gt_OSX/OSX_world.d 


# Each subdirectory must supply rules for building sources it contributes
gt_OSX/OSX_addon.o: /Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/source/gt_OSX/OSX_addon.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libgamestool/eclipse" -I/Developer/SDKs/boost_1_46_0 -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libOSX/eclipse/../../../source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gt_OSX/OSX_fileIO.o: /Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/source/gt_OSX/OSX_fileIO.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libgamestool/eclipse" -I/Developer/SDKs/boost_1_46_0 -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libOSX/eclipse/../../../source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gt_OSX/OSX_world.o: /Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/source/gt_OSX/OSX_world.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libgamestool/eclipse" -I/Developer/SDKs/boost_1_46_0 -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libOSX/eclipse/../../../source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


