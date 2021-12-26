# Recent changes

These notes record some of the recent changes to ZXCC

26-Dec-2021

ZXCC now works under Windows (32 & 64bit). There is a Visual Studio solution file under winbuild.

Key changes & bug fixes

1) For Windows, some operations e.g. delete file and rename file can fail if a file is still open. To avoid this a level of open file tracking is now done, which can be used to force close open files, prior to delete, rename or  truncation. It is optional for non Windows builds, however it does reduce the number of open file handles.
2) truncate and fcb_sdate  are now supported
3) basic file attributes are supported i.e. hidden, system and archive
4) fcb_randwr - bug fix to make sure that the file length calculation takes into account the sector(s) just written
5) fcb_randwz - to implement this correctly requires tracking of sparse files. The previous solutions padded a file with zeros up to the new write point, if required. As this is effectively the default behaviour for lseek anyway, the function now just calls fcb_randwr.
6) fcb_randrd - now pads last partial sector to 128 bytes if eof if reached on multi sector read
7) bdos_rdline - modified to support redirect of stdin from a file and to handle \r\n correctly under unix variants
8) A number of conditional compilation checks have been simplified, including where specific Microsoft library capability is used, which is handled differently by CYGWIN etc.
9) The ability to modify the search path, using environmental variables, has been extended to allow for a single environment variable to be used as a prefix for the bin80, lib80 and include80 directories.

Mark Ogden