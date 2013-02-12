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

#ifndef THREAD_EVENT_H
#define THREAD_EVENT_H

#include <wx/event.h>

//
// Event definitions.
//

DECLARE_EVENT_TYPE(wxEVT_THREAD_EVENT, -1)

/**
 * Thread event class.
 */
class ThreadEvent : public wxEvent
{

public:

    /**
     * Constructor.
     */
    ThreadEvent();
    
    /**
     * From wxEvent.
     */
    virtual wxEvent* Clone() const;

};

typedef void (wxEvtHandler::*ThreadEventFunction)(ThreadEvent&);

#define EVT_THREAD(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_THREAD_EVENT, 0, -1, \
    (wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( ThreadEventFunction, & fn ), (wxObject *) NULL ),

#endif