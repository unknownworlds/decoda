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

#ifndef SCRIPT_DEBUGGER_CONFIG_H
#define SCRIPT_DEBUGGER_CONFIG_H

#include <wx/wx.h>

// This define determines if Decoda is being compiled to work only for a
// specific product.
//#define DEDICATED_PRODUCT_VERSION

#ifdef DEDICATED_PRODUCT_VERSION

/**
 * Returns a human-readable name of the product that Decoda is being built
 * for.
 */
wxString GetDedicatedProductName();

/**
 * Returns the command line that should be used to launch the dedicated
 * application.
 */
wxString GetDedicatedProductCommand();

/**
 * Returns the command line arguments that will be used to launch the dedicated
 * application. The arguments passed in are the arguments specified by the user.
 * In general, these should be added to whatever special arguments the dedicated
 * product requires.
 */
wxString GetDedicatedProductCommandArguments(const wxString& arguments);

#endif

#endif