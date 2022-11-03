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
TESTFLAGS = -lstdc++ -Os

# build and source folders
BUILD = build
SOURCE = src
SOURCES = $(wildcard $(SOURCE)/*.c)
INCLUDE = include
INCLUDES = $(wildcard $(INCLUDE)/*.h)
OBJECTS = $(subst $(SOURCE), $(BUILD), $(SOURCES:.c=.o))
#LIB = lib
#LIBRARY = $(wildcard $(LIB)/*.c)

# test files
TEST = test
TESTS = $(wildcard $(TEST)/*.c)
TEST_TARGET = test.out

#programmer constant
AVRDUDE = avrdude 
PROGRAMMER = usbasp

#fuses
LF = 0xff
HF = 0x9c
EF = 0xff


all: $(BUILD)/$(TARGET).hex

test: $(BUILD)/$(TEST_TARGET)
	$(BUILD)/$(TEST_TARGET)

disasm: $(BUILD)/$(TARGET).elf
	$(OBJDUMP) -d $(BUILD)/$(TARGET).elf

size: $(BUILD)/$(TARGET).elf
	$(OBJSIZE) -C $(BUILD)/$(TARGET).elf

test-flash:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU)

flash: all
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$(BUILD)/$(TARGET).hex

fuse:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:$(LF):m -U hfuse:w:$(HF):m -U efuse:w:$(EF):m

clean:
	@rm -f $(BUILD)/$(TARGET).elf
	@rm -f $(BUILD)/$(TARGET).hex
	@rm -f $(OBJECTS)
	@rm -f $(BUILD)/$(TEST_TARGET)
	
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

$(BUILD)/$(TARGET).elf: $(TARGET).c $(OBJECTS)
	$(CC) -DF_CPU=$(CLK) -mmcu=$(MCU) $(CFLAGS) $(TARGET).c $(OBJECTS) -o $(BUILD)/$(TARGET).elf

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).hex -O ihex


$(BUILD)/$(TEST_TARGET): $(TESTS)
	$(TEST-CC) $(TESTFLAGS) $(TESTS) -o $@