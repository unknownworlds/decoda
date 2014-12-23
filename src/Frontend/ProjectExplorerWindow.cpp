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

#include "ProjectExplorerWindow.h"
#include "SearchTextCtrl.h"
#include "ProjectFileInfoCtrl.h"
#include "ProjectFilterPopup.h"
#include "Tokenizer.h"
#include "StlUtility.h"
#include "Symbol.h"
#include "DebugFrontend.h"

#include <wx/file.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <hash_map>

#include "res/explorer.xpm"
#include "res/filter_bitmap.xpm"

DECLARE_EVENT_TYPE( wxEVT_TREE_CONTEXT_MENU, -1 )

typedef void (wxEvtHandler::*wxTreeContextMenuEvent)(wxTreeEvent&);

#define EVT_TREE_CONTEXT_MENU(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_TREE_CONTEXT_MENU, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxTreeContextMenuEvent, & fn ), (wxObject *) NULL ),

DEFINE_EVENT_TYPE( wxEVT_TREE_CONTEXT_MENU )

BEGIN_EVENT_TABLE(ProjectExplorerWindow, wxPanel)

    EVT_TEXT( ID_Filter,                OnFilterTextChanged )

    EVT_BUTTON( ID_FilterButton,        OnFilterButton )

    EVT_TREE_ITEM_EXPANDING(wxID_ANY,   OnTreeItemExpanding )
    EVT_TREE_ITEM_COLLAPSING(wxID_ANY,  OnTreeItemCollapsing )
    EVT_TREE_ITEM_ACTIVATED(wxID_ANY,   OnTreeItemActivated )
    EVT_TREE_ITEM_MENU(wxID_ANY,        OnTreeItemContextMenu)
    EVT_TREE_SEL_CHANGED(wxID_ANY,      OnTreeItemSelectionChanged)

    EVT_TREE_CONTEXT_MENU(wxID_ANY,     OnMenu)

END_EVENT_TABLE()

ProjectExplorerWindow::ProjectExplorerWindow(wxWindow* parent, wxWindowID winid)
    : wxPanel(parent, winid)
{

    SetSize(250, 300);

    m_stopExpansion = 0;

    // Initially everything will be displayed.
    m_filterFlags = FilterFlag_Temporary | FilterFlag_Unversioned | FilterFlag_CheckedIn | FilterFlag_CheckedOut;

    // Load the bitmaps for the filter button.

    wxBitmap filterBitmap(filter_bitmap, wxBITMAP_TYPE_XPM);

    m_filterImageList = new wxImageList(18, 17);
    m_filterImageList->Add(filterBitmap, wxColour(0xFF, 0x9B, 0x77));

    // Load the bitmaps for the tree view.

    wxBitmap bitmap(explorer, wxBITMAP_TYPE_XPM);

    wxImageList* imageList = new wxImageList(18, 17);
    imageList->Add(bitmap, wxColour(0xFF, 0x9B, 0x77));

    m_root = 0;

	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	
    gSizer1->AddGrowableCol( 0 );
    gSizer1->AddGrowableRow( 1 );
    gSizer1->SetFlexibleDirection( wxBOTH );

	wxFlexGridSizer* gSizer2;
	gSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	
    gSizer2->AddGrowableCol( 0 );
    gSizer2->AddGrowableRow( 0 );
    gSizer2->SetFlexibleDirection( wxBOTH );

    m_searchBox = new SearchTextCtrl(this, ID_Filter, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH);
    
    m_filterButton = new wxBitmapButton( this, ID_FilterButton, wxNullBitmap, wxDefaultPosition, wxSize(18, 17), 0 );
    m_filterPopup  = NULL;

    gSizer2->Add( m_searchBox, 1, wxALL | wxEXPAND, 4);
    gSizer2->Add( m_filterButton, 1, wxALL | wxEXPAND, 4);

    gSizer1->Add( gSizer2, 1, wxEXPAND, 5  );

    m_tree = new wxTreeCtrl(this, winid, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_MULTIPLE | wxTR_EXTENDED);
    m_tree->AssignImageList(imageList);

    // The "best size" for the tree component must be 0 so that the tree control doesn't
    // attempt to expand to fit all of the items in it (which would cause the info box to
    // be nudged out of the window.
    m_tree->SetInitialSize(wxSize(0,0));

    m_searchBox->SetNextWindow(m_tree);

    m_infoBox = new ProjectFileInfoCtrl(this);

    gSizer1->Add( m_tree, 1, wxALL|wxEXPAND, 4 );
    gSizer1->Add( m_infoBox, 1, wxALL | wxEXPAND, 4 );

	SetSizer( gSizer1 );
	Layout();

    m_project = NULL;

    m_contextMenu = NULL;
    m_filterMatchAnywhere = false;

    UpdateFilterButtonImage();

}

ProjectExplorerWindow::~ProjectExplorerWindow()
{
    delete m_filterImageList;
}


void ProjectExplorerWindow::SetFontColorSettings(const FontColorSettings& settings)
{
  SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Window).backColor);
  m_tree->SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Window).backColor);

  m_searchBox->SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Window).backColor);
  m_searchBox->SetDefaultStyle(wxTextAttr(settings.GetColors(FontColorSettings::DisplayItem_Window).foreColor));

  m_infoBox->SetFontColorSettings(settings);

  m_itemColor = settings.GetColors(FontColorSettings::DisplayItem_Window).foreColor;

  Rebuild();
}

void ProjectExplorerWindow::SetFocusToFilter()
{
    // Select all of the text in the box (makes it easy to clear).
    m_searchBox->SetSelection(-1, -1);
    m_searchBox->SetFocus();
}

void ProjectExplorerWindow::SetProject(Project* project)
{
    m_project = project;
    Rebuild();
}

ProjectExplorerWindow::ItemData* ProjectExplorerWindow::GetDataForItem(wxTreeItemId id) const
{
  ItemData* data = nullptr;
  if (id.IsOk())
    data =static_cast<ItemData*>(m_tree->GetItemData(id));
  
  return data;
}

wxTreeItemId ProjectExplorerWindow::GetSelection() const
{

    wxArrayTreeItemIds selectedIds;
    m_tree->GetSelections(selectedIds);

    if (selectedIds.Count() > 0)
    {
        return selectedIds[0];
    }
    else
    {
        return wxTreeItemId();
    }

}

void ProjectExplorerWindow::OnFilterTextChanged(wxCommandEvent& event)
{
    
    m_filter = m_searchBox->GetValue();

    // If the filter begins with a space, we'll match the text anywhere in the
    // the symbol name.
    unsigned int filterLength = m_filter.Length();
    m_filter.Trim(false);
    m_filterMatchAnywhere = (m_filter.Length() < filterLength);

    // Make the filter lowercase since matching with be case independent.
    m_filter.MakeLower();
    
    Rebuild();

}

void ProjectExplorerWindow::Rebuild()
{

    m_tree->Freeze();

    m_tree->DeleteAllItems();
    m_root = m_tree->AddRoot("Root");

    if (m_project != NULL)
    {
        for (unsigned int i = 0; i < m_project->GetNumDirectories(); ++i)
        {
           RebuildForDirectory(m_project->GetDirectory(i));
        }

        for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
        {
            RebuildForFile(m_root, m_project->GetFile(i));
        }
    }

    m_tree->SortChildren(m_tree->GetRootItem());

    // For whatever reason the unselect event isn't reported properly
    // after deleting all items, so explicitly clear out the info box.
    m_infoBox->SetFile(NULL);

    m_tree->Thaw();

    // Select the first item in the newly created list
    wxTreeItemId firstItem = m_tree->GetFirstVisibleItem();
    m_tree->SelectItem(firstItem);

}

void ProjectExplorerWindow::RebuildForDirectory(Project::Directory *directory)
{
  ItemData* data = new ItemData;
  data->file = directory;
  data->symbol = NULL;
  data->isFile = false;

  wxTreeItemId node = m_tree->AppendItem(m_root, directory->name, 8, 8, data);
  m_tree->SetItemTextColour(node, m_itemColor);

  for (Project::File *file : directory->files)
  {
    RebuildForFile(node, file);
  }
}

void ProjectExplorerWindow::RebuildForFile(wxTreeItemId node, Project::File* file)
{

    // Check if the file passes our filter flags.

    bool matchesFlags = false;

    if ((m_filterFlags & FilterFlag_Unversioned) && file->status == Project::Status_None)
    {
        matchesFlags = !file->temporary;
    }

    if ((m_filterFlags & FilterFlag_CheckedIn) && file->status == Project::Status_CheckedIn)
    {
        matchesFlags = true;
    }
    
    if ((m_filterFlags & FilterFlag_CheckedOut) && file->status == Project::Status_CheckedOut)
    {
        matchesFlags = true;
    }

    if ((m_filterFlags & FilterFlag_Temporary) && file->temporary)
    {
        matchesFlags = true;
    }

    if (matchesFlags)
    {
        if (m_filter.IsEmpty())
        {
            // Just include the files like the standard Visual Studio project view.
            AddFile(node, file);
        }
        else
        {

            // Filter all of the symbols, modules and file name.
                
            // Check to see if the file name matches the filter.
          if (MatchesFilter(file->fileName.GetFullName(), m_filter) || MatchesFilter(file->localPath, m_filter))
            {
                AddFile(node, file);
            }

            for (unsigned int j = 0; j < file->symbols.size(); ++j)
            {

                Symbol* symbol = file->symbols[j];
                
                // Check to see if the symbol matches the filter.
                if (MatchesFilter(symbol->name, m_filter))
                {
                    AddSymbol(node, file, symbol);
                }

            }
        
        }
    }

}

bool ProjectExplorerWindow::MatchesFilter(const wxString& string, const wxString& filter) const
{

    if (m_filterMatchAnywhere)
    {
        return string.Lower().Find(m_filter) != wxNOT_FOUND;
    }
    else
    {

        if (filter.Length() > string.Length())
        {
            return false;
        }

        for (unsigned int i = 0; i < filter.Length(); ++i)
        {
            if (tolower(string[i]) != filter[i])
            {
                return false;
            }
        }

        return true;
    
    }

}

void ProjectExplorerWindow::AddFile(wxTreeItemId parent, Project::File* file)
{

    ItemData* data = new ItemData;
    data->file      = file;
    data->symbol    = NULL;
    data->isFile    = true;

    int image = GetImageForFile(file);
    wxTreeItemIdValue cookie;

    bool found = false;
    wxString path = file->localPath;
    wxString directory = path.BeforeFirst('\\');
    wxTreeItemId node;
    wxTreeItemId parentNode = parent;

    if (directory.IsEmpty())
      node = parent;
    else
    {
      while (!directory.IsEmpty())
      {
        found = false;

        //Add needed directories for file
        node = m_tree->GetFirstChild(parentNode, cookie);
        while (node.IsOk())
        {
          if (m_tree->GetItemText(node) == directory)
          {
            found = true;
            break;
          }

          node = m_tree->GetNextChild(parentNode, cookie);
        }

        if (!found)
        {
          node = m_tree->AppendItem(parentNode, directory, 8, 8, nullptr);
          m_tree->SetItemTextColour(node, m_itemColor);
        }

        path.Remove(0, directory.size() + 1);

        directory = path.BeforeFirst('\\');

        parentNode = node;
      }

    }

    wxTreeItemId fileNode = m_tree->AppendItem(node, file->GetDisplayName(), image, image, data);
    m_tree->SetItemTextColour(fileNode, m_itemColor);
    // Add the symbols.

    stdext::hash_map<std::string, wxTreeItemId> groups;

    for (unsigned int i = 0; i < file->symbols.size(); ++i)
    {

        wxTreeItemId node = fileNode;

        if (!file->symbols[i]->module.IsEmpty())
        {

            stdext::hash_map<std::string, wxTreeItemId>::const_iterator iterator;
            iterator = groups.find(file->symbols[i]->module.ToAscii());

            if (iterator == groups.end())
            {
                node = m_tree->AppendItem(fileNode, file->symbols[i]->module, Image_Module, Image_Module);
                m_tree->SetItemTextColour(node, m_itemColor);
                groups.insert(std::make_pair(file->symbols[i]->module.ToAscii(), node));
            }
            else
            {
                node = iterator->second;
            }

        }

        AddSymbol(node, file, file->symbols[i]);

    }

}

void ProjectExplorerWindow::AddSymbol(wxTreeItemId parent, Project::File* file, Symbol* symbol)
{

    ItemData* data = new ItemData;
    data->file      = file;
    data->symbol    = symbol;
    data->isFile    = true;

    wxString fullName = symbol->name + " ()";

    if (!symbol->module.IsEmpty())
    {
        fullName += " - " + symbol->module;
    }

    wxTreeItemId node = m_tree->AppendItem(parent, fullName, Image_Function, Image_Function, data);
    m_tree->SetItemTextColour(node, m_itemColor);
}

void ProjectExplorerWindow::OnTreeItemExpanding(wxTreeEvent& event)
{
    if (event.GetItem() == m_stopExpansion)
    {
        event.Veto();
        m_stopExpansion = 0;
    }
}

void ProjectExplorerWindow::OnTreeItemCollapsing(wxTreeEvent& event)
{
    if (event.GetItem() == m_stopExpansion)
    {
        event.Veto();
        m_stopExpansion = 0;
    }
}

void ProjectExplorerWindow::OnTreeItemActivated(wxTreeEvent& event)
{
    
    // This is a bit hacky, but to prevent the control from expanding an item
    // when we activate it, we store the id and then veto the
    // expansion/contraction event.
    m_stopExpansion = event.GetItem();

    // Keep processing the event.
    event.Skip(true);

}

void ProjectExplorerWindow::OnTreeItemContextMenu(wxTreeEvent& event)
{

    m_tree->SelectItem(event.GetItem(), true);

    if (m_contextMenu != NULL)
    {

        ItemData* itemData = GetDataForItem(event.GetItem());

        // If this is a file, display the context menu.
        if (itemData != NULL && itemData->file != NULL && itemData->symbol == NULL)
        {
            wxTreeEvent copy(event);
            copy.SetEventType(wxEVT_TREE_CONTEXT_MENU);
            AddPendingEvent(copy);
        }

    }

}

void ProjectExplorerWindow::OnTreeItemSelectionChanged(wxTreeEvent& event)
{
    // HATE: So when windows deletes a treeview item it sends a ItemSelectionChange
    // event which in this code will cause the SelectItem function to be called
    // this will cause the DeleteAllItems function to bail but will not unfreeze
    // the tree when we rebuild the list which will then cause it to crash when
    // when we try to change focus to the list. So we just ignore these events if
    // the tree is frozen. This would not happen if Microsoft did not LIE in its
    // documentation.
    if (m_tree->IsFrozen()) {
        event.Skip();
        return;
    }

    Project::File* file = NULL;
    wxTreeItemId item = event.GetItem();

    if (item.IsOk())
    {
        m_tree->SelectItem(item);
        ItemData* itemData = GetDataForItem(item);

        if (itemData != NULL && itemData->file != NULL && itemData->isFile)
        {
            file = (Project::File *)itemData->file;
        }

    }
    else
    {
        int a = 0;
    }

    m_infoBox->SetFile(file);

}

void ProjectExplorerWindow::GetSelectedFiles(std::vector<Project::File*>& selectedFiles) const
{
    wxArrayTreeItemIds selectedIds;
    m_tree->GetSelections(selectedIds);

    for (unsigned int i = 0; i < selectedIds.Count(); ++i)
    {

        ItemData* itemData = GetDataForItem(selectedIds[i]);

        // Only add the selected item if it's a file.
        if (itemData != NULL && itemData->file != NULL && itemData->symbol == NULL && itemData->isFile)
        {
            selectedFiles.push_back((Project::File *)itemData->file);
        }

    }
}

void ProjectExplorerWindow::GetSelectedDirectories(std::vector<Project::Directory*>& selectedDirectories) const
{
  wxArrayTreeItemIds selectedIds;
  m_tree->GetSelections(selectedIds);

  for (unsigned int i = 0; i < selectedIds.Count(); ++i)
  {

    ItemData* itemData = GetDataForItem(selectedIds[i]);

    // Only add the selected item if it's a file.
    if (itemData != NULL && itemData->file != NULL && itemData->isFile == false)
    {
      selectedDirectories.push_back((Project::Directory *)itemData->file);
    }

  }
  
}

void ProjectExplorerWindow::InsertDirectory(Project::Directory* directory)
{
  RebuildForDirectory(directory);
  m_tree->SortChildren(m_tree->GetRootItem());
}

void ProjectExplorerWindow::RemoveDirectory(Project::Directory* directory)
{
  for (Project::File *file : directory->files)
  {
    RemoveFileSymbols(m_tree->GetRootItem(), file);

    if (m_infoBox->GetFile() == file)
    {
      m_infoBox->SetFile(NULL);
    }
  }

  RemoveFileSymbols(m_tree->GetRootItem(), (Project::File *)directory);
}

void ProjectExplorerWindow::InsertFile(Project::File* file)
{
    RebuildForFile(m_root, file);
    m_tree->SortChildren(m_tree->GetRootItem());
}

void ProjectExplorerWindow::RemoveFile(Project::File* file)
{

    RemoveFileSymbols(m_tree->GetRootItem(), file);

    if (m_infoBox->GetFile() == file)
    {
        m_infoBox->SetFile(NULL);
    }

}

void ProjectExplorerWindow::RemoveFiles(const std::vector<Project::File*>& files)
{

    stdext::hash_set<Project::File*> fileSet;

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        fileSet.insert(files[i]);
    }

    RemoveFileSymbols(m_tree->GetRootItem(), fileSet);

}

void ProjectExplorerWindow::RemoveFileSymbols(wxTreeItemId node, Project::File* file)
{

    ItemData* data = static_cast<ItemData*>(m_tree->GetItemData(node));

    if (data != NULL && data->file == file)
    {
        m_tree->Delete(node);
    }
    else
    {

        // Recurse into the children.

        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(node, cookie);

        while (child.IsOk())
        {
            wxTreeItemId next = m_tree->GetNextChild(node, cookie);
            RemoveFileSymbols(child, file);
            child = next;
        }

    }
    
}

void ProjectExplorerWindow::RemoveFileSymbols(wxTreeItemId node, const stdext::hash_set<Project::File*>& fileSet)
{

    ItemData* data = static_cast<ItemData*>(m_tree->GetItemData(node));

    if (data != NULL && data->isFile && fileSet.find((Project::File *)data->file) != fileSet.end())
    {
        m_tree->Delete(node);
    }
    else
    {

        // Recurse into the children.

        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(node, cookie);

        while (child.IsOk())
        {
            wxTreeItemId next = m_tree->GetNextChild(node, cookie);
            RemoveFileSymbols(child, fileSet);
            child = next;
        }

    }
    
}

void ProjectExplorerWindow::SetFileContextMenu(wxMenu* contextMenu)
{
    m_contextMenu = contextMenu;
}

int ProjectExplorerWindow::GetImageForFile(const Project::File* file) const
{

    int image = 0;

    if (file->status == Project::Status_CheckedIn)
    {
        image = Image_FileCheckedIn;
        if (file->temporary)
        {
            image = Image_FileTempCheckedIn;
        }
    }
    else if (file->status == Project::Status_CheckedOut)
    {
        image = Image_FileCheckedOut;
        if (file->temporary)
        {
            image = Image_FileTempCheckedOut;
        }
    }
    else
    {
        image = Image_File;
        if (file->temporary)
        {
            image = Image_FileTemp;
        }
    }
    
    return image;
}

void ProjectExplorerWindow::UpdateFileImages()
{

    m_tree->Freeze();

    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_tree->GetFirstChild(m_root, cookie);

    while (item.IsOk())
    {

        ItemData* data = GetDataForItem(item);
        
        if (data != NULL && data->file != NULL && data->symbol == NULL && data->isFile)
        {
            int image = GetImageForFile((Project::File *)data->file);
            m_tree->SetItemImage(item, image);
            m_tree->SetItemImage(item, image, wxTreeItemIcon_Selected);
        }

        item = m_tree->GetNextSibling(item);

    }

    m_tree->Thaw();

}

void ProjectExplorerWindow::OnMenu(wxTreeEvent& event)
{
    m_tree->PopupMenu(m_contextMenu, event.GetPoint());
}

void ProjectExplorerWindow::OnFilterButton(wxCommandEvent& event)
{
    
    wxPoint point = m_filterButton->GetScreenPosition();
    point.y += m_filterButton->GetSize().y; 

    if (m_filterPopup == NULL)
    {
        // For some reason, creating this in the constructor causes hangs on some
        // machines. It *seems* like a problem with Windows/wxWidgets so we just
        // defer construction until we need it.
        m_filterPopup = new ProjectFilterPopup( this, this );
    }
    
    m_filterPopup->Position(point, wxDefaultSize);
    m_filterPopup->Popup(NULL);

}

void ProjectExplorerWindow::UpdateFile(Project::File* file)
{

    m_tree->Freeze();

    RemoveFileSymbols(m_tree->GetRootItem(), file);
    RebuildForFile(m_root, file);

    m_tree->SortChildren(m_tree->GetRootItem());

    m_tree->Thaw();

}

void ProjectExplorerWindow::SetFilterFlags(unsigned int filterFlags)
{
    if (m_filterFlags != filterFlags)
    {
        
        m_filterFlags = filterFlags;
        
        // Because rebuilding the tree can take a few milliseconds which prevents
        // the message pump from dispatching paint events, we get less flickering
        // if we update the button image after rebuilding the tree.
        Rebuild();
        UpdateFilterButtonImage();

    }
}

unsigned int ProjectExplorerWindow::GetFilterFlags() const
{
    return m_filterFlags;
}

void ProjectExplorerWindow::UpdateFilterButtonImage()
{

    const wxColor chromaKey(0xFF, 0x9B, 0x77);

    wxBitmap bitmap(18, 17, 24);

    wxMemoryDC dc;
    dc.SelectObject(bitmap);

    wxBrush backgroundBrush( chromaKey );

    dc.SetBackground( backgroundBrush );
    dc.Clear();

    if (m_filterFlags & FilterFlag_Temporary && !(m_filterFlags & FilterFlag_Unversioned))
    {
        dc.DrawBitmap( m_filterImageList->GetBitmap(1), 0, 0, true );
    }
    else if (m_filterFlags & FilterFlag_Unversioned && m_filterFlags & FilterFlag_Temporary)
    {
        dc.DrawBitmap( m_filterImageList->GetBitmap(2), 0, 0, true );
    }
    else
    {
        dc.DrawBitmap( m_filterImageList->GetBitmap(0), 0, 0, true );
    }

    if (m_filterFlags & FilterFlag_CheckedIn)
    {
        dc.DrawBitmap( m_filterImageList->GetBitmap(3), 0, 0, true );
    }

    if (m_filterFlags & FilterFlag_CheckedOut)
    {
        dc.DrawBitmap( m_filterImageList->GetBitmap(4), 0, 0, true );
    }

    wxBitmap temp;
    dc.SelectObject(temp);

    // Create a mask for the bitmap to mark the transparent areas.
    wxMask* mask = new wxMask( bitmap, chromaKey );
    bitmap.SetMask( mask );

    m_filterButton->SetBitmapLabel(bitmap);
    
}