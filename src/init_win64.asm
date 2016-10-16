section .text
bits 64
align 16

global Lantern_Start
global Lantern_End
global ?ScreenWidth@Lantern@@3IB
global ?ScreenHeight@Lantern@@3IB

global Lantern_ScreenWidth
global Lantern_ScreenHeight

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

%define SCREENWIDTH 640
%define SCREENHEIGHT 480

Lantern_Start:
	
	sub rsp, 8
	mov rcx, SCREENWIDTH
	mov rdx, SCREENHEIGHT
    
    mov rax, QWORD ?ScreenWidth@Lantern@@3IB

    shr rcx, 1
	mov [rax], rcx
    shl rcx, 1

    mov rax, QWORD ?ScreenHeight@Lantern@@3IB

    shr  rdx, 1
	mov [rax], rdx
    shl  rdx, 1

    xor rax, rax
	
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

section .bss
    Lantern_ScreenWidth:
    ?ScreenWidth@Lantern@@3IB: resd 1
    Lantern_ScreenHeight:
    ?ScreenHeight@Lantern@@3IB: resd 1

section .data
	glow_title: db "Lantern Game Engine",0
