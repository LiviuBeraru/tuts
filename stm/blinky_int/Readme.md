This is a simple "delay blinky" example for the
[STM32F4 Discovery board](http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419)
which extends
[this example](https://github.com/Malkavian/tuts/tree/master/stm/blinky)
by using a delay function and a timer (TIM7).

The delay functionality is implemented using a timer and its corresponding 
interrupt. The interrupt handler `TIM7_IRQHandler` runs every millisecond and 
all it does is to decrement a global counter. The `delay` function sets the 
counter to the desired value and waits until it goes down to zero.

In order to compile and flash the code to the discovery board, you can use the 
supplied Makefile. Type `make` to build the example. Type `make clean` to remove 
all object files. After starting `st-util` in a separate terminal, you can type 
`make debug` to start a debugging session. gdb will read and execute the 
included file `.gdbinit`. If you just want to flash the code, type `make flash`.

As in the 
[previous example](https://github.com/Malkavian/tuts/tree/master/stm/blinky), 
the standard peripherals library is not included because it can be downloaded 
from ST. The makefile supposes the library is located one level above the 
directory containing this code.

This example was tested on Archlinux only but should run on any Linux
distribution.
