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

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <wx/wx.h>
#include <wx/propdlg.h> 

//
// Forward declarations.
//

class KeyBinderDialog;
class FontColorSettingsPanel;
class EditorSettingsPanel;
class SystemSettingsPanel;

/**
 * Settings dialog class.
 */
class SettingsDialog : public wxPropertySheetDialog
{

public:

    /**
     * Constructor.
     */
    explicit SettingsDialog(wxWindow* parent);

    /**
     * Called when the dialog is initialized.
     */
    void OnInitDialog(wxInitDialogEvent& event);

    /**
     * Called when the user activates the content help either by hitting the ? button
     * or pressing F1.
     */
    void OnHelp(wxHelpEvent& event);

    /**
     * Returns the key binder part of the settings dialog.
     */
    KeyBinderDialog* GetKeyBinderDialog() const;

    /**
     * Returns the font and color part of the settings dialog.
     */
    FontColorSettingsPanel* GetFontColorSettingsPanel() const;

    /**
     * Returns the editor part of the settings dialog.
     */
    EditorSettingsPanel* GetEditorSettingsPanel() const;

    /**
     * Returns the systems part of the settings dialog.
     */
    SystemSettingsPanel* GetSystemSettingsPanel() const;

    DECLARE_EVENT_TABLE()

private:

    KeyBinderDialog*            m_keyBinderDialog;
    FontColorSettingsPanel*     m_fontColorSettingsPanel;
    EditorSettingsPanel*        m_editorSettingsPanel;
    SystemSettingsPanel*        m_systemSettingsPanel;

};

#endif