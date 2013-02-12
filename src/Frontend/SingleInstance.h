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

#ifndef SINGLE_INSTANCE_H
#define SINGLE_INSTANCE_H

#include <wx/wx.h>
#include <windows.h>

/**
 * This class is used to ensure that only a single instance of the application
 * is created. It also allows passing of a command line to the currently running
 * instance.
 */
class SingleInstance
{

public:

    /**
     * Constructor.
     */
    SingleInstance();

    /**
     * Destructor.
     */
    ~SingleInstance();

    /**
     * Returns the window handle to any previous instance of the application that's
     * running. If there's no previous instance, the method returns NULL. The name
     * is a unique identifier that describes the application.
     */
    HWND Connect(HWND hWnd, const char* name);

    /**
     * Sends the specified command line to the instance.
     */
    void SetCommand(const wxString& commandLine);

    /**
     * Returns the command line.
     */
    wxString GetCommand() const;

private:

    static const int s_maxCommandLineLength = 2000;

    struct Data
    {
        HWND    hWnd;
        char    commandLine[s_maxCommandLineLength];
    };

    HANDLE      m_hInstance;
    HANDLE      m_mutex;
    Data*       m_data;

};

#endif