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

#include "WatchDropTarget.h"    
#include "WatchWindow.h"

WatchDropTarget::WatchDropTarget(WatchWindow* watch)
{
    m_watch = watch;
}

bool WatchDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
    m_watch->AddWatch(data);    
    // Return false since we want to effectively copy the text, not move it.
    return false;

}