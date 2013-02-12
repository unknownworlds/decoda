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

#include "SettingsDialog.h"
#include "KeyBinderDialog.h"
#include "FontColorSettingsPanel.h"
#include "EditorSettingsPanel.h"
#include "SystemSettingsPanel.h"
#include "ShowHelpEvent.h"

#include <wx/bookctrl.h>
    
BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
    EVT_INIT_DIALOG(        SettingsDialog::OnInitDialog)
    EVT_HELP(wxID_ANY,                      OnHelp) 
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent)
    : wxPropertySheetDialog(parent, -1, "Settings", wxDefaultPosition, wxSize(450, 450), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) 
{

    SetMinSize(wxSize(450, 450));

    CreateButtons(wxOK | wxCANCEL);
    
    m_keyBinderDialog = new KeyBinderDialog(GetBookCtrl());
    GetBookCtrl()->AddPage(m_keyBinderDialog, wxT("Key Bindings"));

    m_editorSettingsPanel = new EditorSettingsPanel(GetBookCtrl());
    GetBookCtrl()->AddPage(m_editorSettingsPanel, wxT("Editor"));

    m_fontColorSettingsPanel = new FontColorSettingsPanel(GetBookCtrl());
    GetBookCtrl()->AddPage(m_fontColorSettingsPanel, wxT("Font and Colors"));

    m_systemSettingsPanel = new SystemSettingsPanel(GetBookCtrl());
    GetBookCtrl()->AddPage(m_systemSettingsPanel, wxT("System"));

    LayoutDialog();

}

void SettingsDialog::OnInitDialog(wxInitDialogEvent& event)
{
    m_keyBinderDialog->Initialize();
    m_fontColorSettingsPanel->Initialize();
    m_editorSettingsPanel->Initialize();
    m_systemSettingsPanel->Initialize();
}

KeyBinderDialog* SettingsDialog::GetKeyBinderDialog() const
{
    return m_keyBinderDialog;
}

FontColorSettingsPanel* SettingsDialog::GetFontColorSettingsPanel() const
{
    return m_fontColorSettingsPanel;
}

EditorSettingsPanel* SettingsDialog::GetEditorSettingsPanel() const
{
    return m_editorSettingsPanel;
}

SystemSettingsPanel* SettingsDialog::GetSystemSettingsPanel() const
{
    return m_systemSettingsPanel;
}

void SettingsDialog::OnHelp(wxHelpEvent&)
{
    wxCommandEvent event( wxEVT_SHOW_HELP_EVENT, GetId() );
    event.SetEventObject( this );
    event.SetString( wxT("Key Bindings") );
    GetParent()->GetEventHandler()->ProcessEvent( event );
}