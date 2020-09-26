#-------------------------------------------------------------------------------
#
#       Marek Mikolajewski (c) 2001
#
#-------------------------------------------------------------------------------
#
CPTH    = $(CC90ROOT)

CSPY    = OFF

%if $(AVR)==AT90S8515
MMOD    = s     # memory model s/t
PVER    = 1     # procesor option 0/1/2/3
%endif
%if $(AVR)==ATmega161
MMOD    = s     # memory model s/t
PVER    = 3     # procesor option 0/1/2/3
%endif

DEFS    += __V$(PVER)__

#-------------------------------------------------------------------------------
#       Used Utility
#
CC      = $(CPTH)\BIN\ICCA90
AC      = $(CPTH)\BIN\AA90
LINK    = $(CPTH)\BIN\XLINK
LIBEXE  = $(CPTH)\BIN\XLIB
CONV    = $(CMPLDIR)\UTILS\HEXBIN2

LIBPTH  += $(CPTH)\LIB\\
INCPTH  += $(CPTH)\INC\\
%foreach IPTH $(IPTLST,UC)
INCPTH  += $(ROOT)\$(IPTH)\\
%endfor

#-------------------------------------------------------------------------------
#       File Extensions
#
C       = c
A       = s90
O       = r90
E       = hex
D       = d90
L       = l90
B       = bin

COPT    = -m$(MMOD) -v$(PVER) -K -e -L -q -r -z9 -y
AOPT    = -DMEMORY_MODEL=$(MMOD) -v$(PVER)
LOPT    =

#-------------------------------------------------------------------------------
