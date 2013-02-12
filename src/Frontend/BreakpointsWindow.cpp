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

#include "BreakpointsWindow.h"
#include "MainFrame.h"
#include "Bitmaps.h"
#include "ListView.h"
#include "ButtonBar.h"

#include <algorithm>

BEGIN_EVENT_TABLE( BreakpointsWindow, wxPanel )

    EVT_LIST_DELETE_ITEM(       wxID_ANY,                   BreakpointsWindow::OnDeleteItem )
    EVT_LIST_DELETE_ALL_ITEMS(  wxID_ANY,                   BreakpointsWindow::OnDeleteAllItems )
    EVT_LIST_ITEM_ACTIVATED(    wxID_ANY,                   BreakpointsWindow::OnItemActivated )
    EVT_LIST_ITEM_SELECTED(     wxID_ANY,                   BreakpointsWindow::OnItemSelected )
    EVT_LIST_ITEM_DESELECTED(   wxID_ANY,                   BreakpointsWindow::OnItemDeselected )
    
    EVT_BUTTON(                 ID_DeleteBreakpoint,        BreakpointsWindow::OnDeleteBreakpoint )
    EVT_BUTTON(                 ID_DeleteAllBreakpoints,    BreakpointsWindow::OnDeleteAllBreakpoints )
    EVT_BUTTON(                 ID_GoToSourceCode,          BreakpointsWindow::OnGoToSourceCode )

END_EVENT_TABLE()

BreakpointsWindow::BreakpointsWindow(MainFrame* mainFrame, wxWindowID winid)
    : wxPanel(mainFrame, winid)
{

    // Create the tool bar.

    m_buttonBar = new ButtonBar(this, wxID_ANY);
    m_buttonBar->SetToolBitmapSize( wxSize(22, 22) );

    wxBitmap deleteBreakpointBitmap     = wxMEMORY_BITMAP(DeleteBreakpoint_png);
    wxBitmap deleteAllBreakpointsBitmap = wxMEMORY_BITMAP(DeleteAllBreakpoints_png);
    wxBitmap goToSourceCodeBitmap       = wxMEMORY_BITMAP(GoToSourceCode_png);

    m_buttonBar->AddTool( ID_DeleteBreakpoint,      _("Delete Breakpoint"), deleteBreakpointBitmap, deleteBreakpointBitmap );
    m_buttonBar->AddTool( ID_DeleteAllBreakpoints,  _("Delete All Breakpoints"), deleteAllBreakpointsBitmap, deleteAllBreakpointsBitmap );
    m_buttonBar->AddTool( ID_GoToSourceCode,        _("Go To Source Code"), goToSourceCodeBitmap, goToSourceCodeBitmap );

    // Create the list view used to show the breakpoints.

    m_breakpointList = new ListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_breakpointList->SetAllowColumnResizing(false);

    m_mainFrame = mainFrame;

    // Add the column that will contain the check box.
    m_breakpointList->InsertColumn(0, _(""), wxLIST_FORMAT_CENTER, 20);

    // Add the column that will contain the name.
    m_breakpointList->InsertColumn(1, _("Name"));

    // Set the icons.

    wxBitmap breakpointBitmap = wxMEMORY_BITMAP(Breakpoint_png);

    wxImageList* imageList = new wxImageList(13, 13);
    imageList->Add(breakpointBitmap);

    m_breakpointList->AssignImageList(imageList, wxIMAGE_LIST_SMALL);    
    
    m_project = NULL;

    // Setup the layout.

	wxFlexGridSizer* fgSizer1;

	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer1->Add( m_buttonBar, 0, wxALL|wxEXPAND, 3 );
	fgSizer1->Add( m_breakpointList, 0, wxALL|wxEXPAND, 0 );
	
	SetSizer( fgSizer1 );
	Layout();

}

void BreakpointsWindow::SetProject(Project* project)
{
    m_project = project;
    UpdateBreakpoints();
}

void BreakpointsWindow::RemoveFile(Project::File* file)
{
    UpdateBreakpoints();
}

void BreakpointsWindow::UpdateBreakpoints()
{

    m_breakpointList->Freeze();
    m_breakpointList->DeleteAllItems();

    if (m_project != NULL)
    {
        for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
        {
            AddBreakpointsForFile(m_project->GetFile(i));
        }
    }

    m_breakpointList->Thaw();

    UpdateToolBarStatus();

}

void BreakpointsWindow::UpdateBreakpoints(Project::File* file)
{
    // For now, just update all of the breakpoints. We have this interface in here
    // so that in the future we can update more selectively if necessary for performance.
    UpdateBreakpoints();
}

void BreakpointsWindow::AddBreakpointsForFile(Project::File* file)
{

    std::vector<unsigned int> breakpoints = file->breakpoints;
    std::sort(breakpoints.begin(), breakpoints.end());

    for (unsigned int breakpointIndex = 0; breakpointIndex < breakpoints.size(); ++breakpointIndex)
    {
        
        unsigned int line = breakpoints[breakpointIndex];

        long item = m_breakpointList->InsertItem(m_breakpointList->GetItemCount(), -1);

        m_breakpointList->SetItemColumnImage(item, 0, 0);

        wxString name;
        name.Printf( wxT("%s, line %d"), file->GetDisplayName(), line + 1 );

        m_breakpointList->SetItem(item, 1, name);

        Breakpoint* breakpoint = new Breakpoint;
        breakpoint->file = file;
        breakpoint->line = breakpoints[breakpointIndex];
        
        m_breakpointList->SetItemPtrData(item, reinterpret_cast<wxUIntPtr>(breakpoint));

    }

}

void BreakpointsWindow::OnDeleteItem(wxListEvent& event)
{
    Breakpoint* breakpoint = reinterpret_cast<Breakpoint*>(m_breakpointList->GetItemData(event.GetIndex())); 
    delete breakpoint;
}

void BreakpointsWindow::OnDeleteAllItems(wxListEvent& event)
{
    for (int itemIndex = 0; itemIndex < m_breakpointList->GetItemCount(); ++itemIndex)
    {
        Breakpoint* breakpoint = reinterpret_cast<Breakpoint*>(m_breakpointList->GetItemData(itemIndex)); 
        delete breakpoint;
    }
}

void BreakpointsWindow::OnItemActivated(wxListEvent& event)
{
    Breakpoint* breakpoint = reinterpret_cast<Breakpoint*>(m_breakpointList->GetItemData(event.GetIndex())); 
    if (breakpoint != NULL)
    {
        m_mainFrame->GotoNewLine( breakpoint->file, breakpoint->line );
    }
}

void BreakpointsWindow::OnItemSelected(wxListEvent& event)
{
    UpdateToolBarStatus();
}

void BreakpointsWindow::OnItemDeselected(wxListEvent& event)
{
    UpdateToolBarStatus();
}

void BreakpointsWindow::OnDeleteBreakpoint(wxCommandEvent& event)
{

    long index = m_breakpointList->GetFirstSelected();

    if (index != -1)
    {

        Breakpoint* breakpoint = reinterpret_cast<Breakpoint*>(m_breakpointList->GetItemData(index)); 
        if (breakpoint != NULL)
        {
            m_mainFrame->ToggleBreakpoint( breakpoint->file, breakpoint->line );
        }
    
    }

}

void BreakpointsWindow::OnDeleteAllBreakpoints(wxCommandEvent& event)
{
    m_mainFrame->DeleteAllBreakpoints();
}

void BreakpointsWindow::OnGoToSourceCode(wxCommandEvent& event)
{

    long index = m_breakpointList->GetFirstSelected();

    if (index != -1)
    {

        Breakpoint* breakpoint = reinterpret_cast<Breakpoint*>(m_breakpointList->GetItemData(index)); 
        if (breakpoint != NULL)
        {
            m_mainFrame->GotoNewLine( breakpoint->file, breakpoint->line );
        }
    
    }

}

void BreakpointsWindow::UpdateToolBarStatus()
{
    
    bool selection = m_breakpointList->GetFirstSelected() != -1;

    m_buttonBar->Freeze();
    m_buttonBar->SetToolEnabled( ID_GoToSourceCode, selection );
    m_buttonBar->SetToolEnabled( ID_DeleteBreakpoint, selection );
    m_buttonBar->SetToolEnabled( ID_DeleteAllBreakpoints, m_breakpointList->GetItemCount() > 0 );
    m_buttonBar->Thaw();

}