CC = gcc

CFLAGS = -march=native -pipe -std=gnu17 -pedantic -Wall -Wextra -Werror
LDLIBS = -lm -pthread -lreadline

INCLUDE_PATH = ./inc

TARGET0      = client
TARGET1      = server
FILEXT       = c

SRCDIR       = src
OBJDIR       = obj
BINDIR       = bin

SOURCES     := $(wildcard $(SRCDIR)/*.$(FILEXT))
INCLUDES    := $(wildcard $(INCLUDE_PATH)/*.h)
OBJECTS     := $(SOURCES:$(SRCDIR)/%.$(FILEXT)=$(OBJDIR)/%.o)


all: debug-client debug-server

debug: debug-client debug-server

release: release-client release-server

debug-client: CFLAGS += -Og -DDEBUG -g -ggdb
debug-client: $(BINDIR)/$(TARGET0)
	@echo "\033[93mRunning client in debug mode!\033[0m"

debug-server: CFLAGS += -Og -DDEBUG -g -ggdb
debug-server: $(BINDIR)/$(TARGET1)
	@echo "\033[93mRunning server in debug mode!\033[0m"

release-client: CFLAGS += -Ofast
release-client: $(BINDIR)/$(TARGET0)
	@echo "\033[94mRunning client in release mode!\033[0m"

release-server: CFLAGS += -Ofast
release-server: $(BINDIR)/$(TARGET1)
	@echo "\033[94mRunning server in release mode!\033[0m"

run-server: debug-server
	./bin/server -p 36000

run-client: debug-client
	./bin/client -p 36000 -t 127.0.0.1

run-server-rpi: release-server
	./bin/server -p 32100

run-client-rpi: release-client
	./bin/client -p 32100 -t 88.170.206.241

$(BINDIR)/$(TARGET0): $(filter-out $(OBJDIR)/main-server.o,$(OBJECTS))
	mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "\033[92mLinking complete!\033[0m"

$(BINDIR)/$(TARGET1): $(filter-out $(OBJDIR)/main-client.o,$(OBJECTS))
	mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "\033[92mLinking complete!\033[0m"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.$(FILEXT) $(INCLUDES)
	mkdir -p $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) -isystem$(INCLUDE_PATH)


.PHONY: clean
clean:
	rm -f $(OBJDIR)/*
	rm -f $(BINDIR)/*
