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

#include "FileStatusThread.h"
#include "ThreadEvent.h"

#include <algorithm>

FileStatusThread::FileStatusThread(wxEvtHandler* eventHandler, SourceControl& sourceControl) 
    : wxThread(wxTHREAD_JOINABLE), m_eventHandler(eventHandler), m_sourceControl(sourceControl)
{
}

void FileStatusThread::AddFileName(const char* fileName)
{
    m_fileNames.push_back(fileName);
}

wxThread::ExitCode FileStatusThread::Entry()
{

    // Remove duplicates from the file list.
    std::sort(m_fileNames.begin(), m_fileNames.end());
    m_fileNames.erase( std::unique( m_fileNames.begin(), m_fileNames.end() ), m_fileNames.end() );

    if (!m_sourceControl.GetFilesStatus(m_fileNames, m_fileStatus))
    {
        // We didn't successfully get status information, so clear out
        // the file names to indicate that the status isn't available.
        m_fileNames.clear();
    }

    // Send the exit event message to the UI.
    if (m_eventHandler != NULL)
    {
        ThreadEvent event;
        m_eventHandler->AddPendingEvent(event);        
    }

    return 0;

}

unsigned int FileStatusThread::GetNumFiles() const
{
    return m_fileNames.size();
}

const std::string& FileStatusThread::GetFileName(unsigned int i) const
{
    return m_fileNames[i];
}

SourceControl::Status FileStatusThread::GetFileStatus(unsigned int i) const
{
    return m_fileStatus[i];
}