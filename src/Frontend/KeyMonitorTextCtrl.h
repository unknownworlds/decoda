/*

Decoda
Copyright (C) 2007-2013 Unknown Worlds Entertainment, Inc. 

Parts adapted from the wxKeyBinder library
Copyright (C) Aleksandras Gluchovas

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

#ifndef KEY_MONITOR_TEXT_CTRL_H
#define KEY_MONITOR_TEXT_CTRL_H

#include <wx/wx.h>

//! A special text control where the user keypresses are displayed
//! as the contained text.
//! This class is used by wxKeyConfigPanel class to allow the user
//! to input a shortcut key which can be associated with a wxCmd.
//!
//! Example: the user presses CTRL+SHIFT+A inside this window;
//!          the wxKeyMonitorTextCtrl displays exactly the string
//!          "Ctrl+Shift+A"
//!
class KeyMonitorTextCtrl : public wxTextCtrl
{
public:
    KeyMonitorTextCtrl(
        wxWindow* parent,
        wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTE_PROCESS_ENTER,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxTextCtrlNameStr) :
        wxTextCtrl(parent, id, value, pos, size, style, validator, name) {}

    virtual ~KeyMonitorTextCtrl() {}

public:

    //! Handles the keypresses and changes accordingly
    //! the text displayed in text ctrl.
    void OnKey(wxKeyEvent &);

    //! Returns TRUE if this window is containing a valid key combination.
    bool IsValidKeyComb() const {
        return !GetValue().IsEmpty() && GetValue().Last() != '+';
    }

private:
    DECLARE_CLASS(wxKeyMonitorTextCtrl)
    DECLARE_EVENT_TABLE()
};

#endif