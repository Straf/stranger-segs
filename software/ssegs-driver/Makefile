# Defines project name and standard directories
PROJECT := ssegs-driver
SRC := src
BUILD := build

# Defines variables to use gcc.
CC := gcc
CFLAGS = -Os
CPPFLAGS = -std=c++17 -iquote $(SRC)
LDLIBS = -lc++ -lstdc++
LDFLAGS = -Wl
CXX := gcc
CXXFLAGS = -Os
AS := as
ASFLAGS = -Os

#
# Rules and goals
#

EXECUTABLE := $(PROJECT)
LISTFILE := $(BUILD)/$(PROJECT).lst
CSOURCES := $(wildcard $(SRC)/*.c)
CXXSOURCES := $(wildcard $(SRC)/*.cpp)
ASOURCES := $(wildcard $(SRC)/*.S)
OBJECTS := $(addprefix $(BUILD)/,$(notdir $(CXXSOURCES:.cpp=.o)) $(notdir $(CSOURCES:.c=.o)) $(notdir $(ASOURCES:.S=.o)))
DEPENDENCIES := $(addprefix $(BUILD)/,$(notdir $(CXXSOURCES:.cpp=.d)) $(notdir $(CSOURCES:.c=.d)))

.PHONY: all clean disasm

all : $(EXECUTABLE)

clean :
	@rm -Rf $(BUILD) $(EXECUTABLE)

disasm: $(EXECUTABLE)
	@objdump -d $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS) | $(BUILD)
	@$(CC) $(LDFLAGS) -Wl,$(LDLIBS) $^ -o $@

$(BUILD):
	@test -d $(BUILD) || mkdir $(BUILD)

$(BUILD)/%.o : $(SRC)/%.c | $(BUILD)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.S | $(BUILD)
	@$(CC) -x assembler-with-cpp $(ASFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.s | $(BUILD)
	@$(AS) $(ASFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/%.d : $(SRC)/%.c | $(BUILD)
	@set -e; rm -f $@ ; \
	$(CC) -MM $(CFLAGS) $(CPPFLAGS) $< > $@.$$$$ ; \
	sed 's,\($*\)\.o[ :]*,$(BUILD)\/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD)/%.d : $(SRC)/%.cpp | $(BUILD)
	@set -e; rm -f $@ ; \
	$(CXX) -MM $(CXXFLAGS) $(CPPFLAGS) $< > $@.$$$$ ; \
	sed 's,\($*\)\.o[ :]*,$(BUILD)\/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPENDENCIES)
