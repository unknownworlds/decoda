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

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <wx/wx.h>
#include <wx/hyperlink.h>

/**
 * About box window.
 */
class AboutDialog : public wxDialog 
{

public:

    explicit AboutDialog( wxWindow* parent );

private:

    wxStaticBitmap*         m_bitmap31;
    wxStaticText*           m_staticText3;
    wxStaticText*           m_staticText13;
    wxHyperlinkCtrl*        m_staticText15;
    wxStaticText*           m_staticText9;
    wxStaticText*           m_staticText10;
    wxStaticText*           m_staticText11;
    wxStaticText*           m_staticText12;
    wxStaticText*           m_staticText16;
    wxStdDialogButtonSizer* m_sdbSizer2;
		
};

#endif