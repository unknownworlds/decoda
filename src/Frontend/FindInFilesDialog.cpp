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

#include "FindInFilesDialog.h"

BEGIN_EVENT_TABLE(FindInFilesDialog, wxDialog)
    EVT_BUTTON(ID_Browse,   OnBrowse)
END_EVENT_TABLE()

FindInFilesDialog::FindInFilesDialog( wxWindow* parent, int id )
    : wxDialog( parent, id,  wxT("Find in Files"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{

	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer18->AddGrowableCol( 0 );
	fgSizer18->SetFlexibleDirection( wxBOTH );
	
	wxFlexGridSizer* fgSizer19;
	fgSizer19 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer19->AddGrowableCol( 1 );
	fgSizer19->SetFlexibleDirection( wxHORIZONTAL );
	
	m_staticText33 = new wxStaticText( this, wxID_ANY, wxT("Find what:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer19->Add( m_staticText33, 0, wxALL, 5 );
	
	m_findWhatBox = new wxComboBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(350, -1), 0, NULL, 0 ); 
	fgSizer19->Add( m_findWhatBox, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText34 = new wxStaticText( this, wxID_ANY, wxT("Look in"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer19->Add( m_staticText34, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer20->AddGrowableCol( 0 );
	fgSizer20->SetFlexibleDirection( wxHORIZONTAL );
	
	m_lookInBox = new wxComboBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer20->Add( m_lookInBox, 0, wxALL|wxEXPAND, 5 );

	m_button14 = new wxButton( this, ID_Browse, wxT("..."), wxDefaultPosition, wxSize( 25,-1 ), 0 );
	fgSizer20->Add( m_button14, 0, wxALL, 5 );
	
	fgSizer19->Add( fgSizer20, 1, wxEXPAND, 5 );
	
	fgSizer19->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_includeSubDirectoriesCheck = new wxCheckBox( this, wxID_ANY, wxT("Include sub-directories"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer19->Add( m_includeSubDirectoriesCheck, 0, wxALL, 5 );
	
	fgSizer18->Add( fgSizer19, 1, wxEXPAND | wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Options") ), wxVERTICAL );
	
	m_matchCaseCheck = new wxCheckBox( this, wxID_ANY, wxT("Match case"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_matchCaseCheck, 0, wxALL, 5 );
	
	m_matchWholeWordCheck = new wxCheckBox( this, wxID_ANY, wxT("Match whole word"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_matchWholeWordCheck, 0, wxALL, 5 );
	
	m_staticText38 = new wxStaticText( this, wxID_ANY, wxT("Look at these file types:"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_staticText38, 0, wxALL, 5 );
	
	m_fileTypesBox = new wxComboBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer6->Add( m_fileTypesBox, 0, wxALL|wxEXPAND, 5 );

    m_fileTypesBox->Append("*.lua");
    m_fileTypesBox->Append("*.*");
    m_fileTypesBox->Select(0);
	
	fgSizer18->Add( sbSizer6, 1, wxEXPAND | wxALL, 5 );

    m_sdbSizer3 = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	fgSizer18->Add( m_sdbSizer3, 0, wxALIGN_RIGHT|wxALL, 5 );
	
    m_findWhatBox->SetFocus();

	SetSizer( fgSizer18 );
    
    wxSize minSize = fgSizer18->GetMinSize();
    SetClientSize(minSize);

    SetMinSize(GetSize());
    SetMaxSize(wxSize(-1, GetSize().y));

    Layout();

}

void FindInFilesDialog::AddLookIn(const wxString& lookIn)
{
    m_lookInBox->Append(lookIn);
    m_lookInBox->Select(0);
}

void FindInFilesDialog::SetText(const wxString& text)
{
    m_findWhatBox->SetValue(text);
    m_findWhatBox->SetSelection(0, text.Length());
}

void FindInFilesDialog::AddText(const wxString& text)
{
    m_findWhatBox->Append(text);
}

void FindInFilesDialog::OnBrowse(wxCommandEvent& event)
{

    wxDirDialog dialog(this, "Look In", m_lookInBox->GetValue());
    
    if (dialog.ShowModal() == wxID_OK)
    {
        
        wxFileName path;
        path.AssignDir(dialog.GetPath());
        
        m_lookInBox->SetValue(path.GetPathWithSep());

    }
    
}

wxString FindInFilesDialog::GetDirectory() const
{
    return m_lookInBox->GetValue();
}

wxString FindInFilesDialog::GetText() const
{
    return m_findWhatBox->GetValue();
}

wxString FindInFilesDialog::GetFileTypes() const
{
    return m_fileTypesBox->GetValue();
}

bool FindInFilesDialog::GetMatchCase() const
{
    return m_matchCaseCheck->GetValue();
}

bool FindInFilesDialog::GetIncludeSubDirectories() const
{
    return m_includeSubDirectoriesCheck->GetValue();
}

bool FindInFilesDialog::GetMatchWholdWord() const
{
    return m_matchWholeWordCheck->GetValue();
}