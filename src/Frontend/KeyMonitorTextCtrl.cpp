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

#include "KeyMonitorTextCtrl.h"
#include "KeyBinder.h"

IMPLEMENT_CLASS(KeyMonitorTextCtrl, wxTextCtrl)

BEGIN_EVENT_TABLE(KeyMonitorTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN(KeyMonitorTextCtrl::OnKey)
    EVT_KEY_UP(KeyMonitorTextCtrl::OnKey)
END_EVENT_TABLE()

void KeyMonitorTextCtrl::OnKey(wxKeyEvent &event)
{

    // backspace cannot be used as shortcut key...
    if (event.GetKeyCode() == WXK_BACK)
    {

        // this text ctrl contains something and the user pressed backspace...
        // we must delete the keypress...
        Clear();
        return;
    }

    if (event.GetEventType() == wxEVT_KEY_DOWN ||
        (event.GetEventType() == wxEVT_KEY_UP && !IsValidKeyComb()))
    {

        // the user pressed some key combination which must be displayed
        // in this text control.... or he has just stopped pressing a
        // modifier key like shift, ctrl or alt without adding any
        // other alphanumeric char, thus generating an invalid keystroke
        // which must be cleared out...

        KeyBinder::Key key;
        key.code    = event.GetKeyCode();
        key.flags   = event.GetModifiers();

        SetValue(KeyBinder::GetKeyBindingAsText(key));
        SetInsertionPointEnd();

    }

}