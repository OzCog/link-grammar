#!/bin/sh
#
# Run this before configure
#

rm -f autogen.err

# For version x.y>1.4, x should not be 0, and y should be [4-9] or more than one digit.
automake --version >/dev/null &&
   automake --version | test "`sed -En '/^automake \(GNU automake\) [^0]\.([4-9]|[1-9][0-9])/p'`"
if [ $? -ne 0 ]; then
    echo "Error: you need automake 1.4 or later.  Please upgrade."
    exit 1
fi

# Update the m4 macros
autoreconf -fvi

case `uname` in
    Darwin)
        [ "$LIBTOOLIZE" = "" ] && LIBTOOLIZE=glibtoolize
        ;;
esac

    ${LIBTOOLIZE:=libtoolize} --force --copy || {
    echo "error: libtoolize failed"
    exit 1
}

# Produce all the `GNUmakefile.in's and create neat missing things
# like `install-sh', etc.
#
echo "automake --add-missing --copy --foreign"

automake --add-missing --copy --foreign 2>> autogen.err || {
    echo ""
    echo "* * * warning: possible errors while running automake - check autogen.err"
    echo ""
}

# If there's a config.cache file, we may need to delete it.
# If we have an existing configure script, save a copy for comparison.
if [ -f config.cache ] && [ -f configure ]; then
  cp configure configure.$$.tmp
fi

# Produce ./configure
#
echo "Creating configure..."

autoconf 2>> autogen.err || {
    echo ""
    echo "* * * warning: possible errors while running automake - check autogen.err"
    echo ""
    grep 'Undefined AX_ macro' autogen.err && exit 1
}

run_configure=true
for arg in $*; do
    case $arg in
        --no-configure)
            run_configure=false
            ;;
        *)
            ;;
    esac
done

if $run_configure; then
    mkdir -p build
    cd build
    ../configure --enable-maintainer-mode "$@"
    status=$?
    if [ $status -eq 0 ]; then
      echo
      echo "Now type 'make' to compile link-grammar (in the 'build' directory)."
    else
      echo
      echo "\"configure\" returned a bad status ($status)."
    fi
else
    echo
    echo "Now run 'configure' and 'make' to compile link-grammar."
fi
