
noinst_DATA = bios.bin
EXTRA_DIST=bios.bin bios.zsm
CLEANFILES = bios.lis

bios.bin:	bios.zsm
	z80asm -fb -l bios.zsm

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cpm/bin80
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cpm/lib80
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cpm/include80
	$(INSTALL_DATA) $(srcdir)/bios.bin $(DESTDIR)$(libdir)/cpm/bin80/bios.bin

uninstall-local:
	$(RM) $(DESTDIR)$(libdir)/cpm/bin80/bios.bin	
