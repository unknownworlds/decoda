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

#include "ExternalTool.h"
#include "MainFrame.h"

#include "wx/process.h"

ExternalTool::ExternalTool()
{
}

void ExternalTool::SetTitle(const wxString& title)
{
    m_title = title;
}

const wxString& ExternalTool::GetTitle() const
{
    return m_title;
}

void ExternalTool::SetCommand(const wxString& command)
{
    m_command = command;
}

const wxString& ExternalTool::GetCommand() const
{
    return m_command;
}

void ExternalTool::SetInitialDirectory(const wxString& initialDirectory)
{
    m_initialDirectory = initialDirectory;
}

const wxString& ExternalTool::GetInitialDirectory() const
{
    return m_initialDirectory;
}

void ExternalTool::SetArguments(const wxString& arguments)
{
    m_arguments = arguments;
}

const wxString& ExternalTool::GetArguments() const
{
    return m_arguments;
}

void ExternalTool::SetEvent(const wxString& setEvent)
{
    m_event = setEvent;
}

const wxString& ExternalTool::GetEvent() const
{
    return m_event;
}

void ExternalTool::SetKeys(const std::vector<KeyBinder::Key>& keys)
{
    m_keys = keys;
}

const std::vector<KeyBinder::Key>& ExternalTool::GetKeys() const
{
    return m_keys;
}

wxProcess* ExternalTool::Run(const MainFrame* mainFrame) const
{
        
    // The caller will display their own error message if they want, so display the
    // automatic one called from wxProcess::Open.
    wxLogNull logNo;

    wxString commandLine      = m_command + " " + m_arguments;
    wxString initialDirectory = m_initialDirectory;
    mainFrame->SubstituteVariableArguments(commandLine     );
    mainFrame->SubstituteVariableArguments(initialDirectory);
    
    wxString workDirOriginal;
    if (!initialDirectory.IsEmpty())
    {
        workDirOriginal = wxGetCwd();
        wxSetWorkingDirectory(initialDirectory);
    }

    wxProcess* process = wxProcess::Open(commandLine);

    if (!initialDirectory.IsEmpty())
    {
        wxSetWorkingDirectory(workDirOriginal);
    }

    return process;

}