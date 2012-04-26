include util.mak

SOURCES = uart.cpp \
	timer.cpp \
	tickhandler.cpp \
	irqctrl.cpp \
	gnssengine.cpp \
	sysctrl.cpp \
	main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = HelloWorld.elf
SRC_DIR = src
OBJ_DIR = target_$(FPGA_BOARD)/obj
ELF_DIR = target_$(FPGA_BOARD)/elf

.PHONY: $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(MKDIR) ./$(ELF_DIR)
	$(CC) $(LDFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJECTS)) -o $(addprefix $(ELF_DIR)/,$@)

#.cpp.o:
%.o: $(SRC_DIR)/%.cpp
	$(MKDIR) ./$(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $(addprefix $(OBJ_DIR)/,$@)
