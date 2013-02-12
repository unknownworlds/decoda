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

#include "StdCall.h"

#pragma runtime_checks( "s", off )
bool GetIsStdCallConvention(void* function, void* arg1, void** result)
{

    int stackLeftOver;
    int final;

    __asm
    {
     
        // Remember the stack pointer, so we can check if it got cleaned up.
        mov     stackLeftOver,  esp
        
        // Call the function.
        push    arg1
        call    function

        // Store the result.
        mov     final,          eax

        // Compute if the arguments were left on the stack by the function.
        mov     eax,            stackLeftOver
        sub     eax,            esp
        mov     stackLeftOver,  eax

        // Fix the stack.
        add     esp,            stackLeftOver

    }

    if (result)
    {
        *result = (void*)final;
    }

    // If anything was left on the stack after we called the function,
    // then it's the cdecl convention.
    return stackLeftOver == 0;


}
#pragma runtime_checks( "s", restore )

#pragma runtime_checks( "s", off )
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void** result)
{

    int stackLeftOver;
    int final;

    __asm
    {
     
        // Remember the stack pointer, so we can check if it got cleaned up.
        mov     stackLeftOver,  esp
        
        // Call the function.
        push    arg2
        push    arg1
        call    function

        // Store the result.
        mov     final,          eax

        // Compute if the arguments were left on the stack by the function.
        mov     eax,            stackLeftOver
        sub     eax,            esp
        mov     stackLeftOver,  eax

        // Fix the stack.
        add     esp,            stackLeftOver

    }

    if (result)
    {
        *result = (void*)final;
    }

    // If anything was left on the stack after we called the function,
    // then it's the cdecl convention.
    return stackLeftOver == 0;

}
#pragma runtime_checks( "s", restore )

#pragma runtime_checks( "s", off )
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void** result)
{

    int stackLeftOver;
    int final;

    __asm
    {
     
        // Remember the stack pointer, so we can check if it got cleaned up.
        mov     stackLeftOver,  esp
        
        // Call the function.
        push    arg3
        push    arg2
        push    arg1
        call    function

        // Store the result.
        mov     final,          eax

        // Compute if the arguments were left on the stack by the function.
        mov     eax,            stackLeftOver
        sub     eax,            esp
        mov     stackLeftOver,  eax

        // Fix the stack.
        add     esp,            stackLeftOver

    }

    if (result)
    {
        *result = (void*)final;
    }

    // If anything was left on the stack after we called the function,
    // then it's the cdecl convention.
    return stackLeftOver == 0;

}
#pragma runtime_checks( "s", restore )

#pragma runtime_checks( "s", off )
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void* arg4, void** result)
{

    int stackLeftOver;
    int final;

    __asm
    {
     
        // Remember the stack pointer, so we can check if it got cleaned up.
        mov     stackLeftOver,  esp
        
        // Call the function.
        push    arg4
        push    arg3
        push    arg2
        push    arg1
        call    function

        // Store the result.
        mov     final,          eax

        // Compute if the arguments were left on the stack by the function.
        mov     eax,            stackLeftOver
        sub     eax,            esp
        mov     stackLeftOver,  eax

        // Fix the stack.
        add     esp,            stackLeftOver

    }

    if (result)
    {
        *result = (void*)final;
    }

    // If anything was left on the stack after we called the function,
    // then it's the cdecl convention.
    return stackLeftOver == 0;

}
#pragma runtime_checks( "s", restore )

#pragma runtime_checks( "s", off )
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void** result)
{

    int stackLeftOver;
    int final;

    __asm
    {
     
        // Remember the stack pointer, so we can check if it got cleaned up.
        mov     stackLeftOver,  esp
        
        // Call the function.
        push    arg5
        push    arg4
        push    arg3
        push    arg2
        push    arg1
        call    function

        // Store the result.
        mov     final,          eax

        // Compute if the arguments were left on the stack by the function.
        mov     eax,            stackLeftOver
        sub     eax,            esp
        mov     stackLeftOver,  eax

        // Fix the stack.
        add     esp,            stackLeftOver

    }

    if (result)
    {
        *result = (void*)final;
    }

    // If anything was left on the stack after we called the function,
    // then it's the cdecl convention.
    return stackLeftOver == 0;

}

#pragma runtime_checks( "s", off )
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, void** result)
{

    int stackLeftOver;
    int final;

    __asm
    {
     
        // Remember the stack pointer, so we can check if it got cleaned up.
        mov     stackLeftOver,  esp
        
        // Call the function.
        push    arg6
        push    arg5
        push    arg4
        push    arg3
        push    arg2
        push    arg1
        call    function

        // Store the result.
        mov     final,          eax

        // Compute if the arguments were left on the stack by the function.
        mov     eax,            stackLeftOver
        sub     eax,            esp
        mov     stackLeftOver,  eax

        // Fix the stack.
        add     esp,            stackLeftOver

    }

    if (result)
    {
        *result = (void*)final;
    }

    // If anything was left on the stack after we called the function,
    // then it's the cdecl convention.
    return stackLeftOver == 0;

}

#pragma runtime_checks( "s", restore )