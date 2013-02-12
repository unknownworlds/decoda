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

#ifndef STRING_HISTORY_H
#define STRING_HISTORY_H

#include <wx/wx.h>
#include <vector>

/**
 * Stores a list of strings.
 */
class StringHistory
{

public:

    /**
     * Adds a string to the history. If the string is already present, it's moved
     * to the beginning of the history.
     */
    void Add(const wxString& string);

    /**
     * Returns the number of strings in the history.
     */
    unsigned int GetNumStrings() const;

    /**
     * Returns the ith string in the history. The 0th string is the most recently
     * added.
     */
    const wxString& GetString(unsigned int i) const;

private:

    static const unsigned int   s_maxStrings = 10;
    std::vector<wxString>       m_strings;

};

#endif