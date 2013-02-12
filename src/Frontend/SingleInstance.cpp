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

#include "SingleInstance.h"

SingleInstance::SingleInstance()
{
    m_data      = NULL;
    m_mutex     = NULL;
    m_hInstance = NULL;
}

SingleInstance::~SingleInstance()
{

    if (m_data != NULL)
    {
        UnmapViewOfFile(m_data);
        m_data = NULL;
    }

    if (m_hInstance != NULL)
    {
        CloseHandle(m_hInstance);
        m_hInstance = NULL;
    }

    if (m_mutex != NULL)
    {
        CloseHandle(m_mutex);
        m_mutex = NULL;
    }

}

HWND SingleInstance::Connect(HWND hWnd, const char* name)
{

    char mutexName[256];
    _snprintf(mutexName, 256, "%s_mutex", name);

    // Try to open the memory mapped file to see if there's an instance
    // already running.
    
    m_hInstance = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);

    if (m_hInstance == NULL)
    {
        
        m_mutex = CreateMutex(NULL, TRUE, mutexName);
        
        // Create a new memory mapped file to store the instance data inside.
        m_hInstance = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(Data), name);

        // Store the window handle for access by other instances.
        m_data = (Data*) MapViewOfFile(m_hInstance, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Data));
        m_data->hWnd = hWnd;   
        m_data->commandLine[0] = 0;

        ReleaseMutex(m_mutex);
    
    }
    else
    {
        
        m_mutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, mutexName);
        
        // The application is already running, so read the instance data from
        // the memory mapped file.
        m_data = (Data*) MapViewOfFile(m_hInstance, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Data));
   
        // Get the handle for the window from the previous instance.
        HWND hWndPrev = NULL;

        if (m_data != NULL)
        {
            hWndPrev = m_data->hWnd;
        }

        return hWndPrev;

    }

    return NULL;

}

void SingleInstance::SetCommand(const wxString& commandLine)
{
    if (WaitForSingleObject(m_mutex, INFINITE) == WAIT_OBJECT_0)
    {
        strncpy(m_data->commandLine, commandLine, s_maxCommandLineLength);
        ReleaseMutex(m_mutex);
    }
}

wxString SingleInstance::GetCommand() const
{
    wxString result;
    if (WaitForSingleObject(m_mutex, INFINITE) == WAIT_OBJECT_0)
    {
        result = m_data->commandLine;
        ReleaseMutex(m_mutex);
    }
    return result;
}