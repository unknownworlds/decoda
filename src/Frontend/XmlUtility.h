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

#ifndef XML_UTILITY_H
#define XML_UTILITY_H

//
// Forward declarations.
//

class wxString;
class wxColour;
class wxXmlNode;
class wxRect;

/**
 * Creates an XML node of the form <tag>data</tag>
 */
wxXmlNode* WriteXmlNode(const wxString& tag, const wxString& data);

/**
 * Creates an XML node of the form <tag>data</tag>
 */
wxXmlNode* WriteXmlNode(const wxString& tag, const char* data);

/**
 * Creates an XML node of the form <tag>data</tag>
 */
wxXmlNode* WriteXmlNode(const wxString& tag, int data);

/**
 * Creates an XML node of the form <tag>data</tag>
 */
wxXmlNode* WriteXmlNode(const wxString& tag, const wxColour& color);

/**
 * Creates an XML node of the form <tag>data</tag>
 */
wxXmlNode* WriteXmlNodeBool(const wxString& tag, bool data);

/**
 * Creates an XML node of the form:
 *  <tag>
 *    <x>x</x>
 *    <y>y</y>
 *    <xSize>xSize</xSize>
 *    <ySize>ySize</ySize>
 *  </tag>
 */
wxXmlNode* WriteXmlNodeRect(const wxString& tag, const wxRect& rect);

/**
 * Attempts to read an XML node of the form <tag>data</tag>
 */
bool ReadXmlNode(wxXmlNode* node, const wxString& tag, wxString& data);

/**
 * Attempts to read an XML node of the form <tag>data</tag>
 */
bool ReadXmlNode(wxXmlNode* node, const wxString& tag, unsigned int& data);

/**
 * Attempts to read an XML node of the form <tag>data</tag>
 */
bool ReadXmlNode(wxXmlNode* node, const wxString& tag, int& data);

/**
 * Attempts to read an XML node of the form <tag>#color</tag>
 */
bool ReadXmlNode(wxXmlNode* node, const wxString& tag, wxColour& color);

/**
 * Attempts to read an XML node of the form <tag>(true|false)</tag>
 */
bool ReadXmlNode(wxXmlNode* node, const wxString& tag, bool& data);

/**
 * Attempts to rad an XML node of the form:
 *  <tag>
 *    <x>x</x>
 *    <y>y</y>
 *    <xSize>xSize</xSize>
 *    <ySize>ySize</ySize>
 *  </tag>
 */
bool ReadXmlNodeRect(wxXmlNode* node, const wxString& tag, wxRect& rect);

/**
 * Searches the children of a node for a specific name.
 */
wxXmlNode* FindChildNode(wxXmlNode* node, const wxString& name);

#endif