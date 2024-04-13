#!/bin/sh

WARNINGS=0
COUNT_WARNINGS=0

on() { ! [ -v $1 ]; }

# Default values
WERROR_ENABLED=1
AUTOADD_SDL=1
DRY_RUN=0
if on $CFLAGS; then
	WERROR_ENABLED=0
else
	CFLAGS="-O2 -Wall -Wextra"
fi

on $BINDIRS && BINDIRS="$BINDIRS "
BINDIRS="$BINDIRS/usr/local/bin /usr/bin /bin"

on $CC || CC=gcc
on $CXX || CXX=g++
on $CCLD || CCLD=ld

### OPTIONAL DEPS TRACKING
### END OPTIONAL DEPS TRACKING

print_usage() {
	echo "Usage: $0 [-n|--dry] [-h|--help] [--disable-werror] [--disable-sdl] [--clean]"
	# echo "Parameters needed for long options are needed for short options too."
	echo ""
	echo "Usable flags:"
	echo "-h --help        : Show this list"
	echo ""
	echo "--disable-werror : Removes the -Werror from the compiler flags. Implicit with a"
	echo "                   custom CFLAGS variable."
	echo ""
	echo "--disable-sdl    : Using this disables the sdl2-config check and automatic"
	echo "                   addition of SDL's cflags and libs. Set custom cflags and"
	echo "                   libs with the CFLAGS and LIBS enviornment variables if you"
	echo "                   use this flag."
	echo ""
	echo "--clean          : Runs \`make clean\`, and then removes all makefiles. A total"
	echo "                   clean of the source tree."
	echo ""
	echo ""
	echo "Usable enviornment variables:"
	echo "CXX    : C++ compiler. Defaults to g++"
	echo ""
	echo "CC     : C compiler. Defaults to gcc"
	echo ""
	echo "CCLD   : Linker. Defaults to ld"
	echo ""
	echo "CFLAGS : Custom cflags. Implicitly concatenated with SDL cflags unless"
	echo "         --disable-sdl is used."
	echo ""
	echo "LIBS   : Custom libss. Implicitly concatenated with SDL libs unless"
	echo "         --disable-sdl is used."
	#    "--------------------------------------------------------------------------------"
}

availible() { command -v $1 1> /dev/null; }
exists() { stat $1 1> /dev/null 2> /dev/null; }
pass() { echo -e $2 "\e[32m[PASS!]\e[0m $1" 1>&2; }
notice() { echo -e $2 "\e[34m[INFO ]\e[0m $1" 1>&2; }
warning() { echo -e $2 "\e[33m[ WARN]\e[0m $1" 1>&2; [ $COUNT_WARNINGS -eq 1 ] && WARNINGS=`expr $WARNINGS + 1`;}
error() { echo -e $2 "\e[31m[ERROR]\e[0m $1" 1>&2; exit 1; }

while [ $# -gt 0 ]; do
	case $1 in
		--clean)
			make clean
			rm -v ./Makefile
			exit 0
			;;
		--disable-werror)
			WERROR_ENABLED=0
			notice "Werror disabled"
			;;
		--disable-sdl)
			AUTOADD_SDL=0
			on $LIBS || warning "--disable-sdl used but \$LIBS was not set (linking may fail)"
			;;
		-h|--help)
			print_usage
			exit 0
			;;
		-n|--dry)
			DRY_RUN=1
			;;
		*)
			print_usage
			error "Unknown option $1"
			exit 1
			;;
	esac
	shift
done

if [ $WERROR_ENABLED -eq 1 ]; then
	CFLAGS="$CFLAGS -Werror"
fi

checkwarnings() {
	if availible expr; then
		if [ $WARNINGS -ne 0 ] ; then
			warning "$WARNINGS warning(s)";
			WARNINGS=`expr $WARNINGS - 1`
		else
			notice "No warnings"
		fi
	fi
}

require() {
	if availible $1; then
		pass "$1 found"
	else
		error "You do not have $1"
	fi
}

want() {
	notice "Looking for $1..."
	if availible $1; then
		pass "$1 found"
		return 0
	else
		warning "You do not have $1"
		return 1
	fi
}

prefer() {
	notice "Looking for $1..."
	if availible $1; then
		notice "$1 found"
		return 0
	else
		warning "You do not have $1"
		return 1
	fi
}

lookfor() {
	notice "Looking for $1..."
	if availible $1; then
		pass "$1 found"
		return 0
	else
		notice "You do not have $1"
		return 1
	fi
}

command echo > /dev/null || error "You do not have \`command\`, which is necessary for depchecking"

trap checkwarnings EXIT

if ! availible "["; then
	error "You do not have ["
fi

if ! availible stat; then
	error "You do not have stat"
fi

require		$CXX
require 	$CC
want 		expr 		&& COUNT_WARNINGS=1
prefer 		$CCLD

if [ $AUTOADD_SDL -eq 1 ]; then
	require 	sdl2-config
	CFLAGS="$CFLAGS $(sdl2-config --cflags)"
	LIBS="$LIBS $(sdl2-config --libs)"
fi

notice "Final cflags are $CFLAGS"
notice "Final libs are $LIBS"

[ $DRY_RUN -eq 1 ] && exit 0;

checkwarnings
notice "Generating Makefile"

cat > Makefile << EOS
CC=$CC
CXX=$CXX
CFLAGS=$CFLAGS
LIBS=$LIBS
CCLD=$CCLD
CLEAN=rm -fv

all: bytebeatPlayground

clean:
	\$(CLEAN) ./font.ipp
	\$(CLEAN) ./bytebeatPlayground
	\$(CLEAN) ./fonttest

bytebeatPlayground: main.cpp font.ipp
	\$(CXX) \$(CFLAGS) \$(LIBS) -o \$@ \$<

font.ipp: font.pl
	perl \$< > \$@

%.out: %.o
	\$(CC) \$(LIBS) -o \$@ \$<

%.out: %.oxx
	\$(CXX) \$(LIBS) -o \$@ \$<

%.o: %.c
	\$(CC) \$(CFLAGS) -c -o \$@ \$<

%.oxx: %.cxx
	\$(CXX) \$(CFLAGS) -c -o \$@ \$<
EOS
notice "Done generating Makefile"
