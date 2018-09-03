PROJECT = test
OBJECTS = board.o main.o
MCU     = atmega328p

all: $(PROJECT).elf $(PROJECT).hex $(PROJECT).lss

$(PROJECT).elf: $(OBJECTS)
    avr-gcc -mmcu=$(MCU) $(OBJECTS) -o $(PROJECT).elf

$(PROJECT).hex: $(PROJECT).elf
    avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock $(PROJECT).elf $(PROJECT).hex

$(PROJECT).lss: $(PROJECT).elf
    avr-objdump -h -S -z $(PROJECT).elf > $(PROJECT).lss

board.o: board.c
    avr-gcc -mmcu=$(MCU) -c board.c -o board.o

main.o: main.c
    avr-gcc -mmcu=$(MCU) -c main.c -o main.o

clean:
    rm -f *.o
    rm -f *.elf
    rm -f *.hex
    rm -f *.lss
