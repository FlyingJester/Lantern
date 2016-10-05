section .text
bits 64
align 16

global Lantern_Start
global Lantern_End

extern Lantern_InitGLExt
extern Lantern_InitPrimitivesSubsystem
extern Lantern_Run

extern Glow_CreateWindow
extern Glow_DestroyWindow
extern Glow_MakeCurrent
extern Glow_ShowWindow

; Will be called with either NULL or a directory in r10
extern lantern_low_create_global_archive
extern lantern_low_destroy_global_archive

Lantern_Start:
	
	xor rax, rax
	sub rsp, 8
	mov rcx, 640
	mov rdx, 480
	mov r8, QWORD glow_title
	mov r9, 2
	
	call Glow_CreateWindow
	mov r15, rax
	
	mov rcx, r15
	call Glow_ShowWindow
	
	call Lantern_InitGLExt
	call Lantern_InitPrimitivesSubsystem
	
    xor rcx, rcx
    xor rdx, rdx
	call lantern_low_create_global_archive
	mov rdx, rax
	
	mov rcx, r15
	call Lantern_Run
	add rsp, 8
    ret

Lantern_End:
	sub rsp, 8
	call Glow_DestroyWindow
    call lantern_low_destroy_global_archive
	add rsp, 8
	mov rax, 0
	ret

section .data
	glow_title: db "Lantern Game Engine",0
