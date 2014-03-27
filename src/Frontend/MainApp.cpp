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

#include "MainApp.h"
#include "MainFrame.h"
#include "SingleInstance.h"
#include "CrashHandler.h"
#include "Report.h"
#include "Config.h"

#include <sstream>

#include <wx/cmdline.h>

IMPLEMENT_APP(MainApp)

// Like a version number, but used to track updates. This should be incremented
// whenever a new build is released.
const unsigned int  MainApp::s_buildNumber = 1034;
const char*         MainApp::s_versionDesc = "Version 1.17 (Build 1035) Beta 1";

std::string UrlEncode(const std::string& string)
{
    
    std::string result;
    result.reserve(string.length());

    for (unsigned int i = 0; i < string.length(); ++i)
    {
        if (string[i] != '_' && !isalnum(string[i]))
        {
            char buffer[4];
            sprintf(buffer, "%%%02x", static_cast<int>(static_cast<unsigned char>(string[i])));
            result += buffer;
        }
        else
        {
            result += string[i];
        }
    }

    return result;

}

/**
 * Called when the application crashes.
 */
void CrashCallback(void* address)
{

    extern const unsigned int g_buildNumber;

    Report report;
    report.AttachMiniDump();

    if (report.Preview())
    {

        std::stringstream stream;
        stream << "Decoda (Build " << MainApp::s_buildNumber << ")";

        std::string buffer = UrlEncode(stream.str());
        
        char serverAddress[1024];
        sprintf(serverAddress, "http://www.unknownworlds.com/game_scripts/report/crash.php?application=%s&address=0x%08X", buffer.c_str(), reinterpret_cast<unsigned int>(address));
        report.Submit(serverAddress, NULL);
    
    }

}

MainApp::MainApp()
{
    CrashHandler::SetCallback( CrashCallback );
}

bool MainApp::OnInit()
{

    UINT openFilesMessage = RegisterWindowMessage("Decoda_OpenFiles");

    // Check to see if another instances is running.

    if (!wxApp::OnInit())
    {
        return false;
    }

    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxPNGHandler);

    MainFrame* frame = new MainFrame("Script Debugger", openFilesMessage, wxDefaultPosition, wxSize(1024,768));

    HWND hWndPrev = m_singleInstance.Connect(reinterpret_cast<HWND>(frame->GetHandle()), "Decoda");

    // If we're loading files from the command line (but not a project) file,
    // then don't create a new instance. This happens when the user double clicks
    // in Explorer.
    if (hWndPrev != NULL && m_loadProjectName.IsEmpty() && m_debugExe.IsEmpty() && m_loadFileNames.Count() > 0)
    {

        // Send the command line to the other window.

        wxString fileNames;

        for (unsigned int i = 0; i < m_loadFileNames.Count(); ++i)
        {
            fileNames += m_loadFileNames[i] + ";";
        }

        m_singleInstance.SetCommand(fileNames);
        PostMessage(hWndPrev, openFilesMessage, 0, 0);

        // Show the other window.
        SetForegroundWindow(hWndPrev);
        
        delete frame;
        return false;

    }

    frame->ApplyWindowPlacement();

    // If a project was specified, open it.
    if (!m_loadProjectName.IsEmpty())
    {
        frame->OpenProject(m_loadProjectName);
    }

    // Load any files that were specified on the command line.
    frame->OpenDocuments(m_loadFileNames);

    // Tell the UI to load the last project if we haven't instructed.
    // it to load anything else.

    if (m_loadProjectName.IsEmpty() && m_loadFileNames.IsEmpty())
    {
        frame->AutoOpenLastProject();        
    }

    m_loadProjectName.Clear();
    m_loadFileNames.Clear();

    frame->CheckForUpdate();

    if (!m_debugExe.IsEmpty())
    {
        frame->DebugExe(m_debugExe);
    }

    return true;

}

void MainApp::OnInitCmdLine(wxCmdLineParser& parser)
{

    // Command line options:
    //  [project file] [files...] [/debugexe imagename]

    const wxCmdLineEntryDesc cmdLineDesc[] =
        {
            { wxCMD_LINE_PARAM,  NULL,          NULL,       "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
#ifndef DEDICATED_PRODUCT_VERSION
            { wxCMD_LINE_OPTION, "debugexe",    "debugexe", "executable", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
#endif
            { wxCMD_LINE_NONE }
        };
    
    parser.SetDesc(cmdLineDesc);

}

bool MainApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
 
    for (unsigned int i = 0; i < parser.GetParamCount(); ++i)
    {

        wxFileName fileName = parser.GetParam(i);

        // File names sometimes come in in the short form from Windows, so convert
        // to the long form.

        if (fileName.GetExt().Lower() == "deproj")
        {
            // This is a project file, so save the name separately.
            wxFileName fileName = parser.GetParam(i);
            m_loadProjectName = fileName.GetLongPath();
        }
        else
        {
            wxFileName fileName = parser.GetParam(i);
            m_loadFileNames.Add(fileName.GetLongPath());
        }

    }
    
#ifndef DEDICATED_PRODUCT_VERSION
    parser.Found("debugexe", &m_debugExe);
#endif
 
    return true;

}

const char* MainApp::GetExeFileName() const
{
    return argv[0];
}

wxString MainApp::GetInstanceCommandLine() const
{
    return m_singleInstance.GetCommand();
}
