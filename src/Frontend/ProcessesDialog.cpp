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

#include "ProcessesDialog.h"

#include <wx/listctrl.h>
#include <wx/filename.h>

BEGIN_EVENT_TABLE(ProcessesDialog, wxDialog)
    EVT_BUTTON(ID_Refresh,              OnRefresh)
    EVT_BUTTON(wxID_CANCEL,             OnCancel)
    EVT_BUTTON(wxID_OK,                 OnAttach)
    EVT_LIST_ITEM_SELECTED(wxID_ANY,    OnProcessSelected)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY,  OnProcessDeselected)
    EVT_LIST_COL_CLICK(wxID_ANY,        OnProcessColumnClicked)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY,   OnProcessActivated)
END_EVENT_TABLE()

ProcessesDialog::ProcessesDialog( wxWindow* parent )
    : wxDialog( parent, wxID_ANY, _("Processes"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Available Processes") ), wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	
	m_processList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize(400, 300), wxLC_REPORT | wxLC_SINGLE_SEL);
    m_processList->InsertColumn(0, "Process");
    m_processList->InsertColumn(1, "ID");
    m_processList->InsertColumn(2, "Title");	

    m_processList->SetColumnWidth(0, 100);
    m_processList->SetColumnWidth(1, 50);
    m_processList->SetColumnWidth(2, 350);

    fgSizer1->Add( m_processList, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_refresh = new wxButton( this, ID_Refresh, wxT("&Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_refresh, 0, wxALL, 5 );
	
	m_attach = new wxButton( this, wxID_OK, wxT("&Attach"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_attach, 0, wxALL, 5 );
	
	m_close = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_close, 0, wxALL, 5 );
	
	fgSizer2->Add( bSizer1, 1, wxALL | wxALIGN_RIGHT, 5 );
	
	SetSizer( fgSizer2 );
	
    wxSize minSize = fgSizer2->GetMinSize();
    SetClientSize(minSize);
    SetMinSize(GetSize());
    
    Layout();

    m_sortColumn = 0;
    m_sortForward = true;

    Refresh();

}

void ProcessesDialog::Refresh()
{

    m_processId = 0;
    m_processes.clear();
    DebugFrontend::Get().GetProcesses(m_processes);

    m_processList->DeleteAllItems();

    for (unsigned int i = 0; i < m_processes.size(); ++i)
    {
        wxFileName fileName(m_processes[i].name.c_str());
        m_processList->InsertItem(i, fileName.GetFullName());
        m_processList->SetItem(i, 1, wxString::Format("%d", m_processes[i].id));
        m_processList->SetItem(i, 2, m_processes[i].title.c_str());
        m_processList->SetItemData(i, i);
    }

    SortColumns();
    UpdateAttachStatus();

}

void ProcessesDialog::OnRefresh(wxCommandEvent& event)
{
    Refresh();
}

void ProcessesDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void ProcessesDialog::OnAttach(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void ProcessesDialog::OnProcessSelected(wxListEvent& event)
{
    if (static_cast<unsigned int>(event.GetIndex()) < m_processes.size())
    {
        long index = m_processList->GetItemData(event.GetIndex());
        m_processId = m_processes[index].id;
    }
    UpdateAttachStatus();
}

void ProcessesDialog::OnProcessDeselected(wxListEvent& event)
{
    m_processId = 0;
    UpdateAttachStatus();
}

void ProcessesDialog::OnProcessColumnClicked(wxListEvent& event)
{

    int column = event.GetColumn();

    if (column == m_sortColumn)
    {
        m_sortForward = !m_sortForward;
    }

    m_sortColumn = column;
    SortColumns(); 

}
   
void ProcessesDialog::OnProcessActivated(wxListEvent& event)
{
    if (static_cast<unsigned int>(event.GetIndex()) < m_processes.size())
    {
        long index = m_processList->GetItemData(event.GetIndex());
        m_processId = m_processes[index].id;
        EndModal(wxID_OK);
    }
}

unsigned int ProcessesDialog::GetProcessId() const
{
    return m_processId;
}

void ProcessesDialog::SelectProcessByModule(const wxString& name)
{

    wxFileName fileName(name);
    wxString exeName = fileName.GetFullName();

    for (long i = 0; i < m_processList->GetItemCount(); ++i)
    {
        if (m_processList->GetItemText(i) == exeName)
        {

            wxListItem item;
            
            item.SetId(i);
            item.SetMask(wxLIST_MASK_STATE);
            item.SetState(wxLIST_STATE_SELECTED);

            m_processList->SetItem(item);
            m_processList->EnsureVisible(i);
            
            break;
            
        }
    }

}

void ProcessesDialog::UpdateAttachStatus()
{
    m_attach->Enable(m_processId != 0);
}

void ProcessesDialog::SortColumns()
{
    m_processList->SortItems(SortCompareFunction, reinterpret_cast<long>(this));
}

int ProcessesDialog::SortCompareFunction(long item1, long item2, long sortData)
{
    
    ProcessesDialog* self = reinterpret_cast<ProcessesDialog*>(sortData);

    int result = 0;

    if (self->m_sortColumn == 0)
    {
        result = strcmp(self->m_processes[item1].name.c_str(), self->m_processes[item2].name.c_str());
    }
    else if (self->m_sortColumn == 1)
    {
        if (self->m_processes[item1].id < self->m_processes[item2].id)
        {
            result = -1;
        }
        else if (self->m_processes[item1].id > self->m_processes[item2].id)
        {
            result = 1;
        }
    }
    else if (self->m_sortColumn == 2)
    {
        result = strcmp(self->m_processes[item1].title.c_str(), self->m_processes[item2].title.c_str());
    }

    if (!self->m_sortForward)
    {
        result = -result;
    }

    return result;

}