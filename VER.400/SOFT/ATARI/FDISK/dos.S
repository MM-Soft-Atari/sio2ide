;;****************************************************************
;; Copyright (C), 2002 MMSoft, All rights reserved
;;****************************************************************

;;****************************************************************
;;
;; SOURCE FILE: DOS.S
;;
;; MODULE NAME: DOS
;;
;; PURPOSE:     Startup code for cc65 (ATARI DOS version)
;;
;; AUTHOR:      Marek Mikolajewski (MM)
;;
;; REVIEWED BY:
;;
;; HISTORY:     Ver   Date       Sign   Description
;;
;;              001    3-09-2002 MM     Created
;;
;;****************************************************************
;
; This must be the *first* file on the linker command line
;

        .export         _exit
        .constructor    initsp,26

        .import         getargs, argc, argv
        .import         initlib, donelib
        .import         zerobss, pushax
        .import         _main,__filetab,getfd
        .import         _memcpy, incaxy, _clrscr

        .import         __CODE_LOAD__, __BSS_LOAD__
        .import         __DLIST_LOAD__, __DLIST_RUN__, __DLIST_SIZE__
        .import         __SCREEN_RUN__

        .include        "atari.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the runtime

        .exportzp       sp, sreg, regsave
        .exportzp       ptr1, ptr2, ptr3, ptr4
        .exportzp       tmp1, tmp2, tmp3, tmp4
        .exportzp       fntemp, regbank, zpspace

.macro  ldax    Value
        lda     #<(Value)
        ldx     #>(Value)
.endmacro

.zeropage

zpstart = *
sp:             .res    2       ; Stack pointer
sreg:           .res    2       ; Secondary register/high 16 bit for longs
regsave:        .res    2       ; slot to save/restore (E)AX into
ptr1:           .res    2
ptr2:           .res    2
ptr3:           .res    2
ptr4:           .res    2
tmp1:           .res    1
tmp2:           .res    1
tmp3:           .res    1
tmp4:           .res    1
fntemp:         .res    2       ; Pointer to file name
regbank:        .res    6       ; 6 byte register bank

zpspace = * - zpstart           ; Zero page space allocated

        .segment "CODE0"

        .segment "CODE1"

        .segment "CODE2"

        .code

; ------------------------------------------------------------------------
; EXE header

        .segment "EXEHDR"
        .word   $FFFF
        .word   __CODE_LOAD__
        .word   __BSS_LOAD__ - 1

        .code
        .reloc

; ------------------------------------------------------------------------
; Actual code

        rts     ; fix for SpartaDOS / OS/A+
                ; they first call the entry point from AUTOSTRT and
                ; then the load addess (this rts here).
                ; We point AUTOSTRT directly after the rts.

; Real entry point:

; Save the zero page locations we need

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Clear the BSS data

        jsr     zerobss

; setup the stack

        tsx
        stx     spsave

; report memory usage

        lda     APPMHI
        sta     appmsav                 ; remember old APPMHI value
        lda     APPMHI+1
        sta     appmsav+1

        lda     MEMTOP
        sta     APPMHI                  ; initialize our APPMHI value
        ldx     MEMTOP+1
        stx     APPMHI+1

; Call module constructors

        jsr     initlib

; Initialise DLIST segment in RAM(DLIST) (load from CODE0)

        ldax    __DLIST_RUN__
        jsr     pushax
        ldax    __DLIST_LOAD__
        jsr     pushax
        ldax    __DLIST_SIZE__
        jsr     _memcpy

; set left margin to 0

        lda     LMARGN
        sta     old_lmargin
        lda     #0
        sta     LMARGN

; set keyb to upper/lowercase mode

        ldx     SHFLOK
        stx     old_shflok
        sta     SHFLOK

; Initialize conio stuff

        lda     #$FF
        sta     CH

; Initialise Display

        jsr     _clrscr
        lda     SDLSTL
        ldx     SDLSTH
        sta     dlptr
        stx     dlptr+1
        lda     #$00
        sta     SDMCTL
        ldax    __DLIST_RUN__
        sta     SDLSTL
        stx     SDLSTH
        ldax    __SCREEN_RUN__
        sta     SAVMSC
        stx     SAVMSC+1
        lda     #$22
        sta     SDMCTL

; set stdio stream handles

        lda     #0
        jsr     getfd
        sta     __filetab               ; setup stdin
        lda     #0
        jsr     getfd
        sta     __filetab + 2           ; setup stdout
        lda     #0
        jsr     getfd
        sta     __filetab + 4           ; setup stderr

; Pass command line if present

        jsr     getargs

        lda     argc
        ldx     argc+1
        jsr     pushax          ; argc
        lda     #<argv
        ldx     #>argv
        jsr     pushax          ; argv

        ldy     #4              ; Argument size
        jsr     _main           ; call the users code

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

; ReInitialise Display

        lda     #$00
        sta     SDMCTL
        lda     dlptr
        ldx     dlptr+1
        sta     SDLSTL
        stx     SDLSTH
        ldy     #$20
        jsr     incaxy
        sta     SAVMSC
        stx     SAVMSC+1
        lda     #$22
        sta     SDMCTL

; Restore system stuff

        ldx     spsave
        txs                     ; Restore stack pointer

; restore left margin

        lda     old_lmargin
        sta     LMARGN

; restore kb mode

        lda     old_shflok
        sta     SHFLOK

; restore APPMHI

        lda     appmsav
        sta     APPMHI
        lda     appmsav+1
        sta     APPMHI+1

; Copy back the zero page stuff

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Back to DOS

        rts

; *** end of main startup code

; setup sp

initsp:
        lda     APPMHI
        sta     sp
        lda     APPMHI+1
        sta     sp+1
        rts

        .data

zpsave: .res    zpspace

        .bss

spsave:         .res    1
appmsav:        .res    1
old_shflok:     .res    1
old_lmargin:    .res    1
dlptr:          .res    2

        .segment "AUTOSTRT"
        .word   $02E0
        .word   $02E1
        .word   __CODE_LOAD__ + 1

;;
;;      SCR[] (DLISTS + screen memory)
;;
        .segment        "DLIST"

__DLIST:
_SCR:   .byte   $70      ; Blank 8 lines
        .byte   $70      ; Blank 8 lines
        .byte   $70      ; Blank 8 lines
        .byte   $42      ; display ANTIC mode 2 (BASIC mode0)
        .word   __SCREEN ; Screen memory start
        .byte   $02      ; Display Antic Mode 2
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $02
        .byte   $41     ; Jump and wait for vertical
        .word   __DLIST ; blank to display list which

        .segment        "SCREEN"

__SCREEN:
        .res    (24*40) ; Screen memory area


;;       End