CC = /usr/local/CodeSourcery/bin/arm-none-eabi-gcc
DIR = ./source/
DDIR  = $(DIR)drivers/
RTOS_DIR = ./FreeRTOS/
RTOS_PORT = $(RTOS_DIR)portable/GCC/ARM_CM3/
RTOS_MEM = $(RTOS_DIR)portable/MemMang/

COMMON = -mcpu=cortex-m3 -mthumb --no-builtin-memcpy --no-builtin-memset --no-builtin-fabs
CFLAGS = $(COMMON) -std=gnu99 -O2 -g -I./include -I./include/drivers -I$(RTOS_DIR)include -I$(RTOS_PORT)
LDFLAGS = $(COMMON) -T link.ld -Wl,--build-id=none


# Driver source files
SRCC =  $(DDIR)syscalls.c $(DDIR)startup.c $(DDIR)system_LPC17xx.c $(DDIR)leds.c  $(DDIR)timer.c $(DDIR)extint.c
SRCC += $(DDIR)iap.c $(DDIR)uart.c $(DDIR)pwm.c $(DDIR)gps.c $(DDIR)i2c.c $(DDIR)itg3200.c $(DDIR)bma180.c

# RTOS source files
SRCC +=	$(RTOS_DIR)list.c $(RTOS_DIR)tasks.c $(RTOS_DIR)queue.c $(RTOS_PORT)port.c $(RTOS_MEM)heap_1.c

# User program source files
SRCC +=	$(DIR)main.c $(DIR)misc.c $(DIR)fmath.c $(DIR)sensors.c  $(DIR)kalman.c $(DIR)pid.c $(DIR)input.c 
SRCC +=	$(DIR)telemetry.c

# All object files for linking
SRCO = 	syscalls.o startup.o system_LPC17xx.o list.o tasks.o queue.o port.o heap_1.o main.o extint.o iap.o
SRCO += misc.o fmath.o leds.o timer.o uart.o pwm.o gps.o sensors.o kalman.o pid.o i2c.o itg3200.o bma180.o
SRCO += input.o telemetry.o

all: clean main.elf
	
main.o: $(SRC)
	@echo "Compiling..."
	$(CC) $(CFLAGS) -c $(SRCC)
	@echo "Done!"

main.elf: main.o
	@echo "Linking..."
	$(CC) $(LDFLAGS) $(SRCO) -o main.elf
	rm -f *.o
	@echo "Done!"
	
clean:
	@echo "Cleaning..."
	rm -f *.o *.elf
