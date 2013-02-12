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

#include "Config.h"
#include "Project.h"
#include "XmlUtility.h"
#include "StlUtility.h"
#include "DebugFrontend.h"
#include "Symbol.h"

#include <wx/xml/xml.h>
#include <wx/filefn.h>

#include <algorithm>

unsigned int Project::s_lastFileId = 0;

wxString Project::File::GetDisplayName() const
{

    wxString displayName = fileName.GetFullName();
    
    if (displayName.IsEmpty())
    {
        displayName = tempName;
    }

    return displayName;

}

wxString Project::File::GetFileType() const
{
    if (!type.IsEmpty())
    {
        return type;
    }
    else
    {
        return fileName.GetExt();
    }
}

Project::Project()
{
    m_needsSave     = false;
    m_needsUserSave = false;
    m_tempIndex = 0;
}

Project::~Project()
{
    
    for (unsigned int i = 0; i < m_files.size(); ++i)
    {
        ClearVector(m_files[i]->symbols);
    }    
    
    ClearVector(m_files);

}

wxString Project::GetName() const
{

    wxFileName fileName = m_fileName;
    wxString name = fileName.GetName();
    
    if (name.empty())
    {
        return "Untitled";
    }
    else
    {
        return name;
    }

}

bool Project::Save(const wxString& fileName)
{

    wxFileName userFileName(fileName);
    userFileName.SetExt("deuser");
    
    if (fileName != m_fileName)
    {
        m_needsSave = true;
        m_needsUserSave = true;
    }

    bool success = true;

    if (m_needsSave)
    {
        if (SaveGeneralSettings(fileName))
        {
            m_needsSave = false;
        }
        else
        {
            success = false;
        }
    }

    if (success && m_needsUserSave)
    {
        if (SaveUserSettings(userFileName.GetFullPath()))
        {
            m_needsUserSave = false;
        }
    }

    if (success)
    {
        m_fileName = fileName;
    }

    return success;

}

bool Project::Load(const wxString& fileName)
{

    if (!LoadGeneralSettings(fileName))
    {
        return false;
    }

    wxFileName userFileName(fileName);
    userFileName.SetExt("deuser");


    // Even if we can't load the user settings, we still treat the load as
    // successful. We do this since the user file may not be present.
    LoadUserSettings(userFileName.GetFullPath());

    m_fileName  = fileName;
    m_needsSave = false;

    return true;

}

const wxString& Project::GetFileName() const
{
    return m_fileName;
}

bool Project::GetNeedsSave() const
{
    return m_needsSave || m_needsUserSave;
}

const wxString& Project::GetCommandLine() const
{
    return m_commandLine;
}

void Project::SetCommandLine(const wxString& commandLine)
{
    m_commandLine = commandLine;
    m_needsUserSave = true;
}

const wxString& Project::GetCommandArguments() const
{
    return m_commandArguments;
}

void Project::SetCommandArguments(const wxString& commandArguments)
{
    m_commandArguments  = commandArguments;
    m_needsUserSave = true;
}

const wxString& Project::GetWorkingDirectory() const
{
    return m_workingDirectory;
}

void Project::SetWorkingDirectory(const wxString& workingDirectory)
{
    m_workingDirectory = workingDirectory;
    m_needsUserSave = true;
}

const wxString& Project::GetSymbolsDirectory() const
{
    return m_symbolsDirectory;
}

void Project::SetSymbolsDirectory(const wxString& symbolsDirectory)
{
    m_symbolsDirectory = symbolsDirectory;
    m_needsUserSave = true;
}

const wxString& Project::GetSccProvider() const
{
    return m_sccProvider;
}

void Project::SetSccProvider(const wxString& sccProvider)
{
    m_sccProvider = sccProvider;
    m_needsUserSave = true;
}

const wxString& Project::GetSccUser() const
{
    return m_sccUser;
}

void Project::SetSccUser(const wxString& sccUser)
{
    m_sccUser = sccUser;
    m_needsUserSave = true;
}

const wxString& Project::GetSccProjectName() const
{
    return m_sccProjName;
}

void Project::SetSccProjectName(const wxString& sccProjName)
{
    m_sccProjName = sccProjName;
    m_needsUserSave = true;
}

const wxString& Project::GetSccLocalPath() const
{
    return m_sccLocalPath;
}

void Project::SetSccLocalPath(const wxString& sccLocalPath)
{
    m_sccLocalPath = sccLocalPath;
    m_needsUserSave = true;
}

const wxString& Project::GetSccAuxProjectPath() const
{
    return m_sccAuxProjPath;
}

void Project::SetSccAuxProjectPath(const wxString& sccAuxProjPath)
{
    m_sccAuxProjPath = sccAuxProjPath;
    m_needsUserSave = true;
}

Project::File* Project::GetFile(unsigned int fileIndex)
{
    return m_files[fileIndex];
}

const Project::File* Project::GetFile(unsigned int fileIndex) const
{
    return m_files[fileIndex];
}

Project::File* Project::GetFileById(unsigned int fileId)
{

    for (unsigned int fileIndex = 0; fileIndex < m_files.size(); ++fileIndex)
    {
        if (m_files[fileIndex]->fileId == fileId)
        {
            return m_files[fileIndex];
        }
    }

    return NULL;

}

unsigned int Project::GetNumFiles() const
{
    return m_files.size();
}

Project::File* Project::AddFile(const wxString& fileName)
{

    // Check if the file is already in the project.

    for (unsigned int i = 0; i < m_files.size(); ++i)
    {
        if (!m_files[i]->temporary && m_files[i]->fileName.SameAs(fileName))
        {
            return NULL;
        }
    }

    File* file = new File;

    file->state         = CodeState_Normal;
    file->scriptIndex   = -1;
    file->temporary     = false;
    file->fileName      = fileName;
    file->status        = Status_None;
    file->fileId        = ++s_lastFileId;

    if (fileName.IsEmpty())
    {
        file->tempName = CreateTempName();
    }

    m_files.push_back(file);
    m_needsSave = true;
    
    return file;

}

Project::File* Project::AddTemporaryFile(unsigned int scriptIndex)
{
    
    const DebugFrontend::Script* script = DebugFrontend::Get().GetScript(scriptIndex);
    assert(script != NULL);

    File* file = new File;

    file->state         = script->state;
    file->scriptIndex   = scriptIndex;
    file->temporary     = true;
    file->fileName      = script->name.c_str();
    file->status        = Status_None;
    file->fileId        = ++s_lastFileId;

    m_files.push_back(file);

    return file;

}

Project::File* Project::AddTemporaryFile(const wxString& fileName)
{
    
    File* file = new File;

    file->state         = CodeState_Normal;
    file->scriptIndex   = -1;
    file->temporary     = true;
    file->fileName      = fileName;
    file->status        = Status_None;
    file->fileId        = ++s_lastFileId;

    if (fileName.IsEmpty())
    {
        file->tempName = CreateTempName();
    }

    m_files.push_back(file);

    return file;

}

void Project::RemoveFile(File* file)
{

    std::vector<File*>::iterator iterator = m_files.begin();

    while (iterator != m_files.end())
    {
        if (file == *iterator)
        {
            m_files.erase(iterator);
            if (!file->temporary)
            {
                m_needsSave = true;
                m_needsUserSave = true;
            }
            break;
        }
        ++iterator;
    }
    
    ClearVector(file->symbols);
    delete file;

}

void Project::CleanUpAfterSession()
{

    for (unsigned int i = 0; i < m_files.size(); ++i)
    {
        m_files[i]->scriptIndex = -1;
    }

}

Project::File* Project::GetFileForScript(unsigned int scriptIndex) const
{

    for (unsigned int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->scriptIndex == scriptIndex)
        {
            return m_files[i];
        }
    }

    return NULL;

}

Project::File* Project::GetFileForFileName(const wxFileName& fileName) const
{

    for (unsigned int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->fileName.GetFullName().CmpNoCase(fileName.GetFullName()) == 0)
        {
            return m_files[i];
        }
    }

    return NULL;

}

void Project::SetBreakpoint(unsigned int scriptIndex, unsigned int line, bool set)
{

    for (unsigned int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->scriptIndex == scriptIndex)
        {

            std::vector<unsigned int>::iterator iterator;
            iterator = std::find(m_files[i]->breakpoints.begin(), m_files[i]->breakpoints.end(), line);

            if (set)
            {
                if (iterator == m_files[i]->breakpoints.end())
                {
                    m_files[i]->breakpoints.push_back(line);
                    if (!m_files[i]->temporary)
                    {
                        m_needsUserSave = true;
                    }
                }
            }
            else
            {
                if (iterator != m_files[i]->breakpoints.end())
                {
                    m_files[i]->breakpoints.erase(iterator);
                    if (!m_files[i]->temporary)
                    {
                        m_needsUserSave = true;
                    }
                }
            }

        }
    }

}

bool Project::ToggleBreakpoint(File* file, unsigned int line)
{

    std::vector<unsigned int>::iterator iterator;
    iterator = std::find(file->breakpoints.begin(), file->breakpoints.end(), line);

    bool set = false;

    if (iterator == file->breakpoints.end())
    {
        file->breakpoints.push_back(line);
        set = true;
    }
    else
    {
        file->breakpoints.erase(iterator);
        set = false;
    }

    if (!file->temporary)
    {
        m_needsUserSave = true;
    }

    return set;

}

void Project::DeleteAllBreakpoints(File* file)
{

    if (!file->breakpoints.empty())
    {
        
        file->breakpoints.clear();
    
        if (!file->temporary)
        {
            m_needsUserSave = true;
        }    

    }

}

wxXmlNode* Project::SaveFileNode(const wxString& baseDirectory, const File* file)
{

    // Temporary nodes should not be saved.
    assert(!file->temporary);

    wxFileName fileName = file->fileName;
    fileName.MakeRelativeTo(baseDirectory);

    wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "file");
    node->AddChild(WriteXmlNode("filename", fileName.GetFullPath()));

    return node;

}

wxXmlNode* Project::SaveUserFileNode(const wxString& baseDirectory, const File* file) const
{

    wxXmlNode* fileNode = NULL;

    if (!file->breakpoints.empty() && !file->temporary)
    {

        wxFileName fileName = file->fileName;
        fileName.MakeRelativeTo(baseDirectory);

        fileNode = new wxXmlNode(wxXML_ELEMENT_NODE, "file");
        fileNode->AddChild(WriteXmlNode("filename", fileName.GetFullPath()));

        for (unsigned int i = 0; i < file->breakpoints.size(); ++i)
        {
            wxXmlNode* breakpointNode = new wxXmlNode(wxXML_ELEMENT_NODE, "breakpoint"); 
            breakpointNode->AddChild(WriteXmlNode("line", file->breakpoints[i]));
            fileNode->AddChild(breakpointNode);
        }

    }

    return fileNode;

}

bool Project::LoadUserFilesNode(const wxString& baseDirectory, wxXmlNode* root)
{

    if (root->GetName() != "files")
    {
        return false;
    }

    wxXmlNode* node = root->GetChildren();
    
    while (node != NULL)
    {
        LoadUserFileNode(baseDirectory, node);
        node = node->GetNext();
    }

    return true;

}

bool Project::LoadUserFileNode(const wxString& baseDirectory, wxXmlNode* root)
{
    
    if (root->GetName() != "file")
    {
        return false;
    }

    wxString fileName;
    std::vector<unsigned int> breakpoints;

    wxXmlNode* node = root->GetChildren();

    while (node != NULL)
    {

        ReadXmlNode(node, wxT("filename"), fileName)
        || LoadBreakpointNode(node, breakpoints);

        node = node->GetNext();
    }
    
    if (!fileName.IsEmpty())
    {

        wxFileName temp = fileName;

        if (temp.IsRelative())
        {
            temp.MakeAbsolute(baseDirectory);
        }

        Project::File* file = NULL;

        for (unsigned int fileIndex = 0; fileIndex < m_files.size(); ++fileIndex)
        {
            if (m_files[fileIndex]->fileName == temp)
            {
                file = m_files[fileIndex];
                break;
            }
        }

        if (file != NULL)
        {
            file->breakpoints = breakpoints;
        }

    }

    return true;

}

bool Project::LoadBreakpointNode(wxXmlNode* root, std::vector<unsigned int>& breakpoints)
{

    if (root->GetName() != "breakpoint")
    {
        return false;
    }

    wxXmlNode* node = root->GetChildren();

    while (node != NULL)
    {

        unsigned int line;

        if (ReadXmlNode(node, wxT("line"), line))
        {
            breakpoints.push_back(line);
        }

        node = node->GetNext();
    
    }

    return true;

}

bool Project::LoadFileNode(const wxString& baseDirectory, wxXmlNode* node)
{

    if (node->GetName() != "file")
    {
        return false;
    }

    File* file = new File;

    file->state         = CodeState_Normal;
    file->scriptIndex   = -1;
    file->temporary     = false;
    file->status        = Status_None;
    file->fileId        = ++s_lastFileId;

    wxXmlNode* child = node->GetChildren();
    wxString fileName;

    while (child != NULL)
    {
        if (ReadXmlNode(child, "filename", fileName))
        {
            
            file->fileName = fileName;

            if (file->fileName.IsRelative())
            {
                file->fileName.MakeAbsolute(baseDirectory);
            }

            wxString temp = file->fileName.GetFullPath();
            int a  =0;

            
        }
        child = child->GetNext();
    }

    m_files.push_back(file);
    return true;

}

std::vector<Project::File*> Project::GetSortedFileList()
{
	struct SortByDisplayName
    {
        bool operator()(const File* file1, const File* file2) const
        {
            return file1->GetDisplayName() < file2->GetDisplayName();
        }
    };

	// Now build a copy of m_files and sort it by file name
	std::vector<Project::File*> fileList(m_files);

	std::sort(fileList.begin(), fileList.end(), SortByDisplayName());

	return fileList;
}

bool Project::SaveGeneralSettings(const wxString& fileName)
{

    // Disable logging.
    wxLogNull logNo;

    wxString baseDirectory = wxFileName(fileName).GetPath();
    
    wxXmlDocument document;
    
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "project");
    document.SetRoot(root);

	// Sort file names before saving, to lessen conflicts when synching to source control
	std::vector<Project::File*> sortedFileList = GetSortedFileList();
	assert(sortedFileList.size() == m_files.size());

	// Now save them out
    for (unsigned int i = 0; i < sortedFileList.size(); ++i)
    {
		File* file = sortedFileList[i];
		assert(file != NULL);

        if (!file->temporary)
        {
            wxXmlNode* node = SaveFileNode(baseDirectory, file);
            root->AddChild(node);
        }
    }

    return document.Save(fileName);

}

bool Project::SaveUserSettings(const wxString& fileName)
{

    // Disable logging.
    wxLogNull logNo;

    wxXmlDocument document;
    
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "project");
    document.SetRoot(root);

#ifndef DEDICATED_PRODUCT_VERSION
    root->AddChild(WriteXmlNode("command",              m_commandLine));
    root->AddChild(WriteXmlNode("working_directory",    m_workingDirectory));
    root->AddChild(WriteXmlNode("symbols_directory",    m_symbolsDirectory));
#endif
    root->AddChild(WriteXmlNode("command_arguments",    m_commandArguments));

    // Add the source control settings.
    
    wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "scc");

    node->AddChild(WriteXmlNode("provider", m_sccProvider));
    node->AddChild(WriteXmlNode("user", m_sccUser));
    node->AddChild(WriteXmlNode("project_name", m_sccProjName));
    node->AddChild(WriteXmlNode("local_path", m_sccLocalPath));
    node->AddChild(WriteXmlNode("aux_project_path", m_sccAuxProjPath));

    root->AddChild(node);

    wxString baseDirectory = wxFileName(fileName).GetPath();

    wxXmlNode* filesNode = NULL;

    for (unsigned int fileIndex = 0; fileIndex < m_files.size(); ++fileIndex)
    {
        const File* file = m_files[fileIndex];
        wxXmlNode* node = SaveUserFileNode(baseDirectory, file);
        if (node != NULL)
        {
            if (filesNode == NULL)
            {
                filesNode = new wxXmlNode(wxXML_ELEMENT_NODE, "files");
            }
            filesNode->AddChild(node);
        }
    }

    if (filesNode != NULL)
    {
        root->AddChild(filesNode);
    }

    return document.Save(fileName);

}

bool Project::LoadUserSettings(const wxString& fileName)
{

    // Disable logging.
    wxLogNull logNo;

    wxXmlDocument document;

    if (!document.Load(fileName))
    {
        return false;
    }

    wxXmlNode* root = document.GetRoot();
    
    if (root->GetName() != "project")
    {
        return false;
    }

    wxString baseDirectory = wxFileName(fileName).GetPath();

    wxXmlNode* node = root->GetChildren();
    
    while (node != NULL)
    {

           ReadXmlNode(node, "command_arguments",   m_commandArguments)
#ifndef DEDICATED_PRODUCT_VERSION
        || ReadXmlNode(node, "command",             m_commandLine)
        || ReadXmlNode(node, "working_directory",   m_workingDirectory)
        || ReadXmlNode(node, "symbols_directory",   m_symbolsDirectory)
#endif
        || LoadSccNode(node)
        || LoadUserFilesNode(baseDirectory, node);
        
        node = node->GetNext();

    }

    return true;

}

bool Project::LoadGeneralSettings(const wxString& fileName)
{

    // Disable logging.
    wxLogNull logNo;

    wxString baseDirectory = wxFileName(fileName).GetPath();

    wxXmlDocument document;

    if (!document.Load(fileName))
    {
        return false;
    }

    wxXmlNode* root = document.GetRoot();
    
    if (root->GetName() != "project")
    {
        return false;
    }
    
    wxXmlNode* node = root->GetChildren();
    
    while (node != NULL)
    {
        LoadFileNode(baseDirectory, node);
        node = node->GetNext();
    }

    return true;

}

wxString Project::CreateTempName()
{
    ++m_tempIndex;
    return wxString::Format("Untitled%d", m_tempIndex);
}

bool Project::LoadSccNode(wxXmlNode* root)
{

    if (root->GetName() != "scc")
    {
        return false;
    }

    wxXmlNode* node = root->GetChildren();
    
    while (node != NULL)
    {
           ReadXmlNode(node, "provider",          m_sccProvider)
        || ReadXmlNode(node, "user",              m_sccUser)
        || ReadXmlNode(node, "project_name",      m_sccProjName)
        || ReadXmlNode(node, "local_path",        m_sccLocalPath)
        || ReadXmlNode(node, "aux_project_path",  m_sccAuxProjPath);
        
        node = node->GetNext();

    }

    return true;

}