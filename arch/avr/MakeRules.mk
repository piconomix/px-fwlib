# ------------------------------------------------------------------------------
# Microchip AVR Makefile Template by Pieter Conradie <https://piconomix.com>
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
# Creation Date:  2012-07-13
# ------------------------------------------------------------------------------

# Define programs and commands
CPREFIX  ?= avr
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
    BUILD_DIR ?= Debug
    CDEFS     += $(CDEFS_DEBUG)
    CFLAGS    += $(CFLAGS_DEBUG)
    CXXFLAGS  += $(CXXFLAGS_DEBUG)
    AFLAGS    += $(AFLAGS_DEBUG)
    LDFLAGS   += $(LDFLAGS_DEBUG)
else ifeq ($(BUILD), release)
    BUILD_DIR ?= Release
    CDEFS     += $(CDEFS_RELEASE)
    CFLAGS    += $(CFLAGS_RELEASE)
    CXXFLAGS  += $(CXXFLAGS_RELEASE)
    AFLAGS    += $(AFLAGS_RELEASE)
    LDFLAGS   += $(LDFLAGS_RELEASE)
else
    $(error "Unsupported build specified. Use 'make build=debug' or 'make build=release'")
endif

# Set output directory
#OUT_DIR ?= $(BUILD_DIR)
OUT_DIR ?= .

# Define Messages
MSG_BEGIN               = '------------ begin (build=$(BUILD)) ---------------'
MSG_END                 = '-------------- end (build=$(BUILD)) ---------------'
MSG_SIZE                = 'Size:'
MSG_FLASH_HEX           = 'Creating HEX load file for Flash:'
MSG_FLASH_BIN           = 'Creating BIN load file for Flash:'
MSG_EEPROM              = 'Creating load file for EEPROM:'
MSG_EXTENDED_LISTING    = 'Creating Extended Listing:'
MSG_SYMBOL_TABLE        = 'Creating Symbol Table:'
MSG_LINKING             = 'Linking:'
MSG_ARCHIVING           = 'Creating static library:'
MSG_COMPILING           = 'Compiling C:'
MSG_COMPILING_CXX       = 'Compiling C++:'
MSG_ASSEMBLING          = 'Assembling:'
MSG_CLEANING            = 'Cleaning project:'

# Define all object files from source files (C, C++ and Assembly)
#     Relative paths are stripped and BUILD_DIR prefix is added.
OBJECTS = $(foreach file,$(SRC) $(CXXSRC) $(ASRC), $(BUILD_DIR)/$(basename $(notdir $(file))).o)

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
ALL_CFLAGS   ?= -mmcu=$(MCU) $(CFLAGS) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@) -I. $(addprefix -I,$(INCDIRS)) $(DEP_FLAGS)
ALL_CXXFLAGS ?= -mmcu=$(MCU) -x c++ $(CXXFLAGS) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@) -I. $(addprefix -I,$(INCDIRS)) $(DEP_FLAGS)
ALL_AFLAGS   ?= -mmcu=$(MCU) -x assembler-with-cpp $(AFLAGS) -I. $(addprefix -I,$(INCDIRS)) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@),--listing-cont-lines=100,--gstabs
ALL_LDFLAGS  ?= -mmcu=$(MCU) $(LDFLAGS) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map $(addprefix -L,$(EXTRALIBDIRS))

# Default target
all: begin gccversion build size end

# Build targets
build: elf hex bin eep lss sym

elf: $(OUT_DIR)/$(PROJECT).elf
hex: $(OUT_DIR)/$(PROJECT).hex
bin: $(OUT_DIR)/$(PROJECT).bin
eep: $(OUT_DIR)/$(PROJECT).eep
lss: $(BUILD_DIR)/$(PROJECT).lss
sym: $(BUILD_DIR)/$(PROJECT).sym

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
	$(SIZE) --mcu=$(MCU) --format=avr $<

# Display compiler version information
gccversion :
	@$(CC) --version

# Create final output file (*.hex) from ELF output file
$(OUT_DIR)/%.hex: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_FLASH_HEX) $@
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock $< $@

# Create final output file (*.bin) from ELF output file
$(OUT_DIR)/%.bin: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_FLASH_BIN) $@
	$(OBJCOPY) -O binary -R .eeprom -R .fuse -R .lock $< $@

# Create EEPROM init file (*.eep) from ELF output file
$(OUT_DIR)/%.eep: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0

# Create extended listing file (*.lss) from ELF output file
$(BUILD_DIR)/%.lss: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table (*.sym) from ELF output file
$(BUILD_DIR)/%.sym: $(OUT_DIR)/%.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@


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
	$(RM) $(OUT_DIR)/$(PROJECT).eep
	$(RMDIR) $(BUILD_DIR)

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
	atprogram -t atmelice --interface ISP --clock 125khz --device $(MCU) program --chiperase --file $< --verify

# Program target using command line utility
reset:
	atprogram -t atmelice --interface ISP --clock 125khz --device $(MCU) reset

# Include the compiler generated dependency files
-include $(OBJECTS:%.o=%.d)

# Listing of phony targets
.PHONY : all begin finish end sizebefore sizeafter gccversion \
         build elf hex bin eep lss sym clean clean_list \
         mostlyclean mostlyclean_list program reset

