# ------------------------------------------------------------------------------
# STM32 Makefile Template by Pieter Conradie <https://piconomix.com>
#
# Based on:
# - WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
# - WinARM template makefile by Martin Thomas, Kaiserslautern, Germany
# - DMBS - Dean's Makefile Build System by Dean Camera [https://github.com/abcminiuser/dmbs]
# - http://stackoverflow.com Q&A research
# - Automatically generate makefile dependencies (http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html)
# - GNU make manual (https://www.gnu.org/software/make/manual/make.html)
#
# Makefile script special requirements:
# - make 3.81 or greater (requires working eval function)
# - sh (sh.exe on MS-Windows platform)
# - rm (to delete files and directories)
# - mkdir (to create directories)
#
# ------------------------------------------------------------------------------
# Creation Date:  2018-03-17
# ------------------------------------------------------------------------------

# Define programs and commands
CPREFIX  ?= arm-none-eabi
CC        = $(CPREFIX)-gcc
CXX       = $(CPREFIX)-g++
OBJCOPY   = $(CPREFIX)-objcopy
OBJDUMP   = $(CPREFIX)-objdump
SIZE      = $(CPREFIX)-size
NM        = $(CPREFIX)-nm
DEBUGGER  = $(CPREFIX)-gdb
AR        = $(CPREFIX)-ar
RM        = rm -f
RMDIR     = rm -rf
MKDIR     = mkdir -p

# Determine build (debug, release, etc.)
# Set build directory and add build specific flags
ifdef build
    BUILD = $(build)
else
    BUILD = debug
endif
ifeq ($(BUILD), debug)
    BUILD_DIR ?= BUILD_DEBUG
    CDEFS     += $(CDEFS_DEBUG)
    CFLAGS    += $(CFLAGS_DEBUG)
    CXXFLAGS  += $(CXXFLAGS_DEBUG)
    AFLAGS    += $(AFLAGS_DEBUG)
    LDFLAGS   += $(LDFLAGS_DEBUG)
else ifeq ($(BUILD), debug-boot)
    BUILD_DIR ?= BUILD_DEBUG_BOOT
    CDEFS     += $(CDEFS_DEBUG)
    CFLAGS    += $(CFLAGS_DEBUG)
    CXXFLAGS  += $(CXXFLAGS_DEBUG)
    AFLAGS    += $(AFLAGS_DEBUG)
    LDFLAGS   += $(LDFLAGS_DEBUG)
else ifeq ($(BUILD), release)
    BUILD_DIR ?= BUILD_RELEASE
    CDEFS     += $(CDEFS_RELEASE)
    CFLAGS    += $(CFLAGS_RELEASE)
    CXXFLAGS  += $(CXXFLAGS_RELEASE)
    AFLAGS    += $(AFLAGS_RELEASE)
    LDFLAGS   += $(LDFLAGS_RELEASE)
else ifeq ($(BUILD), release-boot)
    BUILD_DIR ?= BUILD_RELEASE_BOOT
    CDEFS     += $(CDEFS_RELEASE)
    CFLAGS    += $(CFLAGS_RELEASE)
    CXXFLAGS  += $(CXXFLAGS_RELEASE)
    AFLAGS    += $(AFLAGS_RELEASE)
    LDFLAGS   += $(LDFLAGS_RELEASE)
else
    $(error "Unsupported build specified. Use 'make build=debug', 'make build=debug-boot', 'make build=release' or 'make build=release-boot'")
endif

# Set output directory
OUT_DIR ?= $(BUILD_DIR)
#OUT_DIR ?= .

# Specify Flash, SRAM, Stack and heap size to linker script
LDFLAGS += -Wl,--defsym,FLASH_SIZE=$(FLASH_SIZE),--defsym,SRAM_SIZE=$(SRAM_SIZE),--defsym,STACK_SIZE=$(STACK_SIZE),--defsym,HEAP_SIZE=$(HEAP_SIZE)

# Allocate space for bootloader in boot build (if specified)
#
# VTOR register (Vector Table Offset Register) must be set to correct value in:
# libs/STM32Cube/L0/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c
#
# FLASH_OFFSET must be specified in linker script.
ifeq ($(BUILD), debug-boot)
    ifndef BOOTLOADER_SIZE
        $(error "BOOTLOADER_SIZE not defined")
    endif
    CDEFS   += VECT_TAB_OFFSET=$(BOOTLOADER_SIZE)
    LDFLAGS += -Wl,--defsym,FLASH_OFFSET=$(BOOTLOADER_SIZE)
else ifeq ($(BUILD), release-boot)
    ifndef BOOTLOADER_SIZE
        $(error "BOOTLOADER_SIZE not defined")
    endif
    CDEFS   += VECT_TAB_OFFSET=$(BOOTLOADER_SIZE)
    LDFLAGS += -Wl,--defsym,FLASH_OFFSET=$(BOOTLOADER_SIZE)
endif

# Define Messages
MSG_BEGIN               = '------------ begin (build=$(BUILD)) ---------------'
MSG_END                 = '-------------- end (build=$(BUILD)) ---------------'
MSG_SIZE                = 'Size:'
MSG_FLASH_HEX           = 'Creating HEX load file for Flash:'
MSG_FLASH_BIN           = 'Creating BIN load file for Flash:'
MSG_FLASH_UF2           = 'Creating UF2 load file for Flash:'
MSG_PROG_UF2            = 'Programming UF2 load file:'
MSG_EEPROM              = 'Creating load file for EEPROM:'
MSG_EXTENDED_LISTING    = 'Creating Extended Listing:'
MSG_SYMBOL_TABLE        = 'Creating Symbol Table:'
MSG_LINKING             = 'Linking:'
MSG_ARCHIVING           = 'Creating static library:'
MSG_COMPILING           = 'Compiling C:'
MSG_COMPILING_CXX       = 'Compiling C++:'
MSG_ASSEMBLING          = 'Assembling:'
MSG_CLEANING            = 'Cleaning project:'
MSG_OPENOCD_GDB_SERVER  = 'Launching OpenOCD GDB server:'
MSG_GDB                 = 'Launching GDB:'

# Define all object files from source files (C, C++ and Assembly)
#     Relative paths are stripped and BUILD_DIR prefix is added.
OBJECTS = $(foreach file,$(ASRC) $(SRC) $(CXXSRC), $(BUILD_DIR)/$(basename $(notdir $(file))).o)

# Compiler flags to generate dependency files
DEP_FLAGS = -MMD -MP -MF $$(patsubst %.o,%.d,$$@)

# Add DEFINE macros (with -D prefixed) to compiler flags
CFLAGS   += $(addprefix -D,$(CDEFS))
CXXFLAGS += $(addprefix -D,$(CXXDEFS))
AFLAGS   += $(addprefix -D,$(ADEFS))

# Combine all necessary flags and optional flags
#     Add target processor to flags
#     By using ?= assignment operator, these variables can be overided in
#     Makefile that includes this boilerplate file
ALL_CFLAGS   ?= $(MCU) $(CFLAGS) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@) -I. $(addprefix -I,$(INCDIRS)) $(DEP_FLAGS)
ALL_CXXFLAGS ?= $(MCU) $(CXXFLAGS) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@) -I. $(addprefix -I,$(INCDIRS)) $(DEP_FLAGS)
ALL_AFLAGS   ?= $(MCU) -x assembler-with-cpp $(AFLAGS) -I. $(addprefix -I,$(INCDIRS)) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@),--listing-cont-lines=100,--gstabs
ALL_LDFLAGS  ?= $(MCU) $(LDFLAGS) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map $(addprefix -L,$(EXTRALIBDIRS))

# Default target
all: begin gccversion build size end

# Build targets
ifeq ($(BUILD), debug-boot)
build: elf hex bin lss sym uf2
else ifeq ($(BUILD), release-boot)
build: elf hex bin lss sym uf2
else
build: elf hex bin lss sym
endif

elf: $(OUT_DIR)/$(PROJECT).elf
hex: $(OUT_DIR)/$(PROJECT).hex
bin: $(OUT_DIR)/$(PROJECT).bin
uf2: $(OUT_DIR)/$(PROJECT).uf2
lss: $(BUILD_DIR)/$(PROJECT).lss
sym: $(BUILD_DIR)/$(PROJECT).sym
lib: $(OUT_DIR)/$(PROJECT).a

# Default target to create specified source files.
# If this rule is executed then the input source file does not exist.
$(SRC) $(CXXSRC) $(ASRC):
	$(error "Source file does not exist: $@")

# Eye candy
begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo

# Display size of file
size: $(OUT_DIR)/$(PROJECT).elf
	@echo
	@echo $(MSG_SIZE)
	$(SIZE) $<

# Display compiler version information
gccversion :
	@$(CC) --version

# Create final output file (*.hex) from ELF output file
$(OUT_DIR)/%.hex: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_FLASH_HEX) $@
	$(OBJCOPY) -O ihex $< $@

# Create final output file (*.bin) from ELF output file
$(OUT_DIR)/%.bin: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_FLASH_BIN) $@
	$(OBJCOPY) -O binary $< $@

# Create UF2 bootloader format file (*.uf2) from BIN output file
$(OUT_DIR)/%.uf2: $(OUT_DIR)/%.bin
	@echo
	@echo $(MSG_FLASH_UF2) $@
	python $(PX_FWLIB)/tools/uf2conv.py $< -c -b $(BOOTLOADER_SIZE) -f 0xe892273c -o $@

# Write UF2 bootloader format file (*.uf2) to target
prog_uf2: $(OUT_DIR)/$(PROJECT).bin
	@echo
	@echo $(MSG_PROG_UF2) $@
	python $(PX_FWLIB)/tools/uf2conv.py $< -b $(BOOTLOADER_SIZE) -f 0xe892273c -d $(drive)

# Create extended listing file (*.lss) from ELF output file
$(BUILD_DIR)/%.lss: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) --visualize-jumps -h -S -z $< > $@

# Create a symbol table (*.sym) from ELF output file
$(BUILD_DIR)/%.sym: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n -S $< > $@


ifeq ($(CXXSRC),)
# Link: create ELF output file (*.elf) from C object files
.SECONDARY : $(OUT_DIR)/$(PROJECT).elf
.PRECIOUS : $(OBJECTS)
$(OUT_DIR)/%.elf: $(OBJECTS)
	@echo
	@echo $(MSG_LINKING) $@
	@$(MKDIR) $(@D)
	$(CC) $(ALL_LDFLAGS) $^ $(addprefix -l,$(EXTRALIBS)) -o $@
else
# Link: create ELF output file (*.elf) from C++ and C object files
.SECONDARY : $(OUT_DIR)/$(PROJECT).elf
.PRECIOUS : $(OBJECTS)
$(OUT_DIR)/%.elf: $(OBJECTS)
	@echo
	@echo $(MSG_LINKING) $@
	@$(MKDIR) $(@D)
	$(CXX) $(ALL_LDFLAGS) $^ $(addprefix -l,$(EXTRALIBS)) -o $@
endif

# Archive: create static library (*.a) from object files
$(OUT_DIR)/%.a: $(OBJECTS)
	@echo
	@echo $(MSG_ARCHIVING) $@
	@$(MKDIR) $(@D)
	$(AR) rcs $@ $^

# Compile: create object files from C source files
#   A separate rule is created for each SRC file to ensure that the correct
#   file is used to create the object file (in the BUILD_DIR directory).
define create_c_obj_rule
$(BUILD_DIR)/$(basename $(notdir $(1))).o: $(1)
	@echo
	@echo $(MSG_COMPILING) $$<
	@$(MKDIR) $$(@D)
	$(CC) -c $(ALL_CFLAGS) $$< -o $$@
endef
$(foreach file,$(SRC),$(eval $(call create_c_obj_rule,$(file))))

# Compile: create object files from C++ source files
#   A separate rule is created for each CXXSRC file to ensure that the correct
#   file is used to create the object file (in the BUILD_DIR directory).
define create_cxx_obj_rule
$(BUILD_DIR)/$(basename $(notdir $(1))).o: $(1)
	@echo
	@echo $(MSG_COMPILING_CXX) $$<
	@$(MKDIR) $$(@D)
	$(CXX) -c $(ALL_CXXFLAGS) $$< -o $$@
endef
$(foreach file,$(CXXSRC),$(eval $(call create_cxx_obj_rule,$(file))))

# Assemble: create object files from assembler source files
#   A separate rule is created for each ASRC file to ensure that the correct
#   file is used to create the object file (in the BUILD_DIR directory).
define create_asm_obj_rule
$(BUILD_DIR)/$(basename $(notdir $(1))).o: $(1)
	@echo
	@echo $(MSG_ASSEMBLING) $$<
	@$(MKDIR) $$(@D)
	$(CC) -c $(ALL_AFLAGS) $$< -o $$@
endef
$(foreach file,$(ASRC),$(eval $(call create_asm_obj_rule,$(file))))

# Target: clean project
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(RM) $(OUT_DIR)/$(PROJECT).elf
	$(RM) $(OUT_DIR)/$(PROJECT).hex
	$(RM) $(OUT_DIR)/$(PROJECT).bin
	$(RM) $(OUT_DIR)/$(PROJECT).uf2
	$(RMDIR) $(BUILD_DIR)
	$(RM) $(OPENOCD_SCRIPT)
	$(RM) $(GDB_SCRIPT)

# Target: mostly clean project (remove all temporary building files, but leave final executables)
mostlyclean: begin mostlyclean_list end

mostlyclean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(RM) $(BUILD_DIR)/*.o
	$(RM) $(BUILD_DIR)/*.d
	$(RM) $(BUILD_DIR)/*.lst
	$(RM) $(BUILD_DIR)/$(PROJECT).lss
	$(RM) $(BUILD_DIR)/$(PROJECT).map
	$(RM) $(BUILD_DIR)/$(PROJECT).sym

# Program target using command line utility
program:  $(OUT_DIR)/$(PROJECT).hex
	STM32_Programmer_CLI -c port=SWD freq=4000 reset=HWrst -w $< -v -g 0x00000000

# Reset target using command line utility
reset:
	STM32_Programmer_CLI -c port=SWD freq=4000 reset=HWrst -rst

# Generate OpenOCD config file
.PHONY : $(OPENOCD_SCRIPT)
$(OPENOCD_SCRIPT):
	$(RM) $(OPENOCD_SCRIPT)
	@echo 'gdb_port $(GDB_PORT)' >> $(OPENOCD_SCRIPT)
	@echo 'source [find $(OPENOCD_INTERFACE)]' >> $(OPENOCD_SCRIPT)
	@echo 'transport select hla_swd' >> $(OPENOCD_SCRIPT)
	@echo 'source [find $(OPENOCD_TARGET)]' >> $(OPENOCD_SCRIPT)
	@echo 'reset_config srst_only' >> $(OPENOCD_SCRIPT)
	@echo '$$_TARGETNAME configure -event gdb-attach {' >> $(OPENOCD_SCRIPT)
	@echo '   halt' >> $(OPENOCD_SCRIPT)
	@echo '}' >> $(OPENOCD_SCRIPT)
	@echo '$$_TARGETNAME configure -event gdb-attach {' >> $(OPENOCD_SCRIPT)
	@echo '   reset init' >> $(OPENOCD_SCRIPT)
	@echo '}' >> $(OPENOCD_SCRIPT)

# Start OpenOCD GDB Server
openocd: $(OPENOCD_SCRIPT)
	@echo $(MSG_OPENOCD_GDB_SERVER)
	@openocd --version
	openocd --file $(OPENOCD_SCRIPT)

# Generate GDB config/init file
.PHONY : $(GDB_SCRIPT)
$(GDB_SCRIPT) :
	$(RM) $(GDB_SCRIPT)
	@echo '# Load program to debug' >> $(GDB_SCRIPT)
	@echo 'file $(OUT_DIR)/$(PROJECT).elf' >> $(GDB_SCRIPT)
	@echo '# Connect to GDB server' >> $(GDB_SCRIPT)
	@echo 'target extended-remote localhost:$(GDB_PORT)' >> $(GDB_SCRIPT)
	@echo '# Execute a reset and halt of the target' >> $(GDB_SCRIPT)
	@echo 'monitor reset halt' >> $(GDB_SCRIPT)
	@echo '# Flash the program to the target Flash memory' >> $(GDB_SCRIPT)
	@echo 'load' >> $(GDB_SCRIPT)
	@echo '# Add a one-time break point at main' >> $(GDB_SCRIPT)
	@echo 'thbreak main' >> $(GDB_SCRIPT)
	@echo '# Run the program until the break point' >> $(GDB_SCRIPT)
	@echo 'continue' >> $(GDB_SCRIPT)

# Start GDB debugging
gdb: $(GDB_SCRIPT) elf
	@echo $(MSG_GDB)
	$(DEBUGGER) --command=$(GDB_SCRIPT)

# Include the compiler generated dependency files
-include $(OBJECTS:%.o=%.d)

# Listing of phony targets
.PHONY : all begin finish end sizebefore sizeafter gccversion \
         build elf hex bin uf2 prog_uf2 lss sym clean clean_list \
         mostlyclean mostlyclean_list program openocd gdb

