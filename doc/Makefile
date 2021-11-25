# Revision of library
LIB_REVISION = $(shell cat ../VERSION.txt)

all: html

# Create HTML documentation
html: clean
	mkdir -p html
	cp -R images/html/* html
	( cat Doxyfile ; echo "PROJECT_NUMBER = \"$(LIB_REVISION)\"" ) | doxygen -
#	doxygen Doxyfile

# Remove generated HTML documentation
clean:
	rm -rf html

.PHONY : html log clean
