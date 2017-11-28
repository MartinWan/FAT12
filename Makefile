all: diskinfo disklist diskget diskput

diskinfo: diskinfo.c
	gcc -o diskinfo disk.c dir_entry.c diskutils.c diskinfo.c

disklist: disklist.c
	gcc -o disklist disk.c dir_entry.c diskutils.c disklist.c

diskget: diskget.c
	gcc -o diskget disk.c dir_entry.c diskutils.c diskget.c

diskput: diskput.c
	gcc -o diskput disk.c dir_entry.c diskutils.c diskput.c

.PHONY clean:
clean:
	rm diskinfo disklist diskget diskput
