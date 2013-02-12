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

#include "DebugEvent.h"

DEFINE_EVENT_TYPE(wxEVT_DEBUG_EVENT)

wxDebugEvent::wxDebugEvent(EventId eventId, unsigned int vm)
    : wxEvent(0, wxEVT_DEBUG_EVENT)
{
    m_eventId       = eventId;
    m_vm            = vm;
    m_scriptIndex   = -1;
    m_line          = 0;
    m_enabled       = false;
    m_messageType   = MessageType_Normal;
}

EventId wxDebugEvent::GetEventId() const
{
    return m_eventId;
}

unsigned int wxDebugEvent::GetVm() const
{
    return m_vm;
}

void wxDebugEvent::SetScriptIndex(unsigned int scriptIndex)
{
    m_scriptIndex = scriptIndex;
}

unsigned int wxDebugEvent::GetScriptIndex() const
{
    return m_scriptIndex;
}

unsigned int wxDebugEvent::GetLine() const
{
    return m_line;
}

void wxDebugEvent::SetLine(unsigned int line)
{
    m_line = line;
}

bool wxDebugEvent::GetEnabled() const
{
    return m_enabled;
}

void wxDebugEvent::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

const wxString& wxDebugEvent::GetMessage() const
{
    return m_message;
}

void wxDebugEvent::SetMessage(const wxString& message)
{
    m_message = message;
}

MessageType wxDebugEvent::GetMessageType() const
{
    return m_messageType;
}

void wxDebugEvent::SetMessageType(MessageType messageType)
{
    m_messageType = messageType;
}

wxEvent* wxDebugEvent::Clone() const
{
    return new wxDebugEvent(*this);
}
