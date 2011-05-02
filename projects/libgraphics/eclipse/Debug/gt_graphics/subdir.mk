################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../gt_graphics/bitmap.cpp \
../gt_graphics/layer.cpp \
../gt_graphics/polygonMesh.cpp \
../gt_graphics/windowFrame.cpp 

OBJS += \
./gt_graphics/bitmap.o \
./gt_graphics/layer.o \
./gt_graphics/polygonMesh.o \
./gt_graphics/windowFrame.o 

CPP_DEPS += \
./gt_graphics/bitmap.d \
./gt_graphics/layer.d \
./gt_graphics/polygonMesh.d \
./gt_graphics/windowFrame.d 


# Each subdirectory must supply rules for building sources it contributes
gt_graphics/%.o: ../gt_graphics/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I"/Users/stuandlou/Documents/workspace/gamestool/branch_sherlock/projects/libgraphics/eclipse/../../../source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


