PROJECT = test
SRC     = board.c main.c
MCU     = atmega328p
OBJECTS = $(patsubst %.c,%.o,$(SRC))

all: $(PROJECT).elf $(PROJECT).hex $(PROJECT).lss

$(PROJECT).elf: $(OBJECTS)
    avr-gcc -mmcu=$(MCU) $^ -o $@

$(PROJECT).hex: $(PROJECT).elf
    avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock $< $@

$(PROJECT).lss: $(PROJECT).elf
    avr-objdump -h -S -z $< > $@

%.o: %.c
    avr-gcc -mmcu=$(MCU) -c $< -o $@

clean:
    rm -f *.o
    rm -f *.elf
    rm -f *.hex
    rm -f *.lss
