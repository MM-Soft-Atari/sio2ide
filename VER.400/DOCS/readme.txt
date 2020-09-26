

                          MMSoft (c) 2005

                SIO2IDE HDD interface for 8-bit ATARI
                      Version 4.$ (18.03.2005)


In this version:

     - Hardware:
        - ATMEL ATmega32-16PI (ATmega323-8PI) micro @ 7.3728MHz
        - National USBN9603 USB controller
        - PCB 4.0
     - Software:
        - Three operating modes: SIO2IDE, IDE2USB and TEST (MODE_ZW jumper)
        - Added support for CF cards
        - IDE2USB mode (MODE_ZW-on):
          - USB port driver and USB Mass Storage Class software
            Atari HDD is visible as removable drive under W98, W2k, Wme, Wxp
          - Drivers for W98 are included in this package
        - SIO2IDE mode (MODE_ZW-off):
          - All 3.2e features +:
            - Emulates Atari disks D1: to D8:
            - Supports direct access to HDD via SIO commands ($61, $62, $63)
              (see Tech_Manual_ver4.txt)
            - Added Common Disk D1: (D9:) that can be swapped with Disk D1: (HD1_ZW)
              (see Tech_Manual_ver4.txt)
            - Added Long File Names support
            - Added SIO command $65 to change current IDE Drive (Master/Slave)
              (see Tech_Manual_ver4.txt)
            - ATR files do not need to be defragmented
        - TEST mode (MODE_ZW-on&off in 3sec after start)
          - This mode alows to test drive initialisation process. Debug text
            information is sent on SIO Tx line. To view debug output simply
            connect SIO2IDE to the PC (use signal converter) and start any
            terminal software (19200,8n1).
        - FDISK for Atari:
           - Added support for new commands and features
           - Versions for: DOS, XEGS32k and DB32k Cart
           - Added Long File Names support
        - Atmel AVR programming software:
           - New Atmel AVR programming software for Windows (ver 3.0)
             Gives full access to Flash, EEPROM memory and FUSEs.
        - MakeATR utility:
           - Produces correct ATR files up to 16MB
             (accepted by all Atari emulators etc)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                      AVR device programming note:

        Please use attached Windows software and set the following
        FUSE in the ATmega32 AVR device.

                                LFB = 0xFF
                                HFB = 0xDF

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

e-mail: marek_mikolajewski@wp.pl


