BUILDDIR := build
DBGDIR := $(BUILDDIR)/dbg
TGTDIR := $(BUILDDIR)/target
SRCDIR := src
INCDIR := include
TGTNAME := shell

CC := gcc
CFLAGS := -I$(INCDIR) -c -w
DBGFLAGS := $(CFLAGS) -ggdb

SOURCES := $(shell find $(SRCDIR) -type f -iname '*.c')
HEADERS := $(shell find $(SRCDIR) -type f -iname '*.h')
TGTOBJ := $(subst src,$(TGTDIR),$(SOURCES:.c=.o))
DBGOBJ := $(subst src,$(DBGDIR),$(SOURCES:.c=.o))
TGTDIRS := $(subst src,$(TGTDIR),$(dir $(SOURCES)))
DBGDIRS := $(subst src,$(DBGDIR),$(dir $(SOURCES)))

build: target

run: target
	./$(TGTDIR)/$(TGTNAME) $(filter-out $@,$(MAKECMDGOALS))

target: $(TGTDIRS) $(SOURCES) $(HEADERS) $(TGTOBJ)
	$(CC) -o $(TGTDIR)/$(TGTNAME) $(TGTOBJ)

debug: $(DBGDIRS) $(SOURCES) $(HEADERS) $(DBGOBJ)
	$(CC) -o $(DBGDIR)/$(TGTNAME) $(DBGOBJ)

clean:
	rm -rf $(BUILDDIR)

$(DBGDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) -o $@ $(DBGFLAGS) $<

$(TGTDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) -o $@ $(CFLAGS) $<

$(TGTDIRS):
	mkdir -p $(TGTDIRS)

$(DBGDIRS):
	mkdir -p $(DBGDIRS)

%:
	@:
