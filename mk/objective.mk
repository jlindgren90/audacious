# Shut up GNU make
.SILENT:

OBJECTIVE_DIRECTORIES = none
OBJECTIVE_LIBS = none
OBJECTIVE_LIBS_NOINST = none
OBJECTIVE_BINS = none
OBJECTIVE_DATA = none
SUBDIRS = none
HEADERS = none

LIBDIR = $(libdir)
BINDIR = $(bindir)
INCLUDEDIR = $(pkgincludedir)
CFLAGS += -DHAVE_CONFIG_H

default: all
all: build

install:
	$(MAKE) install-prehook
	@for i in $(BINDIR) $(LIBDIR) $(INCLUDEDIR); do \
		if test ! -d $(DESTDIR)/$$i; then \
			$(INSTALL) -d -m 755 $(DESTDIR)/$$i; \
		fi; \
	done;
	@if test "$(SUBDIRS)" != "none"; then \
		for i in $(SUBDIRS); do \
			echo "[installing subobjective: $$i]"; \
			(cd $$i; $(MAKE) install; cd ..); \
		done; \
	fi
	@if test "$(OBJECTIVE_DIRECTORIES)" != "none"; then \
		for i in $(OBJECTIVE_DIRECTORIES); do \
			printf "%10s     %-20s\n" MKDIR $$i; \
			$(INSTALL) -d -m 755 $(DESTDIR)/$$i; \
		done; \
	fi
	@if test "$(HEADERS)" != "none"; then \
		for i in $(HEADERS); do \
			printf "%10s     %-20s\n" INSTALL $$i; \
			$(INSTALL_DATA) $(INSTALL_OVERRIDE) $$i $(DESTDIR)/$(INCLUDEDIR)/$$i; \
		done; \
	fi
	@if test "$(OBJECTIVE_LIBS)" != "none"; then \
		for i in $(OBJECTIVE_LIBS); do \
			printf "%10s     %-20s\n" INSTALL $$i; \
			$(INSTALL) $(INSTALL_OVERRIDE) $$i $(DESTDIR)/$(LIBDIR)/$$i; \
		done; \
	fi
	@if test "$(OBJECTIVE_BINS)" != "none"; then \
		for i in $(OBJECTIVE_BINS); do \
			printf "%10s     %-20s\n" INSTALL $$i; \
			$(INSTALL) $(INSTALL_OVERRIDE) $$i $(DESTDIR)/$(BINDIR)/$$i; \
		done; \
	fi;
	@if test "$(OBJECTIVE_DATA)" != "none"; then \
		for i in $(OBJECTIVE_DATA); do \
			source=`echo $$i | cut -d ":" -f1`; \
			destination=`echo $$i | cut -d ":" -f2`; \
			if test ! -d $(DESTDIR)/$$destination; then \
				$(INSTALL) -d -m 755 $(DESTDIR)/$$destination; \
			fi; \
			printf "%10s     %-20s\n" INSTALL $$source; \
			$(INSTALL_DATA) $(INSTALL_OVERRIDE) $$source $(DESTDIR)/$$destination; \
		done; \
	fi
	$(MAKE) install-posthook
	@echo "[all objectives installed]"

clean:
	$(MAKE) clean-prehook
	@if test "$(SUBDIRS)" != "none"; then \
		for i in $(SUBDIRS); do \
			echo "[cleaning subobjective: $$i]"; \
			(cd $$i; $(MAKE) clean; cd ..); \
		done; \
	fi
	$(MAKE) clean-posthook
	$(RM) *.o *.lo *.so *.a *.sl
	@echo "[all objectives cleaned]"

distclean: clean
	@if test "$(SUBDIRS)" != "none"; then \
		for i in $(SUBDIRS); do \
			echo "[distcleaning subobjective: $$i]"; \
			(cd $$i; $(MAKE) distclean; cd ..); \
			echo "[distcleaning subobjective: $$i]"; \
		done; \
	fi
	@if test -f Makefile.in; then \
		$(RM) -f Makefile; \
	fi
	@if test -f mk/rules.mk; then \
		$(RM) -f mk/rules.mk; \
	fi

build:
	$(MAKE) build-prehook
	@if test "$(SUBDIRS)" != "none"; then \
		for i in $(SUBDIRS); do \
			echo "[building subobjective: $$i]"; \
			(cd $$i; $(MAKE); cd ..); \
			echo "[finished subobjective: $$i]"; \
		done; \
	fi
	@if test "$(OBJECTIVE_LIBS)" != "none"; then \
		for i in $(OBJECTIVE_LIBS); do \
			echo "[building library objective: $$i]"; \
			$(MAKE) $$i; \
			echo "[finished library objective: $$i]"; \
		done; \
	fi
	@if test "$(OBJECTIVE_LIBS_NOINST)" != "none"; then \
		for i in $(OBJECTIVE_LIBS_NOINST); do \
			echo "[building library dependency: $$i]"; \
			$(MAKE) $$i; \
			echo "[finished library dependency: $$i]"; \
		done; \
	fi
	@if test "$(OBJECTIVE_BINS)" != "none"; then \
		for i in $(OBJECTIVE_BINS); do \
			echo "[building binary objective: $$i]"; \
			$(MAKE) $$i; \
			echo "[finished binary objective: $$i]"; \
		done; \
	fi
	$(MAKE) build-posthook
	@echo "[all objectives built]"

.c.o:
	printf "%10s     %-20s\n" CC $<
	$(CC) $(CFLAGS) -c $< -o $@

.cc.o:
	printf "%10s     %-20s\n" CXX $<;
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cpp.o:
	printf "%10s     %-20s\n" CXX $<;
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cxx.o:
	printf "%10s     %-20s\n" CXX $<;
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.so: $(OBJECTS)
	if test "x$(OBJECTS)" != "x"; then \
		$(MAKE) $(OBJECTS);		\
		printf "%10s     %-20s\n" LINK $@; \
		$(CC) -fPIC -DPIC -shared -o $@ -Wl,-soname=$@ $(OBJECTS) $(LDFLAGS) $(LIBADD); \
	fi

%.a: $(OBJECTS)
	if test "x$(OBJECTS)" != "x"; then \
		$(MAKE) $(OBJECTS);		\
		printf "%10s     %-20s\n" LINK $@; \
		$(AR) cq $@ $(OBJECTS); \
	fi

$(OBJECTIVE_BINS): $(OBJECTS)
	if test "x$(OBJECTS)" != "x"; then \
		$(MAKE) $(OBJECTS);		\
		printf "%10s     %-20s\n" LINK $@; \
		$(CC) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBADD); \
	fi

clean-prehook:
clean-posthook:
build-prehook:
build-posthook:
install-prehook:
install-posthook:

# compatibility with automake follows
am--refresh:
