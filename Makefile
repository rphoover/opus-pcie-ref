.PHONY: firmware bitstream download default


default: bitstream

sdk: hw/system.make
	$(MAKE) -C hw -f system.make exporttosdk

firmware: sdk
	$(MAKE) -C fw/

bitstream: firmware
	$(MAKE) -C hw -f system.make bits
	cd hw/ && bitinit -p xc5vfx30tff665-1 system.mhs -pe ppc440_0 ../fw/executable.elf -bt implementation/system.bit -o implementation/download.bit

download: bitstream
	$(MAKE) -C hw -f system.make download


hw/system.make:
	cd hw/ && xps -nw -scr genmake.tcl system.xmp


