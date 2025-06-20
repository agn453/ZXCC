# ZXCC

## Introduction

This is John Elliott's CP/M 2/3 emulator for cross-compiling and
running CP/M tools under Microsoft Windows and Linux/Unix/macOS.
The ability to run under MS-DOS/DR-DOS is no longer supported.

This repository has been initially extracted from John Elliott's
Stable version 0.5.7 source-code at

http://www.seasip.info/Unix/Zxcc/zxcc-0.5.7.tar.gz

I have some updates that will be applied soon to remedy some
issues that I found when using the latest version of the
HI-TECH Z80 C compiler v3.09-x - obtainable from the repository
at

https://github.com/agn453/HI-TECH-Z80-C

I welcome bug reports/fixes and additional commentry and discussion.

You can raise an issue here on GitHub or contact me directly via e-mail
at <agn453@gmail.com>.


## Updates

* An apparent error in the source of the CP/M BDOS function 10 (read
console buffer) emulation - courtesy of @tsupplis from his repository
at https://github.com/tsupplis/zxccp

* The C compiler (zxc) front-end was not accepting HI-TECH C-style
command options that require filenames.  It now accepts the following

```
  -efile.com     CP/M executable file name
  -ffile.sym     Symbol-table for debugger or overlay creation
  -idirectory    Include directory (in native format)
  -mfile.map     Linker map file
  -crfile.crf    Cross-reference listing
```

* The linker (zxlink) should use the renamed HI-TECH linker
(now linq.com).

* Fix compilation warnings

* Emulation of CP/M BDOS function 60 (call resident system extension)
should be disabled and return 0xFF in both the A and L registers.

* Change cpm_bdos_10() to return an unsigned result to avoid buffer
size being interpreted as negative.

* Fix the emulation of Z80 opcodes for IN (HL),(C) and OUT (C),(HL) -
opcodes 0xED,0x70 and 0xED,0x71 respectively.  This
is noted in Fred Weigel's AM9511 arithmetic processing unit
emulation from https://github.com/ratboy666/am9511 in the howto.txt
description.  NB: I have not included Fred's am9511 support at
this time into ZXCC.

* Russell Marks contributed a fix to the emulation of the Z80 DAA
(decimal adjust) instruction - based on code from the yaze 1.10
emulator.  The original code in ZXCC was based on incorrect documentation
of the DAA instruction in the Zilog and Mostek programming manuals (and
even in Rodney Zaks "Programming the Z80" book).  The most recent
documentation from Zilog has the correct description[^1].  With this
fix in-place ZXCC is able to run the Z80 instruction emulator
test suite ZEXDOC from yaze-ag[^2].  You can download a copy of the
CP/M binary for ZEXDOC from
https://raw.githubusercontent.com/agn453/ZEXALL/main/zexdoc.com

* Mark Ogden (@ogdenpm) has supplied some modifications to allow
stdin redirection from a file, as well as a fix to the BDOS function 10
bdos_rdline() routine to process new-line '\n' and '\r\n' correctly
from file input.

* Further tweaks from Mark Ogden for Console Line input. A NULL character
can now be read, and long-lines are truncated (as they would be on a real
CP/M computer).  Control characters are also echoed using the ^ prefix.
For example, Ctrl-Z echoes as ^Z (and is read by the emulated program
as on real CP/M).

* More updates from Mark Ogden - this time to keep track of open
files. CP/M does not care if you don't close a file open for input -
whereas under ZXCC the operating system keeps a file handle for
each open file.  This can have an effect when files are deleted or
renamed.  There's a symbol FILETRACKER that's been added to the
configuration file ```winbuild/config-win.h``` for the Windows build.
Undefine this if you wish to restore the previous behaviour.

* Michal Tomek has added environment variables for BINDIR80,
INCDIR80 and LIBDIR80 to allow overriding the compiled-in defaults.
Also, the ```bios.bin``` file can be in the same directory as the
ZXCC executable or with the CP/M .COM files.

* More from Michal Tomek - Fix compilation under Cygwin on Windows and
correct BDOS function 40 (Write with Zero-fill).

* Mark Ogden has added the capability to build the ZXCC command-line
programs under Microsoft Windows (using Visual Studio).  You'll find
build instructions in the
[winbuild/README.md](https://raw.githubusercontent.com/agn453/ZXCC/main/winbuild/README.md)
file.  Further details concerning the changes are in
[changes.md](https://raw.githubusercontent.com/agn453/ZXCC/main/changes.md).

* A fix was made so that any reference to the P: drive is now redirected
to the local default directory.  Also, filenames under Windows can use either
a forward slash ```/``` or back-slash ```\``` in the directory path.

* Mark Ogden has cleaned up a number of warnings for the Windows build
using Visual Studio and fixed some issues with the install script when
it encountered spaces in a Windows directory pathname and the CPMDIR80
environment variable determining the search paths for the CP/M binaries,
libraries and include files.

* Modified the compiler front-end (zxc) to support environment variables
for INCDIR80 or CPMDIR80 and more changes to build with Cygwin on Windows
from Mark Ogden.

* Finally fixed by Mark Ogden (much appreciated) is an issue that affected
the processing of the ```$$exec.$$$``` file by HI-TECH C's batch processing.
This changes the way ZXCC implements "exact file size" handling to match that
of Jon Saxton's and HI-TECH C (see the
[README.md](https://raw.githubusercontent.com/agn453/HI-TECH-Z80-C/master/README.md)
file for details).  Also, user area specifications on CP/M filenames have
been removed - since they're not implemented by ZXCC.

* Fix parsing of spaces in the FCBs - resolving issue #39.

* Look for CP/M COM file in current directory (emulated drive P:) before
the binaries (in BINDIR80 emulated as drive A:).

* The search for ```bios.bin``` on macOS should not use Linux-style
/proc path.

* Add some hints on where to find some extra files.  In addition to the
previous locations for ```.COM``` binaries, ```.LIB``` libraries and
object files ```.OBJ``` (in the bin80 and lib80 locations), ZXCC now
searches for ```.HLP```, ```.MSG``` and ```.OVR``` files in bin80, ```.REL```
in lib80 and ```.H``` header files in include80.  The Microsoft BASIC-80
compiler run-time ```BCLOAD.``` can be in the lib80, and HI-TECH C options
help ```OPTIONS``` in the bin80 locations.

* The exact file size changes have been reverted to behave like the
original John Elliott version - and now use the DOS Plus convention.
The HI-TECH Z80 C behaviour has been altered too as at v3.09-13 to
support this.  Please see the HI TECH Z80 C repository at
https://github.com/agn453/HI-TECH-Z80-C for details and to update
to the latest version.

* Jim Burlingame provided updates to the autotools build system to
allow building under recent versions of Linux/Unix/macOS using
the ```./configure ; make ; make install``` method.  This has been
tested and verified to work with Debian Bookworm (and variants)
and macOS Sonoma (x86 and arm).

* Further updates to the autotools build system to permit building
on systems with updated versions of the autotools scripts.  I've
included the script file mentioned by Martin in the comments for issue #47
to accomplish this if needed.  You'll need to have the ```automake``` package
(and prerequisites) installed to run the ```./update-configure.sh```
script.  On Linux use ```sudo apt install automake``` or similar, and
under macOS with brew use ```brew install automake``` to install them.

* Martin contributed an update to add support to run the ```OBJTOHEX.COM```
tool with a new ```zxobjtohex``` wrapper.

* I added some extra hints to support the Digital Research PL/I-80 V1.3
compiler (```*.OVL``` overlays in the BINDIR80 folder and the PL/I
library ```PLILIB.IRL``` in the LIBDIR80 folder).  Also fixed some missing
prototypes to silence compiler warnings under macOS (using clang).

* Only the Windows build was using the FILETRACKER option to keep track
of the number of CP/M open files.  To enable it under Linux/Unix/macOS
you need to manually enable it when running ```./configure``` using
the ```--with-filetracker``` option.

* To download, build and install this repository under Linux/Unix/macOS
proceed as follows -

```
cd src
git clone https://github.com/agn453/ZXCC.git
cd ZXCC
./update-configure.sh
./configure --prefix=$HOME --with-filetracker
make
make install
```

* This will put the executable programs in your local ```~/bin``` directory;
the documentation into ```~/share/zxcc/zxcc.doc```; complied object
libraries for ```libcpmio.a``` and ```libcpmredir.a``` into
```~/lib```; and create the default CP/M heirarchy under ```~/lib/cpm/bin80```
(where you place CP/M .COM files etc), ```~/lib/cpm/lib80``` (for your CP/M
libraries) and ```~/lib/cpm/include80``` (for your CP/M programming include
files like the system headers for HI-TECH-C).  If you'd rather the files were
installed in the ```/usr/local``` heirarchy, then omit the ```--prefix=$HOME```
from the above ```./configure``` command.

* I've refreshed the Windows Build files in ```winbuild``` and included
the missing project build files for the ```zxobjtohex.exe``` target.
In addition, I backported the Visual Studio ```winbuild/wzxcc.sln``` solution
file to support older versions back to at least Visual Studio 2015 which
can produce Windows binaries (both x86 and x86_64).

* Fixed Linux/Unix/macOS build autotools configuration when both
FILETRACKER and debugging is enabled.


[^1]: The most recent Zilog Z80 Family CPU User Manual (UM008001-1000)
can be found at http://www.zilog.com/docs/z80/z80cpu_um.pdf

[^2]: You'll find the final version of yaze-ag 2.51.3 (as curated by
Andreas Gerlich) at https://agl.yaze-ag.de/

--

Tony Nicholson, Friday 20-Jun-2025
