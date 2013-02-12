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

#include "StringHistory.h"
#include <algorithm>

void StringHistory::Add(const wxString& string)
{

    // Check if we already have the string.

    std::vector<wxString>::iterator iterator;
    iterator = std::find(m_strings.begin(), m_strings.end(), string);
    
    // If we have it, remove it. We'll move it up to the front of the
    // history next.
    if (iterator != m_strings.end())
    {
        m_strings.erase(iterator);
    }

    // Add the string to the beginning of the history.
    m_strings.insert(m_strings.begin(), string);

    // If we have too many strings, remove the last.
    if (m_strings.size() > s_maxStrings)
    {
        m_strings.erase(m_strings.begin() + s_maxStrings, m_strings.end());
    }

}

unsigned int StringHistory::GetNumStrings() const
{
    return m_strings.size();
}

const wxString& StringHistory::GetString(unsigned int i) const
{
    return m_strings[i];
}