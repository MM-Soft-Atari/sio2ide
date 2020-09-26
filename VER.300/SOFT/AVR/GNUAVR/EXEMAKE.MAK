#-------------------------------------------------------------------------------
#
#       Marek Mikolajewski (c) 2001
#
#-------------------------------------------------------------------------------
#       Make Binary Target
#
# --- create bin (.hex and .eep) files from elf output file ---
$(OUTF).hex: $(OUTF).$E
        %echo ELF to HEX (format $(FORMAT): $(OUTF).hex)...
        @$(BIN) -O $(FORMAT) -R .eeprom $< $@
        %echo Done
        @$(SIZE) $(OUTF).$E
        @echo "Errors: none"
        @move $(OUTF).hex hex\$(OUTF).hex

$(OUTF).eep: $(OUTF).$E
        %echo ELF to EEP ...
        @$(BIN) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# --- link: instructions to create elf output file from object files ---
$(OUTF).$E: $(OBJF)
        %echo Linking ...($(OUTF).$E)
        @$(CC) $(OBJF) $(LIBF) $(LDFLAGS) -o $(OUTF).$E

# --- assemble: instructions to create object file from assembler files ---
%.$O : %.$A
        %echo Assembling ($<)...
        @$(AS) -c $(ASFLAGS) -I$(INCPTH) $< -o $@

# --- compile: instructions to create assembler and/or object files from C source ---
%.$O : %.$C
        %echo Compiling ($<)...
        @$(CC) -c $(CPFLAGS) -I$(INCPTH) $< -o $@

%.$A : %.$C
        %echo Compiling ($<)...
        @$(CC) -S $(CPFLAGS) -I$(INCPTH) $< -o $@

# --- make instruction to delete created files ---
clean:
        %echo Cleaning...
        @$(RM) $(OBJF)
        @$(RM) $(FILES,>.$A)
        @$(RM) $(FILES,>.lst)
        @$(RM) $(OUTF).map
        @$(RM) $(OUTF).$E
        @$(RM) $(OUTF).hex
        @$(RM) $(OUTF).eep
        @$(RM) *.bak
        @$(RM) *.log

size:
        @$(SIZE) $(OUTF).$E

#-------------------------------------------------------------------------------
