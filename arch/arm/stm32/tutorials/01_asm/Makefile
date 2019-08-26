# Define project name
PROJECT = flashing_led

# Targets to build
all: $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin $(PROJECT).lss

# Link: create ELF output file (*.elf) from object file
%.elf: $(PROJECT).o
	@echo
	@echo 'Linking:' $@
	arm-none-eabi-ld $^ -nostartfiles -T stm32l072xb.ld -o $@

# RULE: Create object file (*.o) from assembler source file (*.S)
%.o: %.S
	@echo
	@echo 'Assembling:' $<
	arm-none-eabi-as $< -mcpu=cortex-m0plus -mthumb -g -o $@

# RULE: Create HEX output file (*.hex) from ELF output file (*.elf)
%.hex: %.elf
	@echo
	@echo 'Creating HEX load file for Flash:' $@
	arm-none-eabi-objcopy -O ihex $< $@

# RULE: Create HEX output file (*.bin) from ELF output file (*.elf)
%.bin: %.elf
	@echo
	@echo 'Creating BIN load file for Flash:' $@
	arm-none-eabi-objcopy -O binary $< $@

# RULE: Create extended listing file (*.lss) from ELF output file (*.elf)
%.lss: %.elf
	@echo
	@echo 'Creating Extended Listing:' $@
	arm-none-eabi-objdump -h -S -z $< > $@

# TARGET: clean project
clean:
	@echo
	@echo 'Cleaning project:'
	rm -f $(PROJECT).o
	rm -f $(PROJECT).elf
	rm -f $(PROJECT).hex
	rm -f $(PROJECT).bin
	rm -f $(PROJECT).lss

