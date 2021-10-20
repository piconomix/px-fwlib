# Revision of library
LIB_REVISION = $(shell cat ../VERSION.txt)

# SVN Revision of repository
SVN_REVISION = $(shell svnversion -n)

all: html

# Create HTML documentation
html: clean
	mkdir -p html
	cp -R images/html/* html
	( cat Doxyfile ; echo "PROJECT_NUMBER = \"$(LIB_REVISION)\"" ) | doxygen -
#	doxygen Doxyfile

log: 
	svn log .. > ../CHANGELOG.txt

clean: 
	rm -rf html

.PHONY : html log clean
