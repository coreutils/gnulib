# Makefile for generating the table files from unicode.org data.

ARCHIVE_DIR = /gfs/ibook/Volumes/ExtData/www-archive/software/i18n/unicode/ftp.unicode.org/ArchiveVersions
VERSION = 5.0.0
UCD_DIR = $(ARCHIVE_DIR)/$(VERSION)/ucd

EXTRA_DIST = gen-ctype.c 3levelbit.h 3level.h

all: force
	gcc -g -Wall gen-ctype.c -o gen-ctype
	./gen-ctype $(UCD_DIR)/UnicodeData.txt \
	            $(UCD_DIR)/PropList.txt \
	            $(UCD_DIR)/DerivedCoreProperties.txt \
	            $(UCD_DIR)/Scripts.txt \
	            $(UCD_DIR)/Blocks.txt \
	            $(ARCHIVE_DIR)/3.0.1/PropList-3.0.1.txt \
	            $(VERSION)
	mv test-* ../../tests/unictype/

force:
