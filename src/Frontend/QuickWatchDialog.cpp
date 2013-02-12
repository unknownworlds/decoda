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

#include "QuickWatchDialog.h"
#include "DebugFrontend.h"
#include "WatchWindow.h"
#include "MainFrame.h"

#include <wx/event.h>

BEGIN_EVENT_TABLE(QuickWatchDialog, wxDialog)
    EVT_BUTTON(ID_Recalculate,          OnRecalculate)
    EVT_BUTTON(ID_AddWatch,             OnAddWatch)
    EVT_TEXT_ENTER(ID_ExpressionBox,    OnExpressionBoxTextEnter)
END_EVENT_TABLE()

QuickWatchDialog::QuickWatchDialog(wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
    : wxDialog( parent, id, title, pos, size, style )
{

    SetMinSize(size);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->SetFlexibleDirection( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Expression:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText1, 0, wxALL, 5 );
	
	m_expressionBox = new wxTextCtrl( this, ID_ExpressionBox, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	fgSizer3->Add( m_expressionBox, 0, wxALL | wxEXPAND, 5 );
	
	fgSizer2->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_recalculateButton = new wxButton( this, ID_Recalculate, wxT("Recalculate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_recalculateButton, 0, wxALL, 5 );
	
	m_addWatchButton = new wxButton( this, ID_AddWatch, wxT("Add Watch"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_addWatchButton, 0, wxALL, 5 );
	
	m_closeButton = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_closeButton, 0, wxALL, 5 );
	
	fgSizer2->Add( bSizer2, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxALL|wxEXPAND, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 1, 1, 0, 0 );
	
	m_resultBox = new WatchCtrl( this, wxID_ANY );
	gSizer1->Add( m_resultBox, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( gSizer1, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();

    m_watchWindow   = NULL;
    m_mainFrame     = NULL;

    m_root = m_resultBox->AddRoot(_T("Root"));

}

void QuickWatchDialog::SetWatchWindow(WatchWindow* watchWindow, MainFrame* mainFrame)
{
    m_watchWindow = watchWindow;
    m_mainFrame   = mainFrame;
}

void QuickWatchDialog::SetContext(unsigned int vm, unsigned int stackLevel)
{
    m_resultBox->SetContext(vm, stackLevel);
}

void QuickWatchDialog::SetExpression(const wxString& expression)
{
    m_expressionBox->SetValue(expression);
    Recalculate();
}

void QuickWatchDialog::OnRecalculate(wxCommandEvent& event)
{
    Recalculate();
}

void QuickWatchDialog::OnAddWatch(wxCommandEvent& event)
{
    if (m_watchWindow != NULL)
    {
        wxString expression = m_expressionBox->GetValue();
        m_watchWindow->AddWatch(expression);
    }
    if (m_mainFrame != NULL)
    {
        m_mainFrame->ShowWatchWindow();
    }
}

void QuickWatchDialog::OnExpressionBoxTextEnter(wxCommandEvent& event)
{
    Recalculate();
}

void QuickWatchDialog::Recalculate()
{

    wxString expression = m_expressionBox->GetValue();
    expression.Trim().Trim(false);

    if (!expression.empty())
    {
        
        // Remove any old watch.
        m_resultBox->DeleteChildren(m_root);

        // Add the new watch value.
        wxTreeItemId item = m_resultBox->AppendItem(m_root, expression);
        m_resultBox->UpdateItem(item);

    }

}