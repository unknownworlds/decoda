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

#include "FileChangeWatcher.h"
#include "FileEvent.h"

FileChangeWatcher::FileChangeWatcher()
{
    m_handle            = NULL;
    m_thread            = NULL;
    m_threadEndEvent    = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_eventHandler      = NULL;
}

FileChangeWatcher::~FileChangeWatcher()
{
    Shutdown();
}

void FileChangeWatcher::Shutdown()
{

    if (m_thread != NULL)
    {
        EndThread();
        m_thread = NULL;
    }

    if (m_handle != NULL)
    {
        FindCloseChangeNotification(m_handle);
        m_handle = NULL;
    }

    CloseHandle(m_threadEndEvent);
    m_threadEndEvent = NULL;

}

void FileChangeWatcher::SetEventHandler(wxEvtHandler* eventHandler)
{
    m_eventHandler = eventHandler;
}

void FileChangeWatcher::SetFile(const wxFileName& fileName)
{

    // If the file is in the same directory, we don't need to update anything.
    bool sameDirectory = (fileName.GetPath() == m_fileName.GetPath());

    m_fileName = fileName;
    UpdateFileAttributes();

    if (!sameDirectory || m_handle == NULL)
    {

        if (m_thread != NULL)
        {
            EndThread();
        }

        if (m_handle != NULL)
        {
            FindCloseChangeNotification(m_handle);
            m_handle = NULL;
        }

        m_handle = FindFirstChangeNotification(m_fileName.GetPath(), FALSE, FILE_NOTIFY_CHANGE_ATTRIBUTES);

        // Sometimes the return is NULL and sometimes it's INVALID_HANDLE_VALUE
        // in error conditions.
        if (m_handle == INVALID_HANDLE_VALUE)
        {
            m_handle = NULL;
        }

        if (m_handle != NULL)
        {
            DWORD threadId;
            m_thread = CreateThread(NULL, 0, ThreadProc, this, 0, &threadId);
        }
    
    }

}

bool FileChangeWatcher::UpdateFileAttributes()
{
    
    DWORD fileAttributes = m_fileAttributes;
    m_fileAttributes = GetFileAttributes(m_fileName.GetFullPath());
    
    return m_fileAttributes != fileAttributes;

}

void FileChangeWatcher::EndThread()
{

    SetEvent(m_threadEndEvent);
    WaitForSingleObject(m_thread, INFINITE);
        
    CloseHandle(m_thread);
    m_thread = NULL;

}

DWORD WINAPI FileChangeWatcher::ThreadProc(LPVOID param)
{

    FileChangeWatcher* self = static_cast<FileChangeWatcher*>(param);

    while (1)
    {

        HANDLE handle[2];

        handle[0] = self->m_handle;
        handle[1] = self->m_threadEndEvent;
        
        DWORD result = WaitForMultipleObjects(2, handle, FALSE, INFINITE);

        if (result == WAIT_OBJECT_0)
        {

            if (self->UpdateFileAttributes())
            {
        
                // Dispatch the message to event handler.
                if (self->m_eventHandler != NULL)
                {
                    FileEvent event(self->m_fileName);
                    self->m_eventHandler->AddPendingEvent(event);
                }

            }

            FindNextChangeNotification(self->m_handle);

        }
        else
        {
            break;
        }

    }

    return 0;

}
