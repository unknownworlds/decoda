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

#include "NewFileDialog.h"
#include "res/fileicons.xpm"

#include <wx/imaglist.h>

BEGIN_EVENT_TABLE(NewFileDialog, wxDialog)
    EVT_BUTTON(         NewFileDialog::ID_Browse,   NewFileDialog::OnBrowse)
    EVT_TEXT_ENTER(     NewFileDialog::ID_FileName, NewFileDialog::OnFileNameTextEnter)
    EVT_BUTTON(         wxID_OK,                    NewFileDialog::OnOk)
END_EVENT_TABLE()

NewFileDialog::NewFileDialog( wxWindow* parent, bool enableSourceControl )
    : wxDialog( parent, wxID_ANY, wxT("Add New File"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{

    wxGridSizer* sizer = new wxGridSizer(1, 1);
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
    
	m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_SINGLE_SEL | wxLC_ICON );
	fgSizer1->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_fileNameCtrl = new wxTextCtrl( this, ID_FileName, wxEmptyString, wxDefaultPosition, wxSize(350, -1), wxTE_PROCESS_ENTER );
	fgSizer2->Add( m_fileNameCtrl, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_staticText2, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->SetFlexibleDirection( wxHORIZONTAL );
	
	m_pathCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_pathCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_browseButton = new wxButton( this, ID_Browse, wxT("&Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_browseButton, 0, wxALL, 5 );
	
	fgSizer2->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline3, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->SetFlexibleDirection( wxHORIZONTAL );
    
    m_sourceControlCheck = new wxCheckBox( this, wxID_ANY, wxT("Add to Source Control"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer4->Add(m_sourceControlCheck, 0, wxEXPAND, 5);
    

    m_sdbSizer1 = new wxStdDialogButtonSizer();
    
    m_sdbSizer1->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer1->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer1->Realize();
	fgSizer4->Add( m_sdbSizer1, 0, wxALIGN_RIGHT, 5 );


    fgSizer1->Add( fgSizer4, 0, wxEXPAND | wxALL, 5);
	
	sizer->Add(fgSizer1, 0, wxALL | wxEXPAND, 5);
    
    SetSizer( sizer );

    wxSize minSize = sizer->GetMinSize();
    SetClientSize(minSize);

    SetMinSize(GetSize());    
    
    Layout();

    m_sourceControlCheck->Enable(enableSourceControl);

    if (enableSourceControl)
    {
        m_sourceControlCheck->SetValue(true);
    }

    // Setup the list control.

    wxBitmap bitmap(fileicons, wxBITMAP_TYPE_XPM);

    m_imageList = new wxImageList(32, 32);
    m_imageList->Add(bitmap, wxColour(128, 64, 0));

    m_listCtrl->SetImageList(m_imageList, wxIMAGE_LIST_NORMAL);

    m_listCtrl->InsertItem(1, "Lua file (.lua)", 0);
    m_listCtrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    // Setup the file name control.

    m_fileNameCtrl->SetFocus();
    m_fileNameCtrl->SetValue("<Enter name>"); 
    m_fileNameCtrl->SelectAll();

}

NewFileDialog::~NewFileDialog()
{
    delete m_imageList;
    m_imageList = NULL;
}

void NewFileDialog::SetPath(const wxString& path)
{
    m_pathCtrl->SetValue(path);
}

void NewFileDialog::OnBrowse(wxCommandEvent& event)
{

    wxDirDialog dialog(this, "Location", m_pathCtrl->GetValue());
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxFileName path(dialog.GetPath());
        m_pathCtrl->SetValue(path.GetPathWithSep());
    }
    
}

void NewFileDialog::OnFileNameTextEnter(wxCommandEvent& event)
{
    VerifyFileNameAndEnd();
}

void NewFileDialog::OnOk(wxCommandEvent& event)
{
    VerifyFileNameAndEnd();
}

wxFileName NewFileDialog::GetFileName() const
{

    // Grab the file name and path that the user entered.

    wxFileName fileName;
    
    fileName.SetPath(m_pathCtrl->GetValue());
    fileName.SetFullName(m_fileNameCtrl->GetValue());

    // If there isn't an extension, append the extension for the type of file
    // that was selected (only Lua right now).

    if (!fileName.HasExt())
    {
        fileName.SetExt("lua");
    }

    return fileName;

}

bool NewFileDialog::GetIsFileNameValid(const wxFileName& fileName) const
{

    if (!fileName.IsOk())
    {
        return false;
    }

    // Check if the file name has illegal characters in it.

    wxString forbidden = fileName.GetForbiddenChars();

    const wxArrayString dirs = fileName.GetDirs();

    for (unsigned int i = 0; i < dirs.Count(); ++i)
    {
        for (unsigned int j = 0; j < dirs[i].Length(); ++j)
        {
            if (forbidden.Find(dirs[i][j]) != wxNOT_FOUND)
            {
                return false;
            }
        }
    }

    wxString fullName = fileName.GetFullName();

    for (unsigned int j = 0; j < fullName.Length(); ++j)
    {
        if (forbidden.Find(fullName[j]) != wxNOT_FOUND)
        {
            return false;
        }
    }

    return true;

}

void NewFileDialog::VerifyFileNameAndEnd()
{

    // If we have a valid file name, act as if the user pressed ok.

    if (GetIsFileNameValid(GetFileName()))
    {
        EndModal(wxID_OK);
    }
    else
    {
        wxMessageBox("You must enter a valid name", "Decoda", wxOK | wxICON_ERROR, this);
    }

}

bool NewFileDialog::GetAddToSourceContrl() const
{
    return m_sourceControlCheck->GetValue();
}