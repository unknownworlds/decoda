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


#include "Config.h"

#ifdef DEDICATED_PRODUCT_VERSION

// The implementations of these functions should be changed to match your own
// project so that Decoda will automatically launch your executable, etc.

#include <wx/msw/registry.h>
#include <wx/filename.h>

wxString GetExecutablePath();

wxString GetDedicatedProductName()
{
    return wxT("Spark");
}

wxString GetDedicatedProductCommand()
{
    // Use the EXE in the same directory as Decoda.
    return GetExecutablePath() + wxFileName::GetPathSeparator() + wxT("ns2.exe");
}

wxString GetDedicatedProductCommandArguments(const wxString& arguments)
{

    wxLogNull log;
    wxRegKey regKey(wxT("HKEY_CURRENT_USER\\Software\\Spark Engine"));

    wxString gameDirectory;

    if (regKey.Exists())
    {
        regKey.QueryValue(wxT("Game Asset Directory"), gameDirectory);
    }

    wxString result;

    if (!gameDirectory.IsEmpty())
    {
        result = wxT("-game \"") + gameDirectory + wxT("\"");
    }
    
    if (!arguments.IsEmpty())
    {
        result += wxT(" ") + arguments;
    }

    return result;

}

#endif