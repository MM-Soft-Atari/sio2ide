;
; Startup code for cc65 (ATARI Cartridge version)
;
;
; This must be the *first* file on the linker command line
;

        .export         _exit, _CardWait

        .import         initlib, donelib
        .import         zerobss, pushax
        .import         _main,__filetab,getfd
        .import         _memcpy, incaxy, _clrscr

        .import         __STACK_START__, __STACK_SIZE__
        .import         __CODE_LOAD__, __BSS_LOAD__

        .import         __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
        .import         __DLIST_LOAD__, __DLIST_RUN__, __DLIST_SIZE__
        .import         __SCREEN_RUN__
;        .import         __RODATA_LOAD__, __RODATA_RUN__, __RODATA_SIZE__
        .import         __CODE0_LOAD__, __CODE0_RUN__, __CODE0_SIZE__
        .import         __CODE1_LOAD__, __CODE1_RUN__, __CODE1_SIZE__

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

; ------------------------------------------------------------------------
; Cartridge header

        .segment "CARTHDR"

        .word   __CART_START__  ; Cartridge start routine
        .byte   $0              ; Cartridge present
        .byte   $5              ; Cartridge flags (allow for Disk Boot)
        .word   __CART_INIT__   ; Cartridge initialisation routine

; ------------------------------------------------------------------------
; Real entry point (initialisation)

        .code

__CART_INIT__:

        rts             ; Initialisation finished

__CART_START__:

        lda     WARMST
        bne     _warm
_cold:
        jmp     _start
_warm:
        jmp     COLDSV          ; Do Cold start

_start:

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

        lda     #<(__STACK_START__+__STACK_SIZE__-1)
        sta     sp
        lda     #>(__STACK_START__+__STACK_SIZE__-1)
        sta     sp+1

; Call module constructors

        jsr     initlib

; Switch to ROM BANK 0 (0000-2000) in ROM image
; Mapped as (8000-A000)

        lda     #$0
        sta     $D500+$0

; Initialise DLIST segment in RAM(DLIST) (load from CODE0)

        ldax    __DLIST_RUN__
        jsr     pushax
        ldax    __DLIST_LOAD__
        jsr     pushax
        ldax    __DLIST_SIZE__
        jsr     _memcpy

; Initialise DATA segment in RAM (load from CODE0)

        ldax    __DATA_RUN__
        jsr     pushax
        ldax    __DATA_LOAD__
        jsr     pushax
        ldax    __DATA_SIZE__
        jsr     _memcpy

; Initialise RODATA segment in RAM0 (load from CODE0)

;        ldax    __RODATA_RUN__
;        jsr     pushax
;        ldax    __RODATA_LOAD__
;        jsr     pushax
;        ldax    __RODATA_SIZE__
;        jsr     _memcpy

; Initialise CODE0 segment in RAM0

        ldax    __CODE0_RUN__
        jsr     pushax
        ldax    __CODE0_LOAD__
        jsr     pushax
        ldax    __CODE0_SIZE__
        jsr     _memcpy

; Initialise CODE1 segment in RAM1
; Switch to ROM BANK 1 (2000-4000) in ROM image

        lda     #$1
        sta     $D500+$1

        ldax    __CODE1_RUN__
        jsr     pushax
        ldax    __CODE1_LOAD__
        jsr     pushax
        ldax    __CODE1_SIZE__
        jsr     _memcpy

; Switch to ROM BANK 2 (4000-6000) in ROM image
; Mapped as (8000-A000)

        lda     #$2
        sta     $D500+$2

; set left margin to 0

        lda     #0
        sta     LMARGN

; set keyb to upper/lowercase mode

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

        jsr     _main           ; call main

_exit:                          ; exit to OS (DOS)

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

; Done lib

        jsr     donelib

; Restore system stuff

        ldx     spsave
        txs                     ; Restore stack pointer

; Copy back the zero page stuff

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

        jmp     _dos

        .segment "CODE1"

;;
;; Leave to OS from RAM
;;
_dos:
        jmp     (DOSVEC)        ;

;;
;;  Wait for card release (RD4&RD5 switch off)
;;
_CardWait:
        lda     #$80            ; For Nir Dary XEGS
        sta     $D500           ;
_CWait:
        lda     TRIG3           ; wait for card release
        bne     _CWait
        sta     GINTLK
;        jmp     _dos
        jmp     COLDSV          ; Do Cold start

        .data

zpsave: .res    zpspace

        .bss

spsave: .res    1
dlptr:  .res    2

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

;;      End
