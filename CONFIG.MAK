#################################################################
# Copyright (C), 2002 MMSoft, All rights reserved
#################################################################

#################################################################
#
# SOURCE FILE: CONFIG.MAK
#
# PURPOSE:      Root configuration file for make process.
#       	Please edit this file if needed.
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

#
#       System paths (set your own paths if needed)
#

PROOT    = C:\Work\Projects\SIO2IDE\     # Project package root directory
CMPLDIR  = C:\Work\CMPLRS       	     # Compiler(s) directory
OPTH     = C:\Work\CMPLRS\OPUS\DOS       # Opus Make directory

#
#       Compilers (set your own paths if needed)
#

CC65ROOT = $(CMPLDIR)\cc65      	 # GNU CC65
CC90ROOT = $(CMPLDIR)\a90       	 # IAR AVR
GAVRROOT = $(CMPLDIR)\avrgcc    	 # GNU AVR
BC30ROOT = $(CMPLDIR)\borlandc  	 # BC 3.0

#
#       Utilities (set your own path if needed)
#

MAKE     = $(OPTH)\make 	        # Opus Make

#-------------------------------------------------------------------------------
