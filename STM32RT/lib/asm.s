@ additional code in 

	.cpu cortex-m4
	.arch armv7e-m
	.fpu fpv4-sp-d16
	.global memset
    .global memcpy
    .syntax unified
    .type   memset, %function
    .type   memcpy, %function    
    .thumb
	
    .thumb_func
memset:
	@ R0: memory
	@ R1: byte
	@ R2: size

	push {R0, R2}

memset_loop:
	strb	R1, [R0]
	add		R0, R0, #1
	sub		R2, R2, #1
	cmp		R2, #0
	bne		memset_loop
	
	pop {R0, R2}
	bx	lr

	.thumb_func
memcpy:
	@ R0: destination
	@ R1: source
	@ R2: size

	push {R0, R2, R3}

memcpy_loop:
	ldrb	R3, [R1]
	strb	R3, [R0]
	add		R1, R1, #1
	add		R0, R0, #1
	sub		R2, R2, #1
	cmp		R2, #0
	bne		memcpy_loop
	
	pop {R0, R2, R3}
	bx	lr
