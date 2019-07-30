
CC := gcc
LDFLAGS := -lcurl
TARGETS := post-file
INCLUDES :=
CFLAGS :=
INCLIBS :=

all: $(TARGETS)

%.c.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<
	
%:
	$(CC) -o $@ $^ $(INCLIBS) $(LDFLAGS)

$(TARGETS): form-data.c

.PHONY: all