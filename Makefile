PROJECTS=libc fdlibm kernel
HOST?=$(shell ./scripts/default-host.sh)
HOSTARCH:=$(shell ./scripts/target-triplet-to-arch.sh $(HOST))

include make.config

all: clean build iso

clean:
	$(MAKE) -C fdlibm clean
	$(MAKE) -C libc clean
	$(MAKE) -C kernel clean
	rm -rf sysroot
	rm -rf isodir
	rm -rf byok.iso

headers:
	DESTDIR="$(PWD)/sysroot" $(MAKE) -C libc install-headers
	DESTDIR="$(PWD)/sysroot" $(MAKE) -C kernel install-headers

$(PROJECTS): headers
	DESTDIR="$(PWD)/sysroot" $(MAKE) -C $@ install

build: $(PROJECTS)

iso: build
	mkdir -p isodir/boot/grub
	cp sysroot/boot/byok.kernel isodir/boot/byok.kernel
	cp boot/grub/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o byok.iso isodir

qemu: iso
	qemu-system-$(HOSTARCH) -cdrom byok.iso

.PHONY: all $(PROJECTS)