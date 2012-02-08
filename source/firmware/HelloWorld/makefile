include util.mak

CC = sparc-elf-c++.exe
CFLAGS = -c -Wall -Wno-deprecated
LDFLAGS = 
SOURCES = main.cpp 
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = HelloWorld.elf
SRC_DIR = src
OBJ_DIR = target_$(FPGA_BOARD)/obj
ELF_DIR = target_$(FPGA_BOARD)/elf

.PHONY: $(EXECUTABLE)

all: $(SRC_DIR)/$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(MKDIR) ./$(ELF_DIR)
	$(CC) $(LDFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJECTS)) -o $(addprefix $(ELF_DIR)/,$@)

#.cpp.o:
%.o: $(SRC_DIR)/%.cpp
	$(MKDIR) ./$(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $(addprefix $(OBJ_DIR)/,$@)
