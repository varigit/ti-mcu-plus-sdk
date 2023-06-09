;******************************************************************************
;* LL_MUL16.ASM  - 16 BIT STATE -  v15.12.3                                   *
;*                                                                            *
;* Copyright (c) 1996-2016 Texas Instruments Incorporated                     *
;* http://www.ti.com/                                                         *
;*                                                                            *
;*  Redistribution and  use in source  and binary forms, with  or without     *
;*  modification,  are permitted provided  that the  following conditions     *
;*  are met:                                                                  *
;*                                                                            *
;*     Redistributions  of source  code must  retain the  above copyright     *
;*     notice, this list of conditions and the following disclaimer.          *
;*                                                                            *
;*     Redistributions in binary form  must reproduce the above copyright     *
;*     notice, this  list of conditions  and the following  disclaimer in     *
;*     the  documentation  and/or   other  materials  provided  with  the     *
;*     distribution.                                                          *
;*                                                                            *
;*     Neither the  name of Texas Instruments Incorporated  nor the names     *
;*     of its  contributors may  be used to  endorse or  promote products     *
;*     derived  from   this  software  without   specific  prior  written     *
;*     permission.                                                            *
;*                                                                            *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
;*                                                                            *
;******************************************************************************

;****************************************************************************
;* LL$MUL - MULTIPLY TWO 64 BIT SIGNED LONG LONG NUMBERS
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IS IN r2:r3 (r3:r2 IF LITTLE ENDIAN)
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IN r2:r3 IS NOT DESTROYED
;*
;*   o THE UPPER 64 BITS OF THE 64 X 64 MULTIPLICATION RESULT IS IGNORED.
;*
;****************************************************************************

	.thumb

        .global __TI_umull

	; Not used by TI tools - multiply generated inline

l1_hi	.set r1
l1_lo	.set r0
l2_hi	.set r3
l2_lo	.set r2

	.thumbfunc __aeabi_lmul

	.global	__aeabi_lmul
__aeabi_lmul:	.asmfunc stack_usage(12)
	PUSH	{r4-r5, lr}

        MOVS    r4, l2_lo
        MULS    r4, l1_hi

        MOVS    r5, r4
        MOVS    r4, l2_hi
        MULS    r4, l1_lo
        ADDS    r4, r5

        ; Multiply the low halves
        MOVS    r5, l1_lo
        MOVS    r0, l2_lo
        MOVS    r1, r5
        BL      __TI_umull

        ; Accumulate partial products
        ADDS    r0, r4

        ; Move the result into the right place
        MOVS    r4, r0
        MOVS    r5, r1
        MOVS    l1_hi, r4
        MOVS    l1_lo, r5

	POP	{r4-r5, pc}

	.endasmfunc

	.end
