SCRIPTS		= earthlive-cpp lib/io.lib.sh lib/integration.lib.sh
SED			= /bin/sed
SHELL		= /bin/bash
DESTDIR		?= /
PREFIX		?= /usr/local
INSTALL		= /usr/bin/env install
MSGFMT		= /usr/bin/msgfmt
bindir		= $(DESTDIR)$(PREFIX)/bin
localedir	= $(DESTDIR)$(PREFIX)/share/locale
libdir		= $(DESTDIR)$(PREFIX)/lib/earthlive-cpp
applicationdir	= $(DESTDIR)$(PREFIX)/share/applications

all: $(SCRIPTS) elcppd

$(SCRIPTS): ${SCRIPTS:=.in}
	@echo '	GEN' $@
	@$(SED) -e 's|@libdir@|$(libdir)|g' \
	        -e 's|@localedir@|$(localedir)|g' \
	        -e "s|@ver@|`git rev-list --count HEAD`.`git rev-parse --short HEAD`|g" \
	        $@.in > $@

elcppd: elcppd.cpp
	g++ -std=gnu++11 -Wall $(CFLAGS) $(CPPFLAGS) elcppd.cpp -lboost_filesystem -lcurl -lboost_system -o elcppd $(CXXFLAGS) $(LDFLAGS)

install-scripts: $(SCRIPTS)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(applicationdir)
	$(INSTALL) -m755 earthlive-cpp $(DESTDIR)$(bindir)
	$(INSTALL) -m644 lib/io.lib.sh $(DESTDIR)$(libdir)
	$(INSTALL) -m644 lib/integration.lib.sh $(DESTDIR)$(libdir)
	$(INSTALL) -m644 conf/EarthLiveCPPStart.desktop $(DESTDIR)$(applicationdir)
	$(INSTALL) -m644 conf/EarthLiveCPPControl.desktop $(DESTDIR)$(applicationdir)
	test -x $(MSGFMT) && for file in po/*/*.po; \
	do \
	  package=$$(echo $$file | $(SED) -e 's#po/\([^/]\+\).*#\1#'); \
	  lang=$$(echo $$file | $(SED) -e 's#.*/\([^/]\+\).po#\1#'); \
	  $(INSTALL) -d $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES; \
	  $(MSGFMT) -o $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$$package.mo $$file; \
	done

install-binary: elcppd
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -m755 elcppd $(DESTDIR)$(bindir)

install: install-scripts install-binary

clean:
	rm -rf $(SCRIPTS) elcppd

uninstall:
	rm -f $(DESTDIR)$(bindir)/earthlive-cpp
	rm -f $(DESTDIR)$(bindir)/elcppd
	rm -rf $(DESTDIR)$(libdir)
	rm -rf $(DESTDIR)$(applicationdir)/EarthLiveCPPStart.desktop
	rm -rf $(DESTDIR)$(applicationdir)/EarthLiveCPPControl.desktop

.PHONY: all install install-binary uninstall
