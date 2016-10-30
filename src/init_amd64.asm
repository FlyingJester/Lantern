section .text
bits 64
align 16

%if WINDOWS
%define ARG1 rcx
%define ARG2 rdx
%define ARG3 r8
%define ARG4 r9
%endif

%if UNIX
%define ARG1 rdi
%define ARG2 rsi
%define ARG3 rdx
%define ARG4 rcx
%endif


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
    mov ARG1, SCREENWIDTH
    mov ARG2, SCREENHEIGHT
    
    mov rax, QWORD ?ScreenWidth@Lantern@@3IB

    shr ARG1, 1
    mov [rax], ARG1
    shl ARG1, 1

    mov rax, QWORD ?ScreenHeight@Lantern@@3IB

    shr  ARG2, 1
    mov [rax], ARG2
    shl  ARG2, 1

    xor rax, rax
	
    mov ARG3, QWORD glow_title
    mov ARG4, 2
	
    call Glow_CreateWindow
    mov r15, rax

    mov ARG1, r15
    call Glow_ShowWindow
    
    call Lantern_InitGLExt
    call Lantern_InitPrimitivesSubsystem
	
    xor ARG1, ARG1
    xor ARG2, ARG2
	call lantern_low_create_global_archive
	mov ARG2, rax
	
	mov ARG1, r15
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
