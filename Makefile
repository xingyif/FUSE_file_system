
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
HDRS := $(wildcard *.h)

CFLAGS := -std=c11 -g `pkg-config fuse --cflags`
LDLIBS := `pkg-config fuse --libs` 

nufs: $(OBJS)
	gcc $(CLFAGS) -o $@ $^ $(LDLIBS)

%.o: %.c $(HDRS)
	gcc $(CFLAGS) -c -o $@ $<

clean: unmount
	rm -f nufs *.o test.log
	rmdir mnt || true

mount: nufs
	mkdir -p mnt || true
	./nufs -s -f mnt data.nufs

unmount:
	fusermount -u mnt || true

test: nufs
	perl test.t

gdb: nufs
	mkdir -p mnt || true
	gdb --args ./nufs -f mnt data.nufs

.PHONY: clean mount unmount gdb

