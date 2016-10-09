struc ArchiveData
	.filedata resq 1
	.filesize resq 1
endstruc

%define ARCHIVE_DATA_SIZE 16
%define ARCHIVE_SIZE_SHL 4
%define MAX_ARCHIVES 127

section .text
bits 64
align 16

; Will be called with either NULL or a directory in r10
global lantern_low_create_global_archive
global lantern_low_destroy_global_archive
global lantern_num_stored_archives

extern Lantern_CreateArchiveServer
extern Lantern_DestroyArchiveServer
extern Lantern_AppendToArchiveServer

; unsigned Lantern_FileFinderSize();
extern Lantern_FileFinderSize
; bool Lantern_InitFileFinder(struct Lantern_FileFinder *finder, const char *path);
extern Lantern_InitFileFinder
;void Lantern_DestroyFileFinder(struct Lantern_FileFinder *finder);
extern Lantern_DestroyFileFinder
;const char *Lantern_FileFinderPath(const struct Lantern_FileFinder *finder);
extern Lantern_FileFinderPath
;const char *Lantern_FileFinderPath(const struct Lantern_FileFinder *finder);
extern Lantern_FileFinderPath
; unsigned long Lantern_FileFinderFileSize(const struct Lantern_FileFinder *finder);
extern Lantern_FileFinderFileSize
;bool Lantern_FileFinderNext(struct Lantern_FileFinder *finder);
extern Lantern_FileFinderNext

;void *BufferFile(const char *file, int *size);
extern BufferFile
;void FreeBufferFile(void *in, int size);
extern FreeBufferFile

extern strlen
extern strcpy

; Archive path string address should be in rdx
new_archive_path:
    ; Init the file finder
    mov rcx, QWORD file_finder
    push r15
    push r14
    push r13
    mov r13, rdx ; r13 holds the starting string at first
    call Lantern_InitFileFinder
    cmp rax, 0
    je archive_end
do_archive:
    
    ; Make r13 hold the end of the original string, and
    ; path_scratch hold a copy of the original string.
    mov rcx, QWORD path_scratch
    mov rdx, r13
    call strcpy
    
    mov rcx, r13
    call strlen
    mov rcx, QWORD path_scratch
    mov r13, rax
    add r13, rcx 
    
    ; Fetch the file path
    mov rcx, QWORD file_finder
    call Lantern_FileFinderPath
    cmp rax, 0
    je archive_end
    
    mov rcx, r13
    mov rdx, rax
    call strcpy
    
    ; Get the archive location, put it into r15,
    ; increment the number of stored archives
    mov r9, QWORD num_archives
    mov r14, QWORD [r9]
    mov r15, r14
    inc r14
    mov QWORD [r9], r14
    shl r15, ARCHIVE_SIZE_SHL
    mov r9, QWORD archives
    add r15, r9
    mov rdx, r15 ; Move this into rdx.
    ; We leave the archvie struct offset in r15 for later use
    
    ; Load the filesize for the call to BufferFile
    add rdx, ArchiveData.filesize
    
    mov rcx, QWORD path_scratch
    call BufferFile
    
    cmp rax, 0
    je find_next
    
    ; Put the file data pointer into the archives array
    mov r8, r15
    add r8, ArchiveData.filedata
    
    mov QWORD [r8], rax
    
    ; Archive server is first arg
    mov rcx, QWORD archive_server
    mov rcx, QWORD [rcx]
    
    ; Put the data pointer into the second arg for the append call
    mov rdx, rax

    ; Load the file size into the third and fourth(?) arg
    mov r8, r15
    add r8, ArchiveData.filesize
    mov r9, QWORD [r8]
    mov r8, r9
    call Lantern_AppendToArchiveServer
 
find_next:
    mov rcx, QWORD file_finder
    call Lantern_FileFinderNext
    test eax, eax
    jnz do_archive
archive_end:

    pop r13
    pop r14
    
    mov rcx, QWORD file_finder
    call Lantern_DestroyFileFinder
    
    pop r15
    ret

; Put the destination in r15, the source in rax. Will modify r8, r15, rax
; copy_string is the actual entry point.

;copy_string_start:
;    mov BYTE [r15], r8
;    inc rax
;    inc r15
;copy_string:
;    mov r8, BYTE [rax]
;    cmp r8, 255
;    jnz copy_string_start
;copy_string_end:
;    ret

lantern_low_create_global_archive:  
	push r15
	mov r15, r8 ; Put the path in r15
	call Lantern_CreateArchiveServer
	mov QWORD [QWORD archive_server], rax
	cmp rax, 0
	je create_end
	
    ; TODO: handle prefixes
    cmp r15, 0
;	jne create_end
    
    ; find texture archives
    mov rdx, QWORD default_texture_path
    call new_archive_path
    
    ; find item archives
    mov rdx, QWORD default_item_path
    call new_archive_path
    
    ; find room archives
    mov rdx, QWORD default_room_path
    call new_archive_path

create_end:
	pop r15
	mov rax, QWORD [QWORD archive_server]
	ret

lantern_low_destroy_global_archive:
	push r15
	mov rcx, QWORD archive_server
    mov rcx, [rcx]
    ; TODO: WTF is happening here?
	call Lantern_DestroyArchiveServer
    
    mov rax, QWORD num_archives
    mov r15, QWORD [rax]
    test r15, r15
    jz end_destroy
    
    dec r15
    shl r15, ARCHIVE_SIZE_SHL
    mov rax, QWORD archives
    add r15, rax
    
free_buffer_file:
    mov rcx, r15
    add rcx, ArchiveData.filedata
    
    mov rdx, r15
    add rcx, ArchiveData.filesize
    
    call FreeBufferFile
    sub r15, ARCHIVE_DATA_SIZE
    mov rax, QWORD archives
    cmp r15, rax
    jne free_buffer_file
end_destroy:
	pop r15
    ret

lantern_num_stored_archives:
    mov rax, QWORD [QWORD num_archives]
    ret

section .data
    texture_prefix: times 255 db 0
    default_texture_path: db "textures/"
    texture_file: times 255 db 0
    
    item_prefix: times 255 db 0
    default_item_path: db "items/"
    item_file: times 255 db 0
    
    room_prefix: times 255 db 0
    default_room_path: db "rooms/"
    room_file: times 255 db 0

    num_archives: dq 0
    
section .bss
    path_scratch: resb 255
    file_finder: resq 64
	archive_server: resq 1
	archives: times MAX_ARCHIVES resb ARCHIVE_DATA_SIZE
