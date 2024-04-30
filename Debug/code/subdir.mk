################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/PID.c \
../code/charge.c \
../code/init.c \
../code/speed.c \
../code/test.c \
../code/xcorr.c 

COMPILED_SRCS += \
./code/PID.src \
./code/charge.src \
./code/init.src \
./code/speed.src \
./code/test.src \
./code/xcorr.src 

C_DEPS += \
./code/PID.d \
./code/charge.d \
./code/init.d \
./code/speed.d \
./code/test.d \
./code/xcorr.d 

OBJS += \
./code/PID.o \
./code/charge.o \
./code/init.o \
./code/speed.o \
./code/test.o \
./code/xcorr.o 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/my file/workspace/E04_pwm_demo/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/code\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-code

clean-code:
	-$(RM) ./code/PID.d ./code/PID.o ./code/PID.src ./code/charge.d ./code/charge.o ./code/charge.src ./code/init.d ./code/init.o ./code/init.src ./code/speed.d ./code/speed.o ./code/speed.src ./code/test.d ./code/test.o ./code/test.src ./code/xcorr.d ./code/xcorr.o ./code/xcorr.src

.PHONY: clean-code

