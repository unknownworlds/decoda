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

#include "SystemSettingsPanel.h"
#include "MainApp.h"

#include <wx/msw/registry.h>

BEGIN_EVENT_TABLE(SystemSettingsPanel, wxPanel)
    EVT_CHECKBOX(       SystemSettingsPanel::ID_CheckForUpdates,    SystemSettingsPanel::OnCheckForUpdates)
END_EVENT_TABLE()

SystemSettingsPanel::SystemSettingsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->AddGrowableRow( 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("File Associations") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer7->AddGrowableCol( 0 );
	fgSizer7->AddGrowableRow( 0 );
	fgSizer7->SetFlexibleDirection( wxVERTICAL );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );

	m_fileTypesBox = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), 0, NULL, 0 ); 
	fgSizer7->Add( m_fileTypesBox, 0, wxALL|wxEXPAND, 5 );

    wxStaticText* staticText5 = new wxStaticText( this, wxID_ANY, wxT("Select the file types on the right that you would like to associate with Decoda in Windows Explorer (double clicking them will open them in Decoda)"), wxDefaultPosition, wxDefaultSize, 0 );
    staticText5->Wrap(200);
    fgSizer7->Add( staticText5, 0, wxALL, 5 );

	sbSizer3->Add( fgSizer7, 1, wxEXPAND | wxALL, 5 );

	fgSizer4->Add( sbSizer3, 0, wxEXPAND | wxALL, 5 );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Updates") ), wxVERTICAL );

	m_checkForUpdates = new wxCheckBox( this, ID_CheckForUpdates, wxT("Automatically check for updates to Decoda"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_checkForUpdates, 0, wxALL, 5 );
    fgSizer4->Add (sbSizer4, 0, wxEXPAND | wxALL, 5);
	
	this->SetSizer( fgSizer4 );
	this->Layout();

}
    
void SystemSettingsPanel::Initialize()
{

    // Turn off the ugly error messages if we can't access the registry.
    wxLogNull logNo;

    // Check the current settings for the associations.

    wxString openCommand = GetOpenCommand();

    for (unsigned int i = 0; i < m_fileTypes.size(); ++i)
    {
        bool registered = GetIsRegistered(m_fileTypes[i].ext, openCommand);
        m_fileTypesBox->Check(i, registered);
    }

    m_checkForUpdates->SetValue(m_settings.GetCheckForUpdates());

}

void SystemSettingsPanel::AddFileType(const wxString& ext, int icon)
{

    FileType fileType;

    fileType.ext = ext;
    fileType.icon = icon;

    m_fileTypesBox->Append(ext);
    m_fileTypes.push_back(fileType);

}

wxString SystemSettingsPanel::GetOpenCommand() const
{
    return wxString::Format("%s \"%%1\"", wxGetApp().GetExeFileName());
}

void SystemSettingsPanel::ApplyFileAssociations()
{

    // Turn off the ugly error messages if we can't access the registry.
    wxLogNull logNo;

    wxString openCommand = GetOpenCommand();

    for (unsigned int i = 0; i < m_fileTypes.size(); ++i)
    {
        if (m_fileTypesBox->IsChecked(i) != GetIsRegistered(m_fileTypes[i].ext, openCommand))
        {

            wxString docType = "Decoda" + m_fileTypes[i].ext;

            wxRegKey extKey("HKEY_CLASSES_ROOT\\" + m_fileTypes[i].ext);
            
            wxRegKey iconKey("HKEY_CLASSES_ROOT\\" + docType + "\\DefaultIcon"); 
            wxRegKey shellKey("HKEY_CLASSES_ROOT\\" + docType + "\\shell\\open\\command"); 

            bool error = false;

            // Test if we have permission. A bit of a hacky way to do it, but
            // wxWidgets doesn't provide a better method.
            if (extKey.Exists())
            {
                extKey.DeleteSelf();
                error = extKey.Exists();
            }
            else
            {
                extKey.DeleteSelf();            
            }

            iconKey.DeleteSelf();
            shellKey.DeleteSelf();

            if (m_fileTypesBox->IsChecked(i))
            {

                extKey.Create();
                error = !extKey.SetValue("", docType);

                iconKey.Create();
                iconKey.SetValue("", wxString::Format("%s,%d", wxGetApp().GetExeFileName(), m_fileTypes[i].icon));

                shellKey.Create();
                shellKey.SetValue("", openCommand);

            }

            if (error)
            {
                wxMessageBox("This account has insufficient privileges to update the registry, which is necessary to set file associations.");
                break;
            }

        }
    }

}

void SystemSettingsPanel::SetSettings(const SystemSettings& settings)
{
    m_settings = settings;
}

const SystemSettings& SystemSettingsPanel::GetSettings() const
{
    return m_settings;
}

void SystemSettingsPanel::OnCheckForUpdates(wxCommandEvent& event)
{
    m_settings.SetCheckForUpdates(m_checkForUpdates->GetValue());
}

bool SystemSettingsPanel::GetIsRegistered(const wxString& ext, const wxString& openCommand) const
{

    // Turn off the ugly error messages if we can't access the registry.
    wxLogNull logNo;

    wxRegKey extKey("HKEY_CLASSES_ROOT\\" + ext);
    
    if (extKey.Exists())
    {

        wxString docType = extKey.QueryDefaultValue();

        if (docType == "Decoda" + ext)
        {

            wxRegKey docKey("HKEY_CLASSES_ROOT\\" + docType + "\\shell\\open\\command"); 
        
            if (docKey.Exists())
            {
                if (docKey.QueryDefaultValue() == openCommand)
                {
                    return true;
                }
            }
        }

    }

    return false;

}