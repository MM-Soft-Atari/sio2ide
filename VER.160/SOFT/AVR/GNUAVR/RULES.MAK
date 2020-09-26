#-------------------------------------------------------------------------------
#
#       Marek Mikolajewski (c) 2001
#
#-------------------------------------------------------------------------------
#
CPTH    = $(GAVRROOT)

MCU     = $(AVR)

FORMAT  = ihex

DEFS    += #AVR_$(MCU)

#-------------------------------------------------------------------------------
#       Used Utility
#
CC      = $(CPTH)\bin\avr-gcc
AS      = $(CPTH)\bin\avr-gcc -x assembler-with-cpp
RM      = $(CPTH)\bin\rm -f
RN      = $(CPTH)\bin\mv
BIN     = +$(CPTH)\bin\avr-objcopy
SIZE    = +$(CPTH)\bin\avr-size

LIBPTH  = $(CPTH)\avr\lib
SHELL   = $(CPTH)\bin\sh.exe

INCPTH  = .
%foreach IPTH $(IPTLST,UC)
INCPTH  += $(ROOT)\$(IPTH)\\
%endfor

#-------------------------------------------------------------------------------
#       File Extensions
#
C       = c
A       = s
O       = o
E       = elf

CPFLAGS = -mmcu=$(MCU,LC) $(DEFS,<-D) -g -O3 -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)
ASFLAGS = -mmcu=$(MCU,LC) -gstabs
LDFLAGS = -mmcu=$(MCU,LC) -Wl,-Map=$(OUTF).map,--cref

#-------------------------------------------------------------------------------
