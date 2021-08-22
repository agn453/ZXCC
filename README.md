# ZXCC

## Introduction

This is John Elliott's CP/M 2/3 emulator for cross-compiling and
running CP/M tools under MS-DOS and Linux/Unix/macOS.

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
emulator.  The original code was based on incorrect documentation
of the DAA instruction in the Zilog and Mostek programming manuals (and
even in Rodney Zaks "Programming the Z80" book).  The most recent
documentation from Zilog has the correct description(1).  With this
fix in-place the emulator is able to run the Z80 instruction emulator
test suite ZEXDOC from yaze-ag(2).

--

(1) The most recent Zilog Z80 Family CPU User Manual (UM008001-1000)
can be found at http://www.zilog.com/docs/z80/z80cpu_um.pdf

(2) You'll find the final version of yaze-ag 2.51.1 (as curated by
Andreas Gerlich) at http://www.mathematik.uni-ulm.de/users/ag/yaze-ag

--

Tony Nicholson 23-Aug-2021
