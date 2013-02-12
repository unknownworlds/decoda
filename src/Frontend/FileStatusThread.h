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

#ifndef FILE_STATUS_THREAD_H
#define FILE_STATUS_THREAD_H

#include <wx/wx.h>
#include <wx/thread.h>

#include "SourceControl.h"

//
// Forward declarations.
//

class wxEvtHandler;

/**
 * This class is used to get file status information from source control in
 * the background.
 */
class FileStatusThread : public wxThread
{

public:

    /**
     * Constructor.
     */
    FileStatusThread(wxEvtHandler* eventHandler, SourceControl& sourceControl);

    /**
     * Adds a new file to check the status of. This should be done before the
     * thread is started.
     */
    void AddFileName(const char* fileName);

    /**
     * From wxThread.
     */
    virtual ExitCode Entry();

    /**
     * Returns the number of files that we have the status results for.
     */
    unsigned int GetNumFiles() const;

    /**
     * Returns the name of the ith file we have status for.
     */
    const std::string& GetFileName(unsigned int i) const;

    /**
     * Returns the status of the ith file we have status for.
     */
    SourceControl::Status GetFileStatus(unsigned int i) const;

private:

    wxEvtHandler*                       m_eventHandler;    
    SourceControl&                      m_sourceControl;

    std::vector<std::string>            m_fileNames;
    std::vector<SourceControl::Status>  m_fileStatus;

};

#endif