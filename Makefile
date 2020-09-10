example: example.c $(wildcard gtkwave/*.c gtkwave/*.h)
	gcc -std=c99 example.c $(wildcard gtkwave/*.c) \
		-DFST_CONFIG_INCLUDE="\"fst_config.h\"" \
		-lz -lpthread -o $@

example.fst: example
	./example

wave: example.fst
	gtkwave --save example.gtkw --dump=$<
	sed -i '/^\[dumpfile/d' example.gtkw
	sed -i '/^\[savefile/d' example.gtkw
	sed -i '/^\[\*]/d' example.gtkw
