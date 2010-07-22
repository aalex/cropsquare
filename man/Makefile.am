dist_man_MANS = cropsquare.1
MAN_TEXT_FILES=man-cropsquare.txt
EXTRA_DIST = $(man_MANS) $(MAN_TEXT_FILES)
MAINTAINERCLEANFILES = $(dist_man_MANS)

# The man pages depend on the --help strings and the version number.
common_mandeps = $(top_srcdir)/configure.ac

# Depend on the source, not the binary; we don't need to regenerate the
# binary when any source file changes, only the main one.
# Use -o so that the `missing' program can infer the output file.
cropsquare.1: $(top_srcdir)/src/main.cpp $(common_mandeps) $(top_srcdir)/man/man-cropsquare.txt
	$(HELP2MAN) --include=$(top_srcdir)/man/man-cropsquare.txt --name="Crops and resizes images" --no-info $(top_builddir)/src/cropsquare -o $@

