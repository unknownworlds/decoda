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

#include "CodeEditDropTarget.h"
#include "CodeEdit.h"
#include "MainFrame.h"

class DropTargetsComposite: public wxDataObjectComposite
{

public:

    DropTargetsComposite()
    { 
        m_dataObjectLast = NULL;
    }

    bool SetData(const wxDataFormat& format, size_t len, const void* buf)
    {
        m_dataObjectLast = GetObject(format);
        wxCHECK_MSG(m_dataObjectLast, FALSE, wxT("unsupported format in wxDataObjectComposite"));
        return m_dataObjectLast->SetData (len, buf);
    }

    wxDataObjectSimple* GetLastDataObject()
    {
        return m_dataObjectLast;
    }

private:

    wxDataObjectSimple* m_dataObjectLast;

};

CodeEditDropTarget::CodeEditDropTarget(CodeEdit* codeEdit, MainFrame* mainFrame)
{

    m_codeEdit  = codeEdit;
    m_mainFrame = mainFrame;

    m_file = new wxFileDataObject();
    m_text = new wxTextDataObject();

    // Set composite target
    DropTargetsComposite *data = new DropTargetsComposite();
    data->Add ((wxDataObjectSimple*)m_file);
    data->Add ((wxDataObjectSimple*)m_text, true); // Set as preferred
    SetDataObject (data);

}

wxDragResult CodeEditDropTarget::OnData (wxCoord x, wxCoord y, wxDragResult def)
{

    if (!GetData())
    {
        return wxDragNone;
    }

    // Get object
    wxDataObjectSimple *obj = ((DropTargetsComposite *)GetDataObject())->GetLastDataObject();
    if (obj == ((wxDataObjectSimple*)m_file))
    {
        if (!OnDataFiles (x, y, m_file->GetFilenames())) def = wxDragNone;
    }
    else if (obj == ((wxDataObjectSimple*)m_text))
    {
        if (!OnDataText (x, y, m_text->GetText())) def = wxDragNone;
    }else
    {
        def = wxDragNone;
    }

    return def;
}

bool CodeEditDropTarget::OnDataFiles (wxCoord x, wxCoord y, const wxArrayString& fileNames)
{
    m_mainFrame->OnDropFiles(x, y, fileNames);                                   
    wxActivateEvent evt (wxEVT_ACTIVATE);
    wxPostEvent (m_mainFrame, evt);
    return TRUE;
}

bool CodeEditDropTarget::OnDataText (wxCoord x, wxCoord y, const wxString& data)
{
    bool ok = m_codeEdit->DoDropText (x, y, data);
    if (ok)
    {
        wxActivateEvent evt (wxEVT_ACTIVATE);
        wxPostEvent (m_mainFrame, evt);
    }
    return ok;
}

wxDragResult CodeEditDropTarget::OnDragOver (wxCoord x, wxCoord y, wxDragResult def)
{
    return m_codeEdit->DoDragOver (x, y, def);
}

wxDragResult CodeEditDropTarget::OnEnter (wxCoord x, wxCoord y, wxDragResult def)
{
    return m_codeEdit->DoDragEnter (x, y, def);
}

void CodeEditDropTarget::OnLeave()
{
    m_codeEdit->DoDragLeave();
}
