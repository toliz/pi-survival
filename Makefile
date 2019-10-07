# Target executable
TARGET   = pimessenger

# Compiler
CC       = arm-linux-gnueabihf-gcc
CFLAGS   = -pthread -I ./lib -w

# Linker
LINKER   = arm-linux-gnueabihf-gcc
LFLAGS   = -pthread -lm

# Dir structure
SRCDIR   = src
LIBDIR	 = lib
OBJDIR   = obj
BINDIR   = bin

# Files
SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(LIBDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

#PI
USER	 = root
IP		 = 192.168.0.1

all: $(BINDIR)/$(TARGET)
	@if ping -c 1 -w 1 $(IP) > /dev/null; then \
	echo "Pi found, transferring executable..."; \
	scp $(BINDIR)/$(TARGET) $(USER)@$(IP):~/; \
	else echo "Pi not found, scp aborted."; \
	fi
	@echo "Ready!"
	

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete."

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully."

.PHONY: clean
clean:
	@rm -rf $(OBJDIR)
	@echo "Cleanup complete."

.PHONY: purge
purge: clean
	@rm -rf $(BINDIR)
	@echo "Executable removed."