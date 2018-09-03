# Specify project options
PROJECT = test
SRC     = board.c main.c
MCU     = atmega328p

# Calculate object files by replacing ".c" suffix with ".o"
OBJECTS = $(patsubst %.c,%.o,$(SRC))

# "all" is the default (first) target to create ELF, HEX and LSS file
all: $(PROJECT).elf $(PROJECT).hex $(PROJECT).lss

# Rule to create ELF file
$(PROJECT).elf: $(OBJECTS)
    avr-gcc -mmcu=$(MCU) $^ -o $@

# Rule to create HEX file using ELF file
$(PROJECT).hex: $(PROJECT).elf
    avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock $< $@

# Rule to create LSS file using ELF file
$(PROJECT).lss: $(PROJECT).elf
    avr-objdump -h -S -z $< > $@

# Pattern rule to create *.o file from a *.c file
%.o: %.c
    avr-gcc -mmcu=$(MCU) -c $< -o $@

# "clean" target to delete all created files
clean:
    rm -f *.o
    rm -f *.elf
    rm -f *.hex
    rm -f *.lss

# Specify dependencies
board.o: board.c board.h
main.o: main.c board.h
