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
  -e file.com	CP/M executable file name
  -f file.sym   Symbol-table for debugger or overlay creation
  -i directory  Include directory (in native format)
  -m file.map   Linker map file
  -cr file.crf  Cross-reference listing
```

* The linker (zxlink) should use the renamed HI-TECH linker
(now linq.com).

* Fix compilation warnings

* Emulation of CP/M BDOS function 60 (call resident system extension)
should be disabled and return 0xFF in both the A and L registers.

* Change cpm_bdos_10() to return an unsigned result to avoid buffer
size being interpreted as negative.


Tony Nicholson 20-Aug-2021
