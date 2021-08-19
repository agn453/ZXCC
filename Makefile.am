## Process this file with automake to produce Makefile.in

SUBDIRS= . @LSUBDIRS@ Z80 bin 
EXTRA_DIST= $(srcdir)/config/* $(srcdir)/zxcc.doc $(srcdir)/zxcc.html
pkgdata_DATA = zxcc.doc

ACLOCAL_AMFLAGS=-I m4

zxcc.doc:	$(srcdir)/zxcc.html
	lynx -dump $(srcdir)/zxcc.html > zxcc.doc
