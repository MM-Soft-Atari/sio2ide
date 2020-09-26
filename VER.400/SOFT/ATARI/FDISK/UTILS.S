;=================================================================
; Copyright (C), 2002 MMSoft, All rights reserved
;=================================================================

;=================================================================
;
; SOURCE FILE:  UTILS.S
;
; MODULE NAME:  UTILS
;
; PURPOSE:      SIO routine in ASM.
;
; AUTHOR:       Marek Mikolajewski (MM)
;
; REVIEWED BY:
;
; HISTORY:      Ver   Date       Sign   Description
;
;               001   27-03-2002 MM     Created
;
;=================================================================

.fopt           compiler,"cc65 v 2.6.1"

.autoimport     on
.case           on
.debuginfo      on
.importzp       sp, sreg, regsave, regbank, tmp1, ptr1
.macpack        longbranch

SIOV    =$E459                  ;SIO ROUTINE

.segment        "CODE"

;;
;;      char siov(void);
;;

        .export _siov

_siov:
        jsr     SIOV
;
        bmi     err1            ; ok, return it
        lda     #0              ; return NULL
        rts
err1:
        tya
        rts
; ****
