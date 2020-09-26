#-------------------------------------------------------------------------------
#
#       Marek Mikolajewski (c) 2001
#
#-------------------------------------------------------------------------------
#       Make Binary Target
#
$(OUTF).$E: $(OBJF) $(LIBF)
            %echo Linking ...
            @$(LINK) -f <<
-z
-l $(OUTF,UC).map
-f $(ROOT)\$(CMPLR)\LNK$(PVER)$(MMOD)$(TYPE).XCL
$(FILES,>.$O,<-A ,W\n)
%if "$(LIBF)" != ""
-C $(LIBF,W\n)
%endif
-C $(LIBPTH)cl$(PVER)$(MMOD).$O
-xsm
-w14
-w18
-w29
%if $(CSPY)==ON
-o $(OUTF,UC).$D
-r
%else
-o $(OUTF,UC).$E
-FINTEL-EXTENDED
%endif
<<
  %echo ************************************
  %echo * Done ...
%if $(CSPY)==ON
  %echo * $(OUTF).$D  created (C-SPY debug mode)
%else
  %echo * $(OUTF).$E  created (RELEASE mode)
%endif
  %echo ************************************
  %echo * Compiler      $(CMPLR) $(MMOD) $(PVER)
  %echo * AVR           $(AVR)
  %echo * USB           $(USPI)
  %echo * Debug         $(DBG)
  %echo ************************************
  %echo
  @copy $(OUTF).$E hex\$(OUTF).$E

#
#       Compile Sources
#
.$C.$O:
        %echo $*.$C
        @$(CC) -f <<
$*.$C
$(COPT)
$(DEFS,<-D,W\n)
$(INCPTH,<-I,W\n)
<<

.$A.$O:
        %echo $*.$A
        @$(AC) -f <<
$*.$A
$(AOPT)
$(INCPTH,<-I,W\n)
<<

#-------------------------------------------------------------------------------
#       Clean All Targets
#
clean:
        @del *.$O >NUL
        @del *.$E >NUL
        @del *.$B >NUL
        @del *.$D >NUL
        @del *.map >NUL
        @del *.lst >NUL
        @del *.bak >NUL
        @del *.rep >NUL

#-------------------------------------------------------------------------------
