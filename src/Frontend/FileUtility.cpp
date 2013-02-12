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

#include "FileUtility.h"

#include <shlobj.h>

void ShowFileInFolder (wxFileName& inPath) {
  if (!inPath.IsOk())
    return;

  typedef HRESULT (WINAPI* SHOpenFolderAndSelectItemsPtr)(
    LPCITEMIDLIST pidlFolder, 
    UINT cidl, 
    LPCITEMIDLIST* apidl, 
    DWORD dwFlags);

  wxString& fullPath = inPath.GetFullPath();  
  wxString& path = inPath.GetPath();

  static SHOpenFolderAndSelectItemsPtr openFolderFunction = NULL;
  static bool intializedFunction = true;

  if (intializedFunction) {
    intializedFunction = false;
    HMODULE shell32 = GetModuleHandle("shell32.dll");
    if (shell32 == NULL)
      return;

    openFolderFunction = reinterpret_cast<SHOpenFolderAndSelectItemsPtr>(GetProcAddress(shell32, "SHOpenFolderAndSelectItems"));
    if (!openFolderFunction) {
      ShellExecute(NULL, _T("open"), fullPath.c_str(), NULL, NULL, SW_SHOW);
      return;
    }
  }

  IShellFolder* desktopFolder;
  HRESULT result = SHGetDesktopFolder(&desktopFolder);
  if(FAILED(result))
    return;

  LPITEMIDLIST folderPath;
  LPITEMIDLIST filePath;

  result = desktopFolder->ParseDisplayName(NULL, NULL, (LPOLESTR)(const wchar_t *)path.wc_str(wxConvUTF8), NULL, &folderPath, NULL);
  if (SUCCEEDED(result)) {  
    result = desktopFolder->ParseDisplayName(NULL, NULL, (LPOLESTR)(const wchar_t *)fullPath.wc_str(wxConvUTF8), NULL, &filePath, NULL);
    if (SUCCEEDED(result)) {

      const ITEMIDLIST* highlight[] = {
        {filePath},
      };

      (*openFolderFunction)(folderPath, ARRAYSIZE(highlight), highlight, NULL);      
    }
  }

  if (folderPath)
    CoTaskMemFree(folderPath);

  if (filePath)
    CoTaskMemFree(filePath);

  desktopFolder->Release();
  
}
