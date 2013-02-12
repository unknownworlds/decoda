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

#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <wx/wx.h>

#include "SingleInstance.h"

class MainApp: public wxApp
{

public:

    static const unsigned int  s_buildNumber;
    static const char*         s_versionDesc;

    /**
     * Constructor.
     */
    MainApp();

    /**
     * From wxApp.
     */
    virtual bool OnInit();

    /**
     *  From wxApp.
     */
    virtual void OnInitCmdLine(wxCmdLineParser& parser);

    /**
     * From wxApp.
     */
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    /**
     * Returns the name of the executable with the full path.
     */
    const char* GetExeFileName() const;

    /**
     * Returns the command line passed to the instance.
     */
    wxString GetInstanceCommandLine() const;

private:

    wxString        m_loadProjectName;
    wxArrayString   m_loadFileNames;
    wxString        m_debugExe;         // Image specified with /debugexe

    SingleInstance  m_singleInstance;

};

DECLARE_APP(MainApp)

#endif