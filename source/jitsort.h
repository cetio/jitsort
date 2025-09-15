#ifndef JITSORT_H
#define JITSORT_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/**
 * @brief Allocates executable memory and assembles instructions to sort the given array at `ptr` using the values of the elements as indexes into the allocated memory.
 * @param ptr The pointer to the array of elements.
 * @param size The size of each element. May be 0 if `func` is present.
 * @param len The length of the array. May be 0 if `func` is present.
 * @param func Alternative function to get the size of each element, returning 0 if the element is invalid or no elements remain.
 * @return Function pointer that when called returns the sorted array.
 */
static inline void* (*jitsort(
    const void* ptr, 
    size_t size, 
    size_t len, 
    size_t (*func)(const void*)
))()
{
    if (ptr == NULL || ((size == 0 || len == 0) && func == NULL))
        return NULL;
    
    const void* end = ptr + (size * len);
    size_t bytes = func == NULL ? size * len : 0;
    size_t code_size = 1024;

    if (func != NULL)
    {
        for (const void* cur = ptr; cur < end; cur = (const char*)cur + size)
        {
            size_t cur_size = func(cur);
            if (cur_size == 0)
                break;
            code_size = cur_size > code_size ? cur_size * 64 : code_size;
            bytes += cur_size;
        }
    }
    
    void* exec = mmap(
        NULL, 
        code_size, 
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, 
        -1, 
        0);
    if (exec == MAP_FAILED)
        return NULL;
    
    uint8_t* code = (uint8_t*)exec;
    memset(code, 0x90, code_size);
    
    // Prologue
    code[0] = 0x55;                    // push rbp
    code[1] = 0x48; code[2] = 0x89; code[3] = 0xe5;  // mov rbp, rsp
    code[4] = 0x53;                    // push rbx
    code[5] = 0x41; code[6] = 0x54;   // push r12
    code[7] = 0x41; code[8] = 0x55;   // push r13
    
    // Allocate memory for sorted array
    code[9] = 0x48; code[10] = 0xbf;  // mov rdi, imm64
    *(size_t*)(code + 11) = bytes;    // bytes parameter
    
    code[19] = 0x48; code[20] = 0xb8; // mov rax, imm64
    *(void**)(code + 21) = malloc;    // malloc address
    code[29] = 0xff; code[30] = 0xd0; // call rax
    
    // Store allocated pointer in r12
    code[31] = 0x49; code[32] = 0x89; code[33] = 0xc4; // mov r12, rax
    
    // Initialize position counter in r13
    code[34] = 0x41; code[35] = 0xbd; // mov r13, imm32
    *(uint32_t*)(code + 36) = 0;      // position = 0
    
    size_t offset = 40;
    size_t dest = 0;
    
    for (size_t val = 0; val < 256; val++)
    {
        for (const void* cur = ptr; cur < end; cur = (const char*)cur + size)
        {
            size_t cur_size = func != NULL ? func(cur) : size;
            if (func != NULL && cur_size == 0)
                break;
            
            size_t idx = 0;
            if (cur_size <= 8)
                memcpy(&idx, cur, cur_size);
            else
                idx = *(size_t*)cur;
            
            if (idx == val)
            {
                // Generate memcpy call for this element
                // mov rdi, r12 (dest = sorted array base)
                code[offset++] = 0x4c; code[offset++] = 0x89; code[offset++] = 0xe7;
                
                // add rdi, immediate (dest += hardcoded offset)
                if (dest != 0)
                {
                    if (dest <= 127)
                    {
                        // add rdi, imm8
                        code[offset++] = 0x48; code[offset++] = 0x83; code[offset++] = 0xc7;
                        code[offset++] = dest;
                    }
                    else
                    {
                        // add rdi, imm32
                        code[offset++] = 0x48; code[offset++] = 0x81; code[offset++] = 0xc7;
                        *(uint32_t*)(code + offset) = dest;
                        offset += 4;
                    }
                }
                
                // mov rsi, src (source = element pointer)
                code[offset++] = 0x48; code[offset++] = 0xbe; // mov rsi, imm64
                *(void**)(code + offset) = (void*)cur; // src pointer
                offset += 8;
                
                // mov rdx, size (size = element size)
                code[offset++] = 0x48; code[offset++] = 0xba; // mov rdx, imm64
                *(size_t*)(code + offset) = cur_size; // size
                offset += 8;
                
                // call memcpy
                code[offset++] = 0x48; code[offset++] = 0xb8; // mov rax, imm64
                *(void**)(code + offset) = memcpy; // memcpy address
                offset += 8;
                code[offset++] = 0xff; code[offset++] = 0xd0; // call rax
                
                dest += cur_size;
            }
        }
    }
    
    // Epilogue
    code[offset++] = 0x4c; code[offset++] = 0x89; code[offset++] = 0xe0; // mov rax, r12
    code[offset++] = 0x41; code[offset++] = 0x5d; // pop r13
    code[offset++] = 0x41; code[offset++] = 0x5c; // pop r12
    code[offset++] = 0x5b; // pop rbx
    code[offset++] = 0x5d; // pop rbp
    code[offset++] = 0xc3; // ret
    
    mprotect(exec, code_size, PROT_READ | PROT_EXEC);
    return (void*(*)())exec;
}

#endif // JITSORT_H
