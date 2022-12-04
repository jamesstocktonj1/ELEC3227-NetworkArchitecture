# AVR Makefile
# James Stockton


# make file commands
#	all 	- compiles whole program
#	disasm 	- disassembles elf file
#	size 	- shows size of elf file
#	test	- test programmer connection
#	flash	- upload hex to target
#	fuse	- set fuses of target
#	clean	- deletes compiled files
#	help	- displays usage message

# avr constants
MCU = atmega644p
CLK = 12000000

# compiler constants
CC = avr-gcc
TEST-CC = gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
OBJSIZE = avr-size

# compiler flags
CFLAGS = -Wall -Os
TARGET = main
TESTFLAGS = -Wall -Os -lstdc++ -DTEST

DLL_RF_TEST_TARGET = dll_rf_test

# build and source folders
BUILD = build
SOURCE = src
SOURCES = $(wildcard $(SOURCE)/*.c)
INCLUDE = include
INCLUDES = $(wildcard $(INCLUDE)/*.h)
OBJECTS = $(subst $(SOURCE), $(BUILD), $(SOURCES:.c=.o))

# library folders
LIB =
LIB_BUILD = build/lib
LIBRARYS = $(wildcard $(LIB)/*.c)
#LIB_OBJECTS = $(LIB_BUILD)/rfm12.o
LIB_OBJECTS = $(foreach x,$(LIB),$(subst $(x), $(LIB_BUILD), $(LIBRARYS:.c=.o)))

# test files
TEST = test
TESTS = $(wildcard $(TEST)/*.c)
TESTS_INCLUDE = $(wildcard $(TEST)/*.h)
TEST_FILES = \
	src/application.c \
	src/network.c \
	src/datalink.c \
	src/util.c
TEST_INCLUDE = $(subst $(SOURCE), $(INCLUDE), $(TEST_FILES:.c=.h))
TEST_TARGET = test.out

#programmer constant
AVRDUDE = avrdude 
PROGRAMMER = usbasp

CFLAGS	+= -D__PLATFORM_AVR__

#fuses
LF = 0xff
HF = 0x9c
EF = 0xff


all: $(BUILD)/$(TARGET).hex

test: $(BUILD)/$(TEST_TARGET)
	$(BUILD)/$(TEST_TARGET)

.PHONY: dll_rf_test
dll_rf_test: $(BUILD)/$(DLL_RF_TEST_TARGET).hex

disasm: $(BUILD)/$(TARGET).elf
	$(OBJDUMP) -d $(BUILD)/$(TARGET).elf

size: $(BUILD)/$(TARGET).elf
	$(OBJSIZE) -C $(BUILD)/$(TARGET).elf

test-flash:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU)

flash: all
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$(BUILD)/$(TARGET).hex

flash-dll: dll_rf_test
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$(BUILD)/$(DLL_RF_TEST_TARGET).hex

fuse:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:$(LF):m -U hfuse:w:$(HF):m -U efuse:w:$(EF):m

clean:
	@rm -f $(BUILD)/$(TARGET).elf
	@rm -f $(BUILD)/$(TARGET).hex
	@rm -f $(BUILD)/$(DLL_RF_TEST_TARGET).hex
	@rm -f $(OBJECTS)
	@rm -f $(LIB_OBJECTS)
	@rm -f $(BUILD)/$(TEST_TARGET)
	@rm -f $(BUILD)/temp.txt
	
help:
	@echo "Il Matto Makefile Usage"
	@echo "	all		- compiles whole program"
	@echo "	disasm		- disassembles elf file"
	@echo "	size		- shows size of elf file"
	@echo "	test-flash	- test programmer connection"
	@echo "	flash		- upload hex to target"
	@echo "	fuse		- set fuses of target"
	@echo "	clean		- deletes compiled files"
	@echo "	test		- runs the tests found in test/"

$(BUILD)/%.o: $(SOURCE)/%.c $(INCLUDES)
	$(CC) -DF_CPU=$(CLK) -mmcu=$(MCU) $(CFLAGS) -c $< -o $@


$(LIB_OBJECTS): $(LIBRARYS)
	$(CC) -DF_CPU=$(CLK) -mmcu=$(MCU) $(CFLAGS) $(addprefix -I,$(INCLUDE)) -c $< -o $@


$(BUILD)/$(TARGET).elf: $(TARGET).c $(OBJECTS) $(LIB_OBJECTS)
	$(CC) -DF_CPU=$(CLK) -mmcu=$(MCU) $(CFLAGS) $(TARGET).c $(OBJECTS) $(LIB_OBJECTS) -o $(BUILD)/$(TARGET).elf

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).hex -O ihex

$(BUILD)/%.elf: %.c $(OBJECTS) $(LIB_OBJECTS)
	$(CC) -DF_CPU=$(CLK) -mmcu=$(MCU) $(CFLAGS) $< $(OBJECTS) $(LIB_OBJECTS) -o $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) $< $@ -O ihex


$(BUILD)/$(TEST_TARGET): $(TESTS) $(TEST_FILES) $(TEST_INCLUDE) $(TESTS_INCLUDE)
	$(TEST-CC) $(TESTFLAGS) $(TESTS) $(TEST_FILES) -o $@
