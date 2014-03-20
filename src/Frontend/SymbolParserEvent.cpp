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

#include "SymbolParserEvent.h"

DEFINE_EVENT_TYPE(wxEVT_SYMBOL_PARSER_EVENT)

SymbolParserEvent::SymbolParserEvent(unsigned int fileId, const std::vector<Symbol*>& symbols, bool isFinalQueueItem)
    : wxEvent(0, wxEVT_SYMBOL_PARSER_EVENT)
{
    m_fileId  = fileId;
    m_symbols = symbols;
    m_isFinalQueueItem = isFinalQueueItem;
}

wxEvent* SymbolParserEvent::Clone() const
{
    return new SymbolParserEvent(*this);
}

unsigned int SymbolParserEvent::GetFileId() const
{
    return m_fileId;
}

const std::vector<Symbol*>& SymbolParserEvent::GetSymbols() const
{
    return m_symbols;
}

bool SymbolParserEvent::GetIsFinalQueueItem() const
{
    return m_isFinalQueueItem;
}