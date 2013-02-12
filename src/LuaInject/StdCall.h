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

#ifndef STDCALL_H
#define STDCALL_H

#pragma runtime_checks( "[runtime_checks]", off )

/**
 * Checks whether or not a function that takes one parameter is using the stdcall
 * calling convention. This function calls the specified function to determine
 * this. The result of calling the function is returned in result.
 */
bool GetIsStdCallConvention(void* function, void* arg1, void** result);

/**
 * Checks whether or not a function that takes two parameter is using the stdcall
 * calling convention. This function calls the specified function to determine
 * this. The result of calling the function is returned in result.
 */
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void** result);

/**
 * Checks whether or not a function that takes three parameters is using the stdcall
 * calling convention. This function calls the specified function to determine
 * this. The result of calling the function is returned in result.
 */
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void** result);

/**
 * Checks whether or not a function that takes four parameters is using the stdcall
 * calling convention. This function calls the specified function to determine
 * this. The result of calling the function is returned in result.
 */
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void* arg4, void** result);

/**
 * Checks whether or not a function that takes four parameters is using the stdcall
 * calling convention. This function calls the specified function to determine
 * this. The result of calling the function is returned in result.
 */
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void** result);

/**
 * Checks whether or not a function that takes four parameters is using the stdcall
 * calling convention. This function calls the specified function to determine
 * this. The result of calling the function is returned in result.
 */
bool GetIsStdCallConvention(void* function, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, void** result);

#pragma runtime_checks( "[runtime_checks]", restore )

#endif