.PHONY: firmware bitstream download default


default: bitstream

sdk: hw/system.make
	$(MAKE) -C hw -f system.make exporttosdk

firmware: sdk
	$(MAKE) -C fw/

bitstream: firmware
	$(MAKE) -C hw -f system.make init_bram

download: bitstream
	$(MAKE) -C hw -f system.make download


hw/system.make:
	cd hw/ && xps -nw -scr genmake.tcl system.xmp


