.PHONY: all clean

TARG = strings-cp932
OFILES = strings-cp932.o

all: $(TARG)
$(TARG): $(OFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OFILES)

strings-cp932.c: _cp932tab.c

_cp932tab.c: cp932tab.awk CP932.TXT
	awk -f cp932tab.awk -F'	' CP932.TXT > $@

$(TARG).1: $(TARG).1.scd
	scdoc < $< > $@ || rm $@

clean:
	rm -f $(TARG) $(OFILES) _cp932tab.c
