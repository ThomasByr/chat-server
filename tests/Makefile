CC = gcc

CFLAGS ?= -march=znver3 -Og -pipe -std=gnu17 -pedantic -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -DDEBUG -g -ggdb
LDLIBS ?= -lm -pthread -lreadline

INCLUDE_PATH = ../inc

TARGET       = tests
FILEXT       = c

SRCDIR       = ../src
OBJDIR       = obj

PATH_TO_EXE  = $(TARGET)


SOURCES  := $(wildcard $(SRCDIR)/*.$(FILEXT))
INCLUDES := $(wildcard $(INCLUDE_PATH)/*.h)
OBJECTS0 := $(SOURCES:$(SRCDIR)/%.$(FILEXT)=$(OBJDIR)/%.o)
OBJECTS1  = $(filter-out $(OBJDIR)/main-client.o,$(OBJECTS0))
OBJECTS   = $(filter-out $(OBJDIR)/main-server.o,$(OBJECTS1))


all : $(PATH_TO_EXE)

cov: clean tests
	valgrind --leak-check=full -q ./$(PATH_TO_EXE)
	lcov -d obj -c -o report.info
	genhtml -o report -t "Coverage" report.info

check: clean tests
	valgrind --leak-check=full --show-leak-kinds=all --vgdb=full -s ./$(PATH_TO_EXE)

$(PATH_TO_EXE): $(OBJECTS) $(OBJDIR)/$(TARGET).o
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "\033[92mLinking complete!\033[0m"
	@echo "\033[93mRunning in debug mode!\033[0m"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.$(FILEXT) $(INCLUDES)
	mkdir -p $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) -isystem$(INCLUDE_PATH)

obj/$(TARGET).o: $(TARGET).$(FILEXT)
	$(CC) -o $@ -c $< $(CFLAGS) -isystem$(INCLUDE_PATH)


.PHONY: clean
clean:
	rm -f $(OBJDIR)/*
	rm -f *.gcno
	rm -f $(PATH_TO_EXE)
