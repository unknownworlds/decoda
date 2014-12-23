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

#include "ProjectFileInfoCtrl.h"
#include "ToolTipWindow.h"

#include <wx/filename.h>

BEGIN_EVENT_TABLE(ProjectFileInfoCtrl, wxControl)

    EVT_PAINT(OnPaint)
    EVT_ENTER_WINDOW(OnMouseEnterWindow)

END_EVENT_TABLE()

ProjectFileInfoCtrl::ProjectFileInfoCtrl(wxWindow* parent, wxWindowID id)
    : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_STATIC)
{
    
    m_file = NULL;
    m_textIsClipped = false;

    wxWindowDC dc(this);
    dc.SetFont(GetFont());
    
    int y = dc.GetTextExtent("A").y;

    SetClientSize(wxSize(GetClientSize().x, y + s_padding * 2));

    wxSize minSize = wxSize(-1, GetSize().y);
    SetMinSize(minSize);

}

void ProjectFileInfoCtrl::SetFontColorSettings(const FontColorSettings& settings)
{
  m_fontColor = settings.GetColors(FontColorSettings::DisplayItem_Window).foreColor;
}

void ProjectFileInfoCtrl::OnPaint(wxPaintEvent& event)
{

    wxSize clientSize = GetClientSize();

    wxPaintDC dc(this);
    dc.SetFont(GetFont());

    m_textIsClipped = false;

    if (m_file != NULL)
    {

        // Abbreviate the file name so that it fits in the allowed area.

        wxFileName fileName = m_file->fileName;

        int width = s_padding;

        wxString result;

        if (fileName.HasVolume())
        {
            result += fileName.GetVolume() + fileName.GetVolumeSeparator() + fileName.GetPathSeparator(); 
            width += dc.GetTextExtent(result).x;
        }

        const wxArrayString& dirs = fileName.GetDirs();
        width += dc.GetTextExtent(fileName.GetFullName()).x;

        wxString ellipsis = wxString("...") + fileName.GetPathSeparator();
        int ellipsisLength = dc.GetTextExtent(ellipsis).x;
        
        wxString path;

        for (unsigned int i = 0; i < dirs.Count(); ++i)
        {

            wxString temp = dirs[dirs.Count() - i - 1] + fileName.GetPathSeparator();
            width += dc.GetTextExtent(temp).x;

            if (width >= clientSize.x || (i + 1 < dirs.Count() && width + ellipsisLength >= clientSize.x))
            {
                path.Prepend(ellipsis);
                m_textIsClipped = true;
                break;
            }
            else
            {
                path.Prepend(temp);
            }

        }

        result += path + fileName.GetFullName(); 

        dc.SetTextForeground(m_fontColor);
        dc.DrawText(result, s_padding, s_padding);
    
    }

}

void ProjectFileInfoCtrl::OnMouseEnterWindow(wxMouseEvent& event)
{
    if (m_file != NULL && m_textIsClipped)
    {
        ToolTipWindow* toolTip = new ToolTipWindow(this, m_file->fileName.GetFullPath(), ClientToScreen(wxPoint(1, 1)));
        toolTip->SetRegion(GetScreenRect());
    }
}

void ProjectFileInfoCtrl::SetFile(const Project::File* file)
{
    if (m_file != file)
    {
        m_file = file;
        Refresh();
    }
}

const Project::File* ProjectFileInfoCtrl::GetFile() const
{
    return m_file;
}