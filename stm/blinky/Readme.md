This is a simple "blinky" example using the Standard Peripheral Library for
[STM32F4 Discovery board](http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419).
It is intended mainly for Linux users and accompanies
[this](http://liviube.wordpress.com/2013/04/22/blink-for-stm32f4-discovery-board-on-linux-with-makefile/)
article.


### Files ###

Here is an overview of the files found in this project:

* .gdbinit: contains startup commands for gdb, the debugger.
  If you don't want to use this file, you will have to enter the last two lines
  every time when you start gdb: `target extended localhost:4242` and `load`.
  This file is a copy of the
  [original by Ross Wolin](https://github.com/rowol/stm32_discovery_arm_gcc/blob/master/blinky/.gdbinit).
* Makefile: commented makefile used to compile the blinky project.
  You can use this file to see how the whole compilation process works and how
  to use the tools.
* main.c: this is the actual program (with comments).
* stm32\_flash.ld: linker script provided by ST. Contains a description of the
  memory layout used by the Cortex-M4.
* stm32f4xx\_conf.h: this file is used to include the header files of the
  standard peripheral library. It is a configuration file.
  You could include those headers directly in
  main.c, but the peripheral library includes this configuration header
  (stm32f4xx.h includes it conditionally),
  so we must provide it - or else we cannot compile our code against the library.
  This file is provided by ST and is supposed to be adapted to every project.
* system\_stm32f4xx.c: implements intialization code called from the startup file
  startup\_stm32f4xx.s (present in the library).
  This file is provided by ST.

#### Missing files ####

This project does not include the peripheral library used to build the
program. You can however download the library from ST (see link below).
In my setup I have extracted the library in a folder one level above the
"blinky" folder. That is I have the following directory structure:


    - stm +
          |- STM32F4-Discovery_FW_V1.1.0/
          |          +
          |          | - Libraries
          |          | - MCD-ST Liberty SW License Agreement 20Jul2011 v0.1.pdf
          |          | - Project
          |          | - Release_Notes.html
          |          | - Utilities
          |          | - _htmresc
          |
          |- blinky/ +
                     | - .gdbinit
                     | - Makefile
                     | - Readme.md
                     | - main.c
                     | - stm32_flash.ld
                     | - stm32f4xx_conf.h
                     | - system_stm32f4xx.c


### Tools and software requirements ###

* [GNU toolchain](https://launchpad.net/gcc-arm-embedded)
  for ARM Cortex-M.
  You need this in order to compile and debug your code.
  You can download pre-built binaries for all major operating systems here:

  [https://launchpad.net/gcc-arm-embedded](https://launchpad.net/gcc-arm-embedded).

  After downloading, just extract the archive somewhere, no installation is required.
  The exact location of the extracted folder will be used in the makefile.

* [stlink](https://github.com/texane/stlink) utility written by texane for Linux.
  You need this for flashing the compiled code to the board. It also contains a
  gdb server used to debug your code on the chip. You can download it here:

  [https://github.com/texane/stlink](https://github.com/texane/stlink)

  Installation and usage instructions are found in the downloaded package.

  **Archlinux** users:
  there is a [package](https://aur.archlinux.org/packages/stlink-git) in the AUR
  already, no need to compile.

  Windows users: You can use
  [this](http://www.st.com/web/catalog/tools/FM146/CL1984/SC724/SS1677/PF251168)
  software package to flash the board:

  [http://www.st.com/web/catalog/tools/FM146/CL1984/SC724/SS1677/PF251168](http://www.st.com/web/catalog/tools/FM146/CL1984/SC724/SS1677/PF251168)

  You will find download links on that page (scroll down to
  "Related Tools and Software").
  The software package can be used to flash the board and also contains a gdb
  server.


* The STM32F4DISCOVERY board firmware package. You need this library in order to
  access and use the board's peripherals. It contains headers and source files 
  which you compile and link together with your own files. Download page:

  [http://www.st.com/web/en/catalog/tools/PF257904](http://www.st.com/web/en/catalog/tools/PF257904)

  Actually you don't *really need* this library, but without it you must know 
  the exact memory mapping of all peripherals and the library's headers contain
  symbolic definitions for these memory addresses. It also contains useful
  functions which you don't really want to write on your own.


* make. You need this in order to... make this project.
  make should be pre-installed on all Linux machines. Windows users can
  use the [MinGW](http://www.mingw.org/) package, it contains make.

### Usage ###

1. Download and install the requirements mentioned above.
2. Download the files in this `blinky` folder. Eventually download the whole
   repository as a zip file. Try to have the same directory layout as shown
   above.
3. Edit the Makefile if needed:
   1. STM\_DIR stores the location of the standard library folder.
   2. TOOLS\_DIR stores the location of the toolchain. If your toolchain
      is already on the path, remove the $(TOOLS\_DIR)/ prefix from
      the variables CC, OBJCOPY and GDB.
4. Open a terminal window and change to the blinky directory.
5. Type `make`.
6. Connect the stm32f4 discovery board to your computer.
7. If you just want to flash the blinky program, type `make flash`.
8. If you want to debug the program:
   1. Start the gdb server included in the stlink package by typing
   `st-util`. You should start st-util in a separated terminal window so that it
   does not polute the gdb session.
   2. Type `make debug`. This will start gdb which will read the .gdbinit file
      and thus connect to localhost port 4242, where st-util is listening. From
      this point on, st-util will take commands from gdb and control the
      discovery board.
   3. In the gdb window, type `break main` to set up a break point to the main
   function.
   4. Type `continue`.
   5. Have fun.

#### Tips ####

You do not have to use the file .gdbinit and the command `make debug`. After
having compiled the program and having started st-util, you can
just type yourself the respective commands like this:

    arm-none-eabi-gdb blinky.elf
    target extended localhost:4242
    load
    break main
    continue

If you would like gdb to display a nice source window along with the command
window, you may want to start gdb with the option `-tui` like this:

    arm-none-eabi-gdb -tui blinky.elf

Eventually you edit the makefile and add this option like this:


    .PHONY: debug
    debug:
    	$(GDB) -tui $(PROJ_NAME).elf


The tui mode does not seem to work in MinGW, but lots of things do not work as
expected in Windows... (this is one of the reasons why I don't use it).


### Thanks ###

* Many thanks to [texane](https://github.com/texane) for writting the
  [stlink](https://github.com/texane/stlink) utility!

* Many thanks to [Ross Wolin](https://github.com/rowol) for writting
  [this article](http://www.wolinlabs.com/blog/linux.stm32.discovery.gcc.html)
  where he shows how to use a makefile to build an application on Linux against
  the standard peripheral library for the STM32F4 board.

  The makefile found in this project is based on Ross Wolin's
  [makefile](https://github.com/rowol/stm32_discovery_arm_gcc/blob/master/blinky/Makefile).
