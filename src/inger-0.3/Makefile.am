####### kdevelop will overwrite this part!!! (begin)##########

SUBDIRS = compiler extra 

EXTRA_DIST = compiler.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO compiler.lsm 

####### kdevelop will overwrite this part!!! (end)############
# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

# Names for tarballs.
DEVPACKAGENAME = inger-0.3

# Define what gets included in the development package.
# Don't say things like PACK_BLAH = blah/* because then you will also include
# the CVS subdirectory of blah.
PACK_INGER = AUTHORS COPYING ChangeLog INSTALL Makefile.am Makefile.in Makefile.dist README TODO acconfig.h acinclude.m4 acinclude.m4.in compiler.kdevprj compiler.lsm config.guess config.h.in config.sub configure configure.in configure.in.no_shlib install-sh libtool.m4.in ltconfig ltmain.sh missing mkinstalldirs special.m4.in stamp-h.in
PACK_COMPILER = compiler/*.c compiler/*.h compiler/lexer.l compiler/Makefile.am compiler/Makefile.in compiler/bnf.b
PACK_TEMPLATES = compiler/templates/header_template
PACK_DOCS = compiler/docs/Makefile.am compiler/docs/Makefile.in
PACK_EN = compiler/docs/en/Makefile.am compiler/docs/en/Makefile.in compiler/docs/en/*.html
PACK_BOOK = compiler/docs/book/Makefile.am compiler/docs/book/Makefile.in compiler/docs/book/gentex.pl compiler/docs/book/*.otx compiler/docs/book/*.sty compiler/docs/book/*.tab compiler/docs/book/*.png compiler/docs/book/*.vsd
PACK_EXTRA = extra/*.vim
PACK_SAMPLES = compiler/samples/*.i compiler/samples/*.ih

devpackage: 
	tar -czf $(DEVPACKAGENAME).tar.gz $(PACK_INGER) $(PACK_COMPILER) $(PACK_TEMPLATES) $(PACK_DOCS) $(PACK_EN) $(PACK_BOOK) $(PACK_EXTRA) $(PACK_SAMPLES)
	rm -Rf $(DEVPACKAGENAME)
	mkdir -p $(DEVPACKAGENAME)
	tar --directory=$(DEVPACKAGENAME) -xzf $(DEVPACKAGENAME).tar.gz
	tar -czf $(DEVPACKAGENAME).tar.gz $(DEVPACKAGENAME)/
	rm -Rf $(DEVPACKAGENAME)

	
	

