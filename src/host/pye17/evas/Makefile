EVAS_PATH=$(E_PATH)/evas/src/lib

CFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/python2.5 -I/usr/include/pygtk-2.0/ -I$(E_PATH)/evas/src/lib/ -I.

.PHONY: all clean evas.so

all: evas.so

evas.so: evas.o evasmodule.o
	$(CC) $(LDFLAGS) -shared $^ -o $@

evas.c: evas_data.defs evas.defs evas.override
	$(PYTHON) $(CODEGEN_PATH)/codegen.py --prefix evas \
	--override evas.override --register evas_data.defs \
	evas.defs > $@

evas_data.defs: $(CODEGEN_PATH)/h2def.py $(EVAS_PATH)/Evas.h $(EVAS_PATH)/Evas_Data.h
	$(CODEGEN_PATH)/h2def.py $(EVAS_PATH)/Evas_Data.h > $@

evas.defs: $(CODEGEN_PATH)/h2def.py $(EVAS_PATH)/Evas.h $(EVAS_PATH)/Evas_Data.h
	$(CODEGEN_PATH)/h2def.py $(EVAS_PATH)/Evas.h > $@

clean:
	rm -f *.o evas.c evas.defs