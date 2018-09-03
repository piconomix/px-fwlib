all: test.elf test.hex test.lss

test.elf: board.o main.o
    avr-gcc -mmcu=atmega328p board.o main.o -o test.elf

test.hex: test.elf
    avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock test.elf test.hex

test.lss: test.elf
    avr-objdump -h -S -z test.elf > test.lss

board.o: board.c
    avr-gcc -mmcu=atmega328p -c board.c -o board.o

main.o: main.c
    avr-gcc -mmcu=atmega328p -c main.c -o main.o

clean:
    rm -f *.o
    rm -f *.elf
    rm -f *.hex
    rm -f *.lss
