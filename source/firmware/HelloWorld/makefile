include util.mak

CC = sparc-elf-c++.exe
CFLAGS = -c -Wall -Wno-deprecated -D$(FPGA_BOARD)
LDFLAGS = 
#Alternative entry point 
#LDFLAGS =  -Ttext=0xA0000000 
# -Tdata=0xA0010000  -Tbss=0xA0030000
SOURCES = main.cpp \
	uart.cpp \
	timer.cpp \
	tickhandler.cpp \
	irqctrl.cpp \
	sysctrl.cpp
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
