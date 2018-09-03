# Revision of library
LIB_REVISION = $(shell cat ../VERSION.txt)

# SVN Revision of repository
SVN_REVISION = $(shell svnversion -n)

all: html

# Create HTML documentation
html:
	-mkdir html
	cp -R images/* html
	( cat Doxyfile ; echo "PROJECT_NUMBER = \"$(LIB_REVISION) SVN $(SVN_REVISION)\"" ) | doxygen -
#	doxygen Doxyfile

log: 
	svn log .. > ../CHANGELOG.txt

clean: 
	rm -rf html

.PHONY : html log clean
