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

#ifndef FILE_CHANGE_WATCHER_H
#define FILE_CHANGE_WATCHER_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <windows.h>

/**
 * Monitors a file to detect changes to it made by another application.
 */
class FileChangeWatcher
{

public:

    /**
     * Constructor.
     */
    FileChangeWatcher();

    /**
     * Destructor.
     */
    virtual ~FileChangeWatcher();

    /**
     * Shuts down.
     */
    void Shutdown();

    /**
     * Sets the event handler that receives notification when a file event
     * occurs.
     */
    void SetEventHandler(wxEvtHandler* eventHandler);
    
    /**
     * Sets the file to be tracked.
     */
    void SetFile(const wxFileName& fileName);

private:

    static DWORD WINAPI ThreadProc(LPVOID param);

    /**
     * Updates the stored attributes for the file. If they've changed
     * the method returns true;
     */
    bool UpdateFileAttributes();

    /**
     * Ends the thread.
     */
    void EndThread();

private:

    HANDLE          m_handle;
    HANDLE          m_thread;
    HANDLE          m_threadEndEvent;
    
    wxFileName      m_fileName;
    DWORD           m_fileAttributes;

    wxEvtHandler*   m_eventHandler;

};

#endif