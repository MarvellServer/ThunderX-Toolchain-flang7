## Cavium port of flang7.
## Includes LLVM, flang, clang, Polly, OpenMP, plus a (large) number of
Cavium internal patches.
========================================================================

## Building Flang:

1. Create a directory where you will clone this Github repo:

```
%> cd /mnt/data/builds
%> mkdir flang-cavium
```

2. Clone this repo:

```
%> cd flang-cavium
%> git clone https://github.com/flang-cavium/flang7.git
```

3. Copy the build Makefile:

```
%> cp flang7/Makefile.build .
```

4. You will now need to edit the build Makefile to match your build
environment. There are a few variables in the build Makefile that
need to be edited to match your build environment:

```
# Edit the following 4 paths to match your installation:
CC = /path/to/your/local/gcc/bin/gcc
CXX = /path/to/your/local/gcc/bin/g++
FC = /path/to/your/local/flang/bin/flang
GCC_RUNPATH = /path/to/your/local/gcc/lib64
```

```
# Edit the following path to match your desired setup:
CMAKE_PREFIX = /path/to/your/desired/flang7/install
```

```
# Edit this path to match your build environment setup.
LIBFFI_INCDIR = /path/to/your/libffi/header/files
```

5. That's it. There are several packages that you must have installed on
your build system:

```
- a recent version of CMake
- libxml2
- libffi
- a recent version of GCC (GCC >= 7.3 preferred)
- a recent version of GNU Binutils (Binutils >= 2.30 preferred)
```

6. You are now ready to configure and build Flang 7:

```
%> gmake -f Makefile.build configure
%> cd build
%> gmake -j16 >& make.out # T-shell syntax
```
7. This version of Flang 7 supports math function vectorization via
the SLEEF libraries:

```
https://sleef.org/
```

To build the SLEEF libraries, please follow the instructions at the SLEEF
web site.

The compile-line option to enable SLEEF in clang is:

```
-fveclib=SLEEF
```

To link with the SLEEF library, pass:

```
-lsleefgnuabi
```

on link-line.


