/*

Decoda
Copyright (C) 2007-2013 Unknown Worlds Entertainment, Inc. 

This file is part of Decoda.

Decoda is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Decoda is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Decoda.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Hook.h"
#include <windows.h>
#include <stdio.h>
#include <assert.h>

HANDLE g_trampolineHeap = NULL;

/**
 * Returns the size of the instruction at the specified point.
 */
int GetInstructionSize(void* address, unsigned char* opcodeOut = NULL, int* operandSizeOut = NULL) 
{ 
    
    // Modified from http://www.devmaster.net/forums/showthread.php?p=47381

    unsigned char* func = static_cast<unsigned char*>(address);

    if (opcodeOut != NULL)
    {
        *opcodeOut = 0;
    }

    if (operandSizeOut != NULL)
    {
        *operandSizeOut = 0;
    }

    if (*func != 0xCC) 
    { 
        // Skip prefixes F0h, F2h, F3h, 66h, 67h, D8h-DFh, 2Eh, 36h, 3Eh, 26h, 64h and 65h
        int operandSize = 4; 
        int FPU = 0; 
        while(*func == 0xF0 || 
              *func == 0xF2 || 
              *func == 0xF3 || 
             (*func & 0xFC) == 0x64 || 
             (*func & 0xF8) == 0xD8 ||
             (*func & 0x7E) == 0x62)
        { 
            if(*func == 0x66) 
            { 
                operandSize = 2; 
            }
            else if((*func & 0xF8) == 0xD8) 
            {
                FPU = *func++;
                break;
            }

            func++;
        }

        // Skip two-byte opcode byte 
        bool twoByte = false; 
        if(*func == 0x0F) 
        { 
            twoByte = true; 
            func++; 
        } 

        // Skip opcode byte 
        unsigned char opcode = *func++; 

        // Skip mod R/M byte 
        unsigned char modRM = 0xFF; 
        if(FPU) 
        { 
            if((opcode & 0xC0) != 0xC0) 
            { 
                modRM = opcode; 
            } 
        } 
        else if(!twoByte) 
        { 
            if((opcode & 0xC4) == 0x00 || 
               (opcode & 0xF4) == 0x60 && ((opcode & 0x0A) == 0x02 || (opcode & 0x09) == 0x9) || 
               (opcode & 0xF0) == 0x80 || 
               (opcode & 0xF8) == 0xC0 && (opcode & 0x0E) != 0x02 || 
               (opcode & 0xFC) == 0xD0 || 
               (opcode & 0xF6) == 0xF6) 
            { 
                modRM = *func++; 
            } 
        } 
        else 
        { 
            if((opcode & 0xF0) == 0x00 && (opcode & 0x0F) >= 0x04 && (opcode & 0x0D) != 0x0D || 
               (opcode & 0xF0) == 0x30 || 
               opcode == 0x77 || 
               (opcode & 0xF0) == 0x80 || 
               (opcode & 0xF0) == 0xA0 && (opcode & 0x07) <= 0x02 || 
               (opcode & 0xF8) == 0xC8) 
            { 
                // No mod R/M byte 
            } 
            else 
            { 
                modRM = *func++; 
            } 
        } 

        // Skip SIB
        if((modRM & 0x07) == 0x04 &&
           (modRM & 0xC0) != 0xC0)
        {
            func += 1;   // SIB
        }

        // Skip displacement
        if((modRM & 0xC5) == 0x05) func += 4;   // Dword displacement, no base 
        if((modRM & 0xC0) == 0x40) func += 1;   // Byte displacement 
        if((modRM & 0xC0) == 0x80) func += 4;   // Dword displacement 

        // Skip immediate 
        if(FPU) 
        { 
            // Can't have immediate operand 
        } 
        else if(!twoByte) 
        { 
            if((opcode & 0xC7) == 0x04 || 
               (opcode & 0xFE) == 0x6A ||   // PUSH/POP/IMUL 
               (opcode & 0xF0) == 0x70 ||   // Jcc 
               opcode == 0x80 || 
               opcode == 0x83 || 
               (opcode & 0xFD) == 0xA0 ||   // MOV 
               opcode == 0xA8 ||            // TEST 
               (opcode & 0xF8) == 0xB0 ||   // MOV
               (opcode & 0xFE) == 0xC0 ||   // RCL 
               opcode == 0xC6 ||            // MOV 
               opcode == 0xCD ||            // INT 
               (opcode & 0xFE) == 0xD4 ||   // AAD/AAM 
               (opcode & 0xF8) == 0xE0 ||   // LOOP/JCXZ 
               opcode == 0xEB || 
               opcode == 0xF6 && (modRM & 0x30) == 0x00)   // TEST 
            { 
                func += 1; 
            } 
            else if((opcode & 0xF7) == 0xC2) 
            { 
                func += 2;   // RET 
            } 
            else if((opcode & 0xFC) == 0x80 || 
                    (opcode & 0xC7) == 0x05 || 
                    (opcode & 0xF8) == 0xB8 ||
                    (opcode & 0xFE) == 0xE8 ||      // CALL/Jcc 
                    (opcode & 0xFE) == 0x68 || 
                    (opcode & 0xFC) == 0xA0 || 
                    (opcode & 0xEE) == 0xA8 || 
                    opcode == 0xC7 || 
                    opcode == 0xF7 && (modRM & 0x30) == 0x00) 
            { 
                func += operandSize; 
            } 
        } 
        else 
        {
            if(opcode == 0xBA ||            // BT 
               opcode == 0x0F ||            // 3DNow! 
               (opcode & 0xFC) == 0x70 ||   // PSLLW 
               (opcode & 0xF7) == 0xA4 ||   // SHLD 
               opcode == 0xC2 || 
               opcode == 0xC4 || 
               opcode == 0xC5 || 
               opcode == 0xC6) 
            { 
                func += 1; 
            } 
            else if((opcode & 0xF0) == 0x80) 
            {
                func += operandSize;   // Jcc -i
            }
        }

        if (opcodeOut != NULL)
        {
            *opcodeOut = opcode;
        }

        if (operandSizeOut != NULL)
        {
            *operandSizeOut = operandSize;
        }

    }

    return func - static_cast<unsigned char*>(address);

}

/**
 * Returns the number of bytes until the next break in instructions.
 */
int GetInstructionBoundary(void* function, int count)
{

    int boundary = 0;

    while (boundary < count)
    {
        unsigned char* func = static_cast<unsigned char*>(function) + boundary;
        boundary += GetInstructionSize(func);
    }

    return boundary;

}

/**
 * Writes a relative jmp instruction.
 */
void WriteJump(void* dst, void* address)
{
    
    unsigned char* jump = static_cast<unsigned char*>(dst);

    // Setup a jump instruction.
    jump[0] = 0xE9;
    *((unsigned long*)(&jump[1])) = (unsigned long)(address) - (unsigned long)(dst) - 5;

}

void* ReadJump(void* src)
{
    void* address = NULL;
    __try
    {
        const unsigned char* jump = static_cast<unsigned char*>(src);
        if (jump[0] == 0xE9)
        {   
            address = reinterpret_cast<void*>(*((unsigned long*)(&jump[1])) + (unsigned long)(jump) + 5);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
    return address;
}

/**
 * Adds the specified offset to all relative jmp instructions in the range of
 * the function specified. This is useful when a piece of code is moved in
 * memory.
 */
void AdjustRelativeJumps(void* function, int length, int offset)
{

    unsigned char* p = reinterpret_cast<unsigned char*>(function);

    int i = 0;

    while (i < length)
    {
    
        unsigned char opcode = 0;
        int operandSize = 0;

        int n = GetInstructionSize(p + i, &opcode, &operandSize);

        if (opcode == 0xE9 || opcode == 0xE8)
        {
            // Relative jump/call instruction.
            if (operandSize == 4)
            {
                unsigned long address = *((unsigned long*)(p + i + n - operandSize));
                *((unsigned long*)(p + i + n - operandSize)) = address + offset;
            }
            else if (operandSize == 2)
            {
                unsigned short address = *((unsigned short*)(p + i + n - operandSize));
                *((unsigned short*)(p + i + n - operandSize)) = address + offset;
            }
            else
            {
                assert(0);
            }
        }

        i += n;

    }

}

bool GetIsHooked(void* function, void* hook)
{

    unsigned char* address = static_cast<unsigned char*>(ReadJump(function));

    if (address == NULL)
    {
        return false;
    }

    if (address == hook)
    {   
        return true;
    }

    return ReadJump(address + 13) == hook;

}

void* HookFunction(void* function, void* hook)
{

    // Don't allow rehooking of the same function since that screws things up.

    assert(!GetIsHooked(function, hook));

    if (GetIsHooked(function, hook))
    {
        return NULL;
    }

    // Missing from windows.h
    #define HEAP_CREATE_ENABLE_EXECUTE 0x00040000

    // Jump instruction is 5 bytes.
    const int jumpSize = 5;

    // Compute the instruction boundary so we don't override half an instruction.
    int boundary = GetInstructionBoundary(function, jumpSize);

    if (g_trampolineHeap == NULL)
    {
        // Create a new heap where we'll add our trampoline functions.
        g_trampolineHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 0);
    }

    // Construct a new function that will invoke the original function.
    unsigned char* trampoline = static_cast<unsigned char*>(HeapAlloc(g_trampolineHeap, 0, boundary + jumpSize));

    if (trampoline == NULL)
    {
        // Out of memory.
        return NULL;
    }

    // Copy the original bytes to the trampoline area and append a jump back
    // to the original function (after our jump).

    memcpy(trampoline, function, boundary);
    AdjustRelativeJumps(trampoline, boundary, ((unsigned char*)function) - trampoline);

    WriteJump(trampoline + boundary, ((unsigned char*)function) + boundary);

    DWORD protection;

    // Give ourself write access to the region.
    if (VirtualProtect(function, boundary, PAGE_EXECUTE_READWRITE, &protection))
    {

        // Fill the area with nops and write the jump instruction to our
        // hook function.
        memset(function, 0x90, boundary);
        WriteJump(function, hook);

        // Restore the original protections.
        VirtualProtect(function, boundary, protection, &protection);

        // Flush the cache so we know that our new code gets executed.
        FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

        return trampoline;
    
    }    

    return NULL;

}

void* HookFunction(void* function, void* hook, unsigned long upValue)
{

    if (GetIsHooked(function, hook))
    {

        unsigned char* setup = static_cast<unsigned char*>(ReadJump(function));
        
        // Get the address of the trampoline function we stored before the setup code.
        unsigned long trampoline = *((unsigned long*)(setup - 4));
        return (void*)trampoline;

    }

    // Missing from windows.h
    #define HEAP_CREATE_ENABLE_EXECUTE 0x00040000

    // Jump instruction is 5 bytes.
    const int jumpSize    = 5;
    
    // Misc data we're going to store (the original function) fits in 4 bytes.
    const int storageSize = 5;

    // Compute the instruction boundary so we don't override half an instruction.
    int boundary = GetInstructionBoundary(function, jumpSize);

    if (g_trampolineHeap == NULL)
    {
        // Create a new heap where we'll add our trampoline functions.
        g_trampolineHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 0);
    }

    // Construct a new function that will invoke the original function.
    unsigned char* trampoline = static_cast<unsigned char*>(HeapAlloc(g_trampolineHeap, 0, boundary + jumpSize));

    if (function == hook)
    {
        hook = trampoline;
    }

    // Construct a new function that will invoke our hook with the up value.
    // We can't write this directly into the original function, since the
    // original function may only be 5 bytes long (aka just a jump).
    unsigned char* setup = static_cast<unsigned char*>(HeapAlloc(g_trampolineHeap, 0, 21));

    if (trampoline == NULL || setup == NULL)
    {
        // Out of memory.
        return NULL;
    }

    // Copy the original bytes to the trampoline area and append a jump back
    // to the original function (after our jump).

    memcpy(trampoline, function, boundary);
    AdjustRelativeJumps(trampoline, boundary, ((unsigned char*)function) - trampoline);

    WriteJump(trampoline + boundary, ((unsigned char*)function) + boundary);

    // Write the setup function.

    unsigned char* p = static_cast<unsigned char*>(setup);

    // Store the address to the trampoline code as the first 4 bytes
    // so that we can check this when attempting to rehook.
    *((unsigned long*)(p + 0)) = (unsigned long)trampoline;
    
    // push        eax  
    p[4] = 0x50;
    
    // mov         eax, 11223344h 
    p[5] = 0xB8;
    *((unsigned long*)(p + 6)) = upValue;

    // xchg        eax, dword ptr [esp+4] 
    p[10] = 0x87;
    p[11] = 0x44;
    p[12] = 0x24;
    p[13] = 0x04;

    // xchg        eax, dword ptr [esp] 
    p[14] = 0x87;
    p[15] = 0x04;
    p[16] = 0x24;

    // Write the jmp instruction to the hook function.
    WriteJump(p + 17, hook);

    DWORD protection;

    // Give ourself write access to the region.
    if (VirtualProtect(function, boundary, PAGE_EXECUTE_READWRITE, &protection))
    {

        // Fill the area with nops so that we don't leave partial instructions.
        memset(function, 0x90, boundary);

        // Write the jmp instruction to the hook function.
        WriteJump(function, setup + 4);

        // Restore the original protections.
        VirtualProtect(function, boundary, protection, &protection);

        // Flush the cache so we know that our new code gets executed.
        FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

        return trampoline;
    
    }    

    // Couldn't overwrite the function.
    return NULL;

}

void* InstanceFunction(void* function, unsigned long upValue)
{

    // Missing from windows.h
    #define HEAP_CREATE_ENABLE_EXECUTE 0x00040000

    if (g_trampolineHeap == NULL)
    {
        // Create a new heap where we'll add our trampoline functions.
        g_trampolineHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 0);
    }

    // Construct a new function that will invoke our hook with the up value.
    // We can't write this directly into the original function, since the
    // original function may only be 5 bytes long (aka just a jump).
    unsigned char* setup = static_cast<unsigned char*>(HeapAlloc(g_trampolineHeap, 0, 17));

    if (setup == NULL)
    {
        // Out of memory.
        return NULL;
    }

    // Write the setup function.

    unsigned char* p = static_cast<unsigned char*>(setup);
    
    // push        eax  
    p[0] = 0x50;
    
    // mov         eax, 11223344h 
    p[1] = 0xB8;
    *((unsigned long*)(p + 2)) = upValue;

    // xchg        eax, dword ptr [esp+4] 
    p[6] = 0x87;
    p[7] = 0x44;
    p[8] = 0x24;
    p[9] = 0x04;

    // xchg        eax, dword ptr [esp] 
    p[10] = 0x87;
    p[11] = 0x04;
    p[12] = 0x24;

    // Write the jmp instruction to the original function.
    WriteJump(p + 13, function);

    return setup;

}