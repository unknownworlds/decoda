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

#include "ListWindow.h"
#include "MainFrame.h"
#include "Bitmaps.h"

BEGIN_EVENT_TABLE(ListWindow, ListView)
    EVT_KILL_FOCUS(                         ListWindow::OnKillFocus)
END_EVENT_TABLE()


ListWindow::ListWindow(MainFrame* mainFrame, wxWindowID winid)
    : ListView(mainFrame, winid, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL)
{

    SetAllowColumnResizing(false);

    // Add the column that will contain the "active" icon.
    InsertColumn(0, _(""), wxLIST_FORMAT_CENTER, 15);

    // Add the column that will contain the name.
    InsertColumn(1, _("Name"));

    // Set the margin icons.

    wxImageList* imageList = new wxImageList(11, 11);

    imageList->Add( wxMEMORY_BITMAP(Breakline_png) );
    imageList->Add( wxMEMORY_BITMAP(Currentline_png) );

    AssignImageList(imageList, wxIMAGE_LIST_SMALL);
}

void ListWindow::SetFontColorSettings(const FontColorSettings& settings)
{
  SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Window).backColor);
  SetTextColour(settings.GetColors(FontColorSettings::DisplayItem_Window).foreColor);
}

void ListWindow::Append(const wxString& name)
{
    long item = InsertItem(GetItemCount(), -1);
    SetItem(item, 1, name);
}

void ListWindow::SetString(long item, const wxString& name)
{
    SetItem(item, 1, name);
}

void ListWindow::SetItemIcon(long item, Icon icon)
{
    SetItemColumnImage(item, 0, static_cast<int>(icon));
}

void ListWindow::ClearAllIcons()
{

    long numItems = GetItemCount();

    for (long i = 0; i < numItems; ++i)
    {
        SetItemColumnImage(i, 0, -1);
    }

}

void ListWindow::OnKillFocus(wxFocusEvent& event)
{
    ClearSelection();
    event.Skip();
}

void ListWindow::ClearSelection()
{

    long item = GetFirstSelected();

    while (item != -1)
    {
        Select(item, false);
        item = GetNextSelected(item);
    }

}