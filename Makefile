BASEDIR = .
include Makefile.in

ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
MACBUNDLE = bundle
endif

all: legesmotus $(MACBUNDLE)

legesmotus: common server client

common:
	$(MAKE) -C common

server: common
	$(MAKE) -C server

client: common
	$(MAKE) -C client

metaserver: common
	$(MAKE) -C metaserver

tests: common server client
	$(MAKE) -C tests

clean:
	$(MAKE) -C common clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean
	$(MAKE) -C tests clean
	rm -rf "Leges Motus.app"


ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
Leges\ Motus.app: client server
	mkdir -p "Leges Motus.app/Contents/MacOS"
	mkdir -p "Leges Motus.app/Contents/Resources"
	mkdir -p "Leges Motus.app/Contents/Frameworks"
	cp -f client/legesmotus "Leges Motus.app/Contents/MacOS/"
	cp -f server/lmserver "Leges Motus.app/Contents/MacOS"
	cp -f client/Info.plist "Leges Motus.app/Contents/"
	cp -Rf client/legesmotus.nib "Leges Motus.app/Contents/Resources/"
	cp -Rf data "Leges Motus.app/Contents/Resources/"
	test -d "Leges Motus.app/Contents/Frameworks/SDL.framework" || cp -Rf /Library/Frameworks/SDL.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_net.framework" || cp -Rf /Library/Frameworks/SDL_net.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_image.framework" || cp -Rf /Library/Frameworks/SDL_image.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_ttf.framework" || cp -Rf /Library/Frameworks/SDL_ttf.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_mixer.framework" || cp -Rf /Library/Frameworks/SDL_mixer.framework "Leges Motus.app/Contents/Frameworks"

legesmotus$(VSHORT).pkg: client server
	mkdir -p tmp/Leges\ Motus/Applications
	mkdir -p tmp/Leges\ Motus/usr/bin
	mkdir -p tmp/Leges\ Motus/usr/share/man/man6
	mkdir -p tmp/Resources/en.lproj
	cp -Rf Leges\ Motus.app tmp/Leges\ Motus/Applications/
	cp -Rf man/man6/* tmp/Leges\ Motus/usr/share/man/man6/
	cd tmp/Leges\ Motus/usr/bin && \
		ln -sf ../../Applications/Leges\ Motus.app/Contents/MacOS/lmserver && \
		ln -sf ../../Applications/Leges\ Motus.app/Contents/MacOS/legesmotus
	cp -f README tmp/Resources/en.lproj/ReadMe.txt
	cp -f COPYING tmp/Resources/en.lproj/License.txt
	/Developer/usr/bin/packagemaker --root tmp/Leges\ Motus --id org.legesmotus.legesmotus \
		--title "Leges Motus $(VLONG)" --version "$(VLONG)" --resources tmp/Resources \
		--out legesmotus$(VSHORT).pkg
	rm -Rf tmp

bundle: Leges\ Motus.app

package: legesmotus$(VSHORT).pkg

install:
	cp -Rf Leges\ Motus.app /Applications/
	ln -s /Applications/Leges\ Motus.app/Contents/MacOS/lmserver $(BINDIR)
	install -d $(MANDIR)/man6
	install $(BASEDIR)/man/man6/lmserver.6 $(MANDIR)/man6

uninstall:
	rm -rf /Applications/Leges\ Motus.app
	rm -f $(BINDIR)/lmserver
	rm -f $(MANDIR)/man6/lmserver.6
else

ifneq ($(PREFIX),)
install:
	install -d $(DATADIR)/fonts
	install -m 0644 $(BASEDIR)/data/fonts/* $(DATADIR)/fonts
	install -d $(DATADIR)/maps
	install -m 0644 $(BASEDIR)/data/maps/* $(DATADIR)/maps
	install -d $(DATADIR)/sounds
	install -m 0644 $(BASEDIR)/data/sounds/* $(DATADIR)/sounds
	install -d $(DATADIR)/sprites
	install -m 0644 $(BASEDIR)/data/sprites/* $(DATADIR)/sprites
	install -d $(MANDIR)/man6
	install $(BASEDIR)/man/man6/* $(MANDIR)/man6
	install -d $(BINDIR)
	install -s $(BASEDIR)/server/lmserver $(BASEDIR)/client/legesmotus $(BINDIR)

uninstall:
	rm -rf $(DATADIR)
	rm -f $(BINDIR)/lmserver $(BINDIR)/legesmotus
	rm -f $(MANDIR)/man6/lmserver.6 $(MANDIR)/man6/legesmotus.6
endif

endif

.PHONY: clean common server client bundle metaserver install uninstall
