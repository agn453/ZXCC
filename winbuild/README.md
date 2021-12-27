# ZXCC build for Windows

The core zxcc files have been modified to build under windows  using visual studio. It may build using different windows build tools but this hasn't been tested.

This folder includes a Visual Studio solution file to build **zxcc** windows, supporting x86,  x64 debug and release configurations.
Intermediate files are built in a **Build** folder, under which there is a folder for each sub project, each containing sub folders for the different configurations e.g. debug/release x86/x64.

Final files are created in an **Install** folder, with sub folders for the different configurations.

In addition, the solution file can optionally install files in any desired location. The configuration script install.cfg should be modified to reflect your own requirements, or removed if you don't want to auto install.

It also includes precompiled dll files for PDCurses along with the required curses.h file in the **external** folder. Due to searching for pdcurses.dll, it is not recommended to install different configurations of the built files in the same directory.

Note I personally use Visual Studio 2022, however if you have earlier or later versions you should be able to change the build tools in the solution file to match your own setup.

Mark Ogden

26-Dec-2021

mark.pm.ogden@btinternet.com