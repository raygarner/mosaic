DEST = /usr/bin
SRC = md.c crd.c cf.c fb.c int.c ks.c mld.c hrm.c mxml.c dg.c cid.c
EXES = $(SRC:.c=)
COMMON = common
MYCFLAGS = -std=c99 -W -Wall -pedantic -Wextra -g -O0
PROJNAME = mosaic
CC = gcc

all: $(EXES)

options:
	@echo "DEST     = $(DEST)"
	@echo "SRC      = $(SRC)"
	@echo "EXES     = $(EXES)"
	@echo "MYCFLAGS = $(MYCFLAGS)"
	@echo "CC       = $(CC)"

clean:
	rm -rf $(EXES) *.musicxml $(PROJNAME) $(PROJNAME).tar.gz \
	    $(PROJNAME).zip dist

install: all
	cp -f $(EXES) $(DEST)

uninstall:
	@$(foreach EXE,$(EXES), rm -f $(DEST)/$(EXE))

dist:
	mkdir -p $(PROJNAME)
	cp -R LICENSE README TODO Makefile $(COMMON).* $(SRC) $(PROJNAME)

tar: dist
	tar -cf - $(PROJNAME) | gzip > $(PROJNAME).tar.gz

zip: dist
	zip -r $(PROJNAME).zip $(PROJNAME)

md:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

crd:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

cf:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

fb:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

int:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

ks:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

mld:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

hrm:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

mxml:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

dg:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@

cid:
	$(CC) $(MYCFLAGS) $@.c $(COMMON).c -o $@
	
.PHONY: all full options clean install uninstall tar zip dist
