# ------------------------------------------------------------------------------
# Microchip AVR Makefile Template by Pieter Conradie <https://piconomix.com>
#
# Based on:
# - WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
# - WinARM template makefile by Martin Thomas, Kaiserslautern, Germany 
# - DMBS - Dean's Makefile Build System by Dean Camera [https://github.com/abcminiuser/dmbs]
# - http://stackoverflow.com Q&A research
# - Automatically generate makefile dependencies (http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html)
#
# Makefile script special requirements:
# - make 3.81 or greater (requires working eval function)
# - rm (to delete files and directories)
#
# ------------------------------------------------------------------------------
# Creation Date:  2012-07-13
# ------------------------------------------------------------------------------

# Define programs and commands
CPREFIX   = avr
CC        = $(CPREFIX)-gcc
OBJCOPY   = $(CPREFIX)-objcopy
OBJDUMP   = $(CPREFIX)-objdump
SIZE      = $(CPREFIX)-size
NM        = $(CPREFIX)-nm
DEBUGGER  = $(CPREFIX)-gdb
REMOVE    = rm -f
REMOVEDIR = rm -rf
COPY      = cp

# Define Messages
MSG_BEGIN               = '-------- begin --------'
MSG_END                 = '--------  end  --------'
MSG_SIZE                = 'Size:'
MSG_FLASH_HEX           = 'Creating HEX load file for Flash:'
MSG_FLASH_BIN           = 'Creating BIN load file for Flash:'
MSG_EEPROM              = 'Creating load file for EEPROM:'
MSG_EXTENDED_LISTING    = 'Creating Extended Listing:'
MSG_SYMBOL_TABLE        = 'Creating Symbol Table:'
MSG_LINKING             = 'Linking:'
MSG_COMPILING           = 'Compiling C:'
MSG_COMPILING_CPP       = 'Compiling C++:'
MSG_ASSEMBLING          = 'Assembling:'
MSG_CLEANING            = 'Cleaning project:'

# Define all object files from source files (C, C++ and Assembly)
#     Relative paths are stripped and OBJDIR prefix is added.
OBJECTS = $(foreach file,$(SRC) $(CPPSRC) $(ASRC), $(OBJDIR)/$(basename $(notdir $(file))).o)

# Compiler flags to generate dependency files
GENDEPFLAGS = -MMD -MP -MF $$(patsubst %.o,%.d,$$@)

# Add DEFINE macros (with -D prefixed) to compiler flags
CFLAGS   += $(addprefix -D,$(CDEFS))
CPPFLAGS += $(addprefix -D,$(CPPDEFS))
AFLAGS   += $(addprefix -D,$(ADEFS))

# Combine all necessary flags and optional flags
#     Add target processor to flags
#     By using ?= assignment operator, these variables can be overided in 
#     Makefile that includes this boilerplate file
ALL_CFLAGS   ?= -mmcu=$(MCU) $(CFLAGS) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@) -I. $(addprefix -I,$(INCDIRS)) $(GENDEPFLAGS)
ALL_CPPFLAGS ?= -mmcu=$(MCU) -x c++ $(CPPFLAGS) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@) -I. $(addprefix -I,$(INCDIRS)) $(GENDEPFLAGS)
ALL_AFLAGS   ?= -mmcu=$(MCU) -x assembler-with-cpp $(AFLAGS) -I. $(addprefix -I,$(INCDIRS)) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@),--listing-cont-lines=100,--gstabs
ALL_LDFLAGS  ?= -mmcu=$(MCU) $(LDFLAGS) -Wl,-Map=$(OBJDIR)/$(PROJECT).map $(addprefix -L,$(EXTRALIBDIRS))

# Default target
all: begin gccversion build size end

# Build targets
build: elf hex bin eep lss sym

elf: $(PROJECT).elf
hex: $(PROJECT).hex
bin: $(PROJECT).bin
eep: $(PROJECT).eep
lss: $(PROJECT).lss
sym: $(PROJECT).sym

# Default target to create specified source files.
# If this rule is executed then the input source file does not exist.
$(SRC) $(CPPSRC) $(ASRC):
	$(error "Source file does not exist: $@")

# Check that specified Object files directory is not empty or the current directory
ifeq ($(strip $(OBJDIR)),)
    $(error "OBJDIR must not be empty or a space")
else ifeq ($(strip $(OBJDIR)),.)
    $(error "OBJDIR must not be a .")
endif

# Eye candy
begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo

# Display size of file
size: $(PROJECT).elf
	@echo
	@echo $(MSG_SIZE)
	$(SIZE) --mcu=$(MCU) --format=avr $(PROJECT).elf

# Display compiler version information
gccversion : 
	@$(CC) --version

# Create final output file (.hex) from ELF output file
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH_HEX) $@
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock $< $@

# Create final output file (.bin) from ELF output file
%.bin: %.elf
	@echo
	@echo $(MSG_FLASH_BIN) $@
	$(OBJCOPY) -O binary -R .eeprom -R .fuse -R .lock $< $@

# Create EEPROM init file (.eep) from ELF output file
%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0

# Create extended listing file from ELF output file
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -z $< > $(OBJDIR)/$@

# Create a symbol table from ELF output file
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $(OBJDIR)/$@

# Link: create ELF output file from object files
.SECONDARY : $(PROJECT).elf
.PRECIOUS : $(OBJECTS)
%.elf: $(OBJECTS)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_LDFLAGS) $^ -o $@

# Compile: create object files from C source files
#   A separate rule is created for each SRC file to ensure that the correct
#   file is used to create the object file (in the OBJDIR directory).
define create_c_obj_rule
$(OBJDIR)/$(basename $(notdir $(1))).o: $(1)
	@echo
	@echo $(MSG_COMPILING) $$<
	$(CC) -c $(ALL_CFLAGS) $$< -o $$@
endef
$(foreach file,$(SRC),$(eval $(call create_c_obj_rule,$(file)))) 

# Compile: create object files from C++ source files
#   A separate rule is created for each CPPSRC file to ensure that the correct
#   file is used to create the object file (in the OBJDIR directory).
define create_cpp_obj_rule
$(OBJDIR)/$(basename $(notdir $(1))).o: $(1)
	@echo
	@echo $(MSG_COMPILING_CPP) $$<
	$(CC) -c $(ALL_CPPFLAGS) $$< -o $$@ 
endef
$(foreach file,$(CPPSRC),$(eval $(call create_cpp_obj_rule,$(file)))) 

# Assemble: create object files from assembler source files
#   A separate rule is created for each ASRC file to ensure that the correct
#   file is used to create the object file (in the OBJDIR directory).
define create_asm_obj_rule
$(OBJDIR)/$(basename $(notdir $(1))).o: $(1)
	@echo
	@echo $(MSG_ASSEMBLING) $$<
	$(CC) -c $(ALL_AFLAGS) $$< -o $$@
endef
$(foreach file,$(ASRC),$(eval $(call create_asm_obj_rule,$(file)))) 

# Target: clean project
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(PROJECT).hex
	$(REMOVE) $(PROJECT).bin
	$(REMOVE) $(PROJECT).eep
	$(REMOVE) $(PROJECT).elf
	$(REMOVEDIR) $(OBJDIR)

# Create object file directory
$(shell mkdir $(OBJDIR) 2>/dev/null)

# Include the compiler generated dependency files
-include $(OBJECTS:%.o=%.d)

# Listing of phony targets
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex bin eep lss sym clean clean_list

