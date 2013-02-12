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

#include "WatchCtrl.h"
#include "Tokenizer.h"
#include "DebugFrontend.h"
#include "XmlUtility.h"

#include <wx/sstream.h>
#include <wx/xml/xml.h>

BEGIN_EVENT_TABLE(WatchCtrl, wxTreeListCtrl)
    EVT_SIZE(                               WatchCtrl::OnSize)
    EVT_LIST_COL_END_DRAG(wxID_ANY,         WatchCtrl::OnColumnEndDrag)
END_EVENT_TABLE()

WatchCtrl::WatchCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator &validator, const wxString& name)
    : wxTreeListCtrl(parent, id, pos, size, style, validator, name)
{

    AddColumn(_("Name"), 0, wxALIGN_LEFT);
    SetColumnEditable(0, true);
    AddColumn(_("Value"), 0, wxALIGN_LEFT);
    SetColumnEditable(1, false);
    AddColumn(_("Type"), 0, wxALIGN_LEFT);
    SetColumnEditable(2, false);

    m_columnSize[0] = 0.5f;
    m_columnSize[1] = 0.5f;
    m_columnSize[2] = -100;

    UpdateColumnSizes();

    m_vm = 0;
    m_stackLevel = 0;

}

void WatchCtrl::SetValueFont(const wxFont& font)
{
    
    m_valueFont = font;

    // Update all of the items.
    UpdateFont( GetRootItem() );

}

void WatchCtrl::UpdateFont(wxTreeItemId item)
{

    if (item.IsOk() )
    {

        SetItemFont( item, m_valueFont );
        UpdateFont( GetNextSibling(item) );

        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(item, cookie);

        while (child.IsOk())
        {
            UpdateFont(child);
            child = GetNextChild(item, cookie);
        }

    }

}

void WatchCtrl::UpdateColumnSizes()
{

    // We subtract two off of the size to avoid generating scroll bars on the window.
    int totalSize = GetClientSize().x - 2;

    int columnSize[3];
    GetColumnSizes(totalSize, columnSize);

    for (unsigned int i = 0; i < s_numColumns; ++i)
    {
        SetColumnWidth(i, columnSize[i]);
    }

}

void WatchCtrl::GetColumnSizes(int totalSize, int columnSize[s_numColumns]) const
{

    int fixedSize = 0;

    for (unsigned int i = 0; i < s_numColumns; ++i)
    {
        if (m_columnSize[i] < 0.0f)
        {
            columnSize[i] = static_cast<int>(-m_columnSize[i]);
            fixedSize += columnSize[i];
        }
    }

    // Set the size of the proportional columns.

    for (unsigned int i = 0; i < s_numColumns; ++i)
    {
        if (m_columnSize[i] >= 0.0f)
        {
            columnSize[i] = static_cast<int>(m_columnSize[i] * (totalSize -fixedSize) + 0.5f);
        }
    }

    // Make sure the total size is exactly correct by resizing the final column.

    for (unsigned int i = 0; i < s_numColumns - 1; ++i)
    {
        totalSize -= columnSize[i];
    }

    columnSize[s_numColumns - 1] = totalSize;

}

bool WatchCtrl::AddCompoundExpression(wxTreeItemId item, wxXmlNode* root)
{

    wxString type;
    wxString text = GetNodeAsText(root, type);
    
    // Remove any embedded zeros in the text. This happens if we're displaying a wide
    // string. Since we aren't using wide character wxWidgets, we cant' display that
    // properly, so we just hack it for roman text.

    bool englishWideCharacter = true;

    for (unsigned int i = 0; i < text.Length(); i += 2)
    {
        if (text[i] != 0)
        {
            englishWideCharacter = false;
        }
    }

    if (englishWideCharacter)
    {

        size_t convertedLength = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)text.c_str(), text.Length() / sizeof(wchar_t), NULL, 0, 0, 0);

        char* result = new char[convertedLength + 1]; 
        convertedLength = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)text.c_str(), text.Length() / sizeof(wchar_t), result, convertedLength, 0, 0);

        text = wxString(result, convertedLength);
    
    }

    SetItemText(item, 1, text);
    SetItemText(item, 2, type);

    if (root != NULL)
    {
        if (root->GetName() == "table")
        {

            // Add the elements of the table as tree children.

            wxXmlNode* node = root->GetChildren();
            while (node != NULL)
            {

                wxString typeName;

                if (ReadXmlNode(node, "type", typeName))
                {
                    SetItemText(item, 2, typeName);
                }
                else if (node->GetName() == "element")
                {

                    wxXmlNode* keyNode  = FindChildNode(node, "key");
                    wxXmlNode* dataNode = FindChildNode(node, "data");

                    if (keyNode != NULL && dataNode != NULL)
                    {

                        wxString type;
                        wxString key  = GetNodeAsText(keyNode->GetChildren(), type);

                        wxTreeItemId child = AppendItem(item, key);
                        SetItemFont(child, m_valueFont);

                        AddCompoundExpression(child, dataNode->GetChildren());

                    }

                }

                node = node->GetNext();

            }

        }
        else if (root->GetName() == "values")
        {

            wxXmlNode* node = root->GetChildren();
            unsigned int i = 1;

            while (node != NULL)
            {

                wxTreeItemId child = AppendItem(item, wxString::Format("%d", i));
                SetItemFont(child, m_valueFont);

                AddCompoundExpression(child, node);

                node = node->GetNext();
                ++i;

            }

        }
    }

    return true;

}

void WatchCtrl::UpdateItem(wxTreeItemId item)
{

    if (m_vm != 0)
    {

        wxString expression = GetItemText(item);
        wxString result;

        if (!expression.empty())
        {
            std::string temp;
            DebugFrontend::Get().Evaluate(m_vm, expression, m_stackLevel, temp);
            result = temp.c_str();
        }

        DeleteChildren(item);
        SetItemFont(item, m_valueFont);

        if (result.IsEmpty())
        {
            SetItemText(item, 1, "");
            SetItemText(item, 2, "");
        }
        else
        {

            wxStringInputStream stream(result);
            wxXmlDocument document;

            wxLogNull logNo;
            
            if (document.Load(stream))
            {
                AddCompoundExpression(item, document.GetRoot());
            }
            else
            {
                SetItemText(item, 1, "Improperly formatted XML data");
                SetItemText(item, 2, "");
            }
    
        }

    }
    else
    {
        SetItemText(item, 1, "");
        SetItemText(item, 2, "");
        DeleteChildren(item);
    }

}

void WatchCtrl::SetContext(unsigned int vm, unsigned int stackLevel)
{
    m_vm = vm;
    m_stackLevel = stackLevel;
}

void WatchCtrl::OnColumnEndDrag(wxListEvent& event)
{

    // Resize all of the columns to eliminate the scroll bar.

    int totalSize = GetClientSize().x;
    int column = event.GetColumn();

    int columnSize[s_numColumns];
    GetColumnSizes(totalSize, columnSize);

    int x = event.GetPoint().x;

    int newColumnSize = GetColumnWidth(column);
    int delta = columnSize[column] - newColumnSize;

    if (column + 1 < s_numColumns)
    {
        columnSize[column] = newColumnSize;
        columnSize[column + 1] += delta;
    }

    // Update the proportions of the columns.

    int fixedSize = 0;

    for (unsigned int i = 0; i < s_numColumns; ++i)
    {
        if (m_columnSize[i] < 0.0f)
        {
            m_columnSize[i] = -static_cast<float>(columnSize[i]);
            fixedSize += columnSize[i];
        }
    }

    for (unsigned int i = 0; i < s_numColumns; ++i)
    {
        if (m_columnSize[i] >= 0.0f)
        {
            m_columnSize[i] = static_cast<float>(columnSize[i]) / (totalSize - fixedSize);
        }
    }

    UpdateColumnSizes();

}

void WatchCtrl::OnSize(wxSizeEvent& event)
{
    UpdateColumnSizes();
    event.Skip();
}

wxString WatchCtrl::GetTableAsText(wxXmlNode* root)
{

    assert(root->GetName() == "table");

    int maxElements = 4;

    // Add the elements of the table as tree children.

    wxString result = "{";
    wxString type;

    wxXmlNode* node = root->GetChildren();

    int numElements = 0;

    while (node != NULL)
    {

        if (node->GetName() == "element")
        {

            wxXmlNode* keyNode  = FindChildNode(node, "key");
            wxXmlNode* dataNode = FindChildNode(node, "data");

            if (keyNode != NULL && dataNode != NULL)
            {

                wxString key  = GetNodeAsText(keyNode->GetChildren(), type);
                wxString data = GetNodeAsText(dataNode->GetChildren(), type);

                if (numElements >= maxElements)
                {
                    result += "...";
                    break;
                }

                result += key + "=" + data + " ";
                ++numElements;

            }

        }

        node = node->GetNext();

    }

    result += "}";

    return result;

}

wxString WatchCtrl::GetNodeAsText(wxXmlNode* node, wxString& type)
{

    wxString text;

    if (node != NULL)
    {
        if (node->GetName() == "error")
        {
            ReadXmlNode(node, "error", text);
        }
        else if (node->GetName() == "table")
        {
            text = GetTableAsText(node);
        }
        else if (node->GetName() == "values")
        {

            wxXmlNode* child = node->GetChildren();

            while (child != NULL)
            {

                if (!text.IsEmpty())
                {
                    text += ", ";
                }
    
                wxString dummy;
                text += GetNodeAsText(child, dummy);
                child = child->GetNext();
            
            }
        
        }
        else if (node->GetName() == "value")
        {

            wxXmlNode* child = node->GetChildren();

            while (child != NULL)
            {
                ReadXmlNode(child, "type", type) ||
                ReadXmlNode(child, "data", text);
                child = child->GetNext();
            }

        }
        else if (node->GetName() == "function")
        {

            unsigned int scriptIndex = -1;
            unsigned int lineNumber  = -1;

            wxXmlNode* child = node->GetChildren();

            while (child != NULL)
            {
                ReadXmlNode(child, "script", scriptIndex);
                ReadXmlNode(child, "line",   lineNumber);
                child = child->GetNext();
            }

            text = "function";

            DebugFrontend::Script* script = DebugFrontend::Get().GetScript(scriptIndex);

            if (script != NULL)
            {
                text += " defined at ";
                text += script->name;
                text += ":";
                text += wxString::Format("%d", lineNumber + 1);
            }

            type = "function";

        }
    }
    
    return text;

}
