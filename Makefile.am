ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

BUILT_SOURCES=src/git_version.h

bin_PROGRAMS=senmqd 

senmqd_SOURCES = \
		 ./src/Gpio.H \
		 ./src/log.cc \
		 ./src/log.h  \
		 ./src/timestamp.h\
		 ./src/main.cc

senmqd_CFLAGS = $(ZMQSTUFF_CFLAGS)
senmqd_CXXFLAGS = ${AM_CXXFLAGS} -Dunix --std=c++11 -ggdb -O0\
		  $(ZMQSTUFF_CXXFLAGS) -I./thirdparty/cppzmq/ 

senmqd_LDFLAGS = $(ZMQSTUFF_LIBS) -lpthread


#testmqd_SOURCES = \
		  #./src/testmqd.cc

#testmqd_CXXFLAGS = ${AM_CXXFLAGS} -Dunix --std=c++11 -ggdb -O0 \
		  #$(ZMQSTUFF_CXXFLAGS) -I./thirdparty/cppzmq/ 

#testmqd_LDFLAGS = $(ZMQSTUFF_LIBS) -lpthread



EXTRA_DIST=



src/git_version.h: $(HEADERS) $(SOURCES)
	echo "//Auto-generate information." > $@
	echo "#pragma once" >> $@
	echo "#define GIT_VERSION_DESC \"`git describe --abbrev=8 --dirty --always`\"" >> $@


dist-hook: ChangeLog

README: README.md
	ln -s

ChangeLog: configure.ac README.md
	git log --stat --name-only --date=short --abbrev-commit > $@

