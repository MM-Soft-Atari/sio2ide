#################################################################
# Copyright (C), 2002 MMSoft, All rights reserved
#################################################################

#################################################################
#
# SOURCE FILE:  Makefile
#
# PURPOSE:      Makefile for BC30 software
#       	This Makefile requires OPUS make
#
# AUTHOR:       Marek Mikolajewski (MM)
#
# REVIEWED BY:
#
# HISTORY:      Ver   Date       Sign   Description
#
#       	001   25-02-2002 MM     Created
#
#################################################################

%include ..\..\..\..\config.mak

CROOT   = $(BC30ROOT)
ROOT    = $(PROOT)\VER.400\SOFT

# Memory model
#
MM      = s

#
# Source files
#
MAIN    = fstest
CSRCS   = $(MAIN) idedos idecddos extmemf fatfs iso eeserv
ASRCS   =
LIBS    = emu maths c$(MM)


#
# Destination files
#
DEST    = checkfs

# Utilities
#
CC      = $(CROOT)\bin\bcc
CL      = $(CROOT)\bin\tlink
AS      = $(CROOT)\bin\tasm
LB      = $(CROOT)\bin\tlib
STRP    = $(CROOT)\bin\tdstrip

# Include paths, Libs and Defs
#
DEF    += DEBUG
INCPTH += $(CROOT)\include\\ $(ROOT)\avr\fat_1632
LIBPTH += $(CROOT)\lib\\

#
# Make process definitions
#

INCDIRS = $(INCPTH,<-I)
LIBDIRS = $(LIBPTH,</L)
DEFS    = $(DEF,<-D)
CRT0    = $(CROOT)\lib\c0$(MM).obj

SRCS    = $(ASRCS) $(CSRCS)
OBJF    = $(SRCS,>.obj)
LIBF    = $(LIBS,>.lib)

COPT    = -v -vi- -wpro -wdef -wuse -weas -wpre $(INCDIRS) $(DEFS)
AOPT    =
LOPT    = /x/c/P-

# Linking
#
$(DEST).exe:  $(OBJF) $(MAIN).obj
	@echo Linking    : $(DEST).exe
	@$(CL) $(LOPT) $(LIBDIRS) @<<
$(CRT0)+
$(OBJF,W+\n)
$(DEST)

$(LIBF,W+\n)
<<
	@echo Done (mem mode = $(MM,UC))
	@echo
	@$(STRP) $(DEST).exe
	@move $(DEST).exe bin\$(DEST).exe

# Compiling
#
.c.obj:
	@echo Compiling  : $<
	@$(CC) $(COPT) -c $<

# Assembling
#
.asm.obj:
	@echo Assembling : $<
	@$(AS) $(AOPT) $*.asm

.BEFORE:
	@copy $(ROOT)\avr\fat_1632\extmemf.c *.*
	@copy $(ROOT)\avr\fat_1632\fatfs.c *.*
	@copy $(ROOT)\avr\fat_1632\iso.c *.*

all:
	@make clean
	@make

clean:
	@del *.map
	@del *.obj
	@del *.exe
	@del *.lst
	@del extmemf.c
	@del fatfs.c
	@del iso.c

#       End
