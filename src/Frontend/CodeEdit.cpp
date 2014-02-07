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

#include "CodeEdit.h"
#include "FontColorSettings.h"
#include "EditorSettings.h"
#include "ToolTipWindow.h"
#include "AutoCompleteManager.h"
#include "Tokenizer.h"
#include "Bitmaps.h"

#include "res/functionicon.xpm"
#include "res/classicon.xpm"

#include <algorithm>

BEGIN_EVENT_TABLE( CodeEdit, wxScintilla )

    EVT_LEAVE_WINDOW(               CodeEdit::OnMouseLeave)
    EVT_KILL_FOCUS(                 CodeEdit::OnKillFocus)
    EVT_SCI_CHARADDED(  wxID_ANY,   CodeEdit::OnCharAdded)
    EVT_SCI_CHANGE(     wxID_ANY,   CodeEdit::OnChange)
    EVT_SCI_MODIFIED(   wxID_ANY,   CodeEdit::OnModified)

END_EVENT_TABLE()

CodeEdit::CodeEdit()
{

    // The minimum number of characters that must be typed before autocomplete
    // is displayed for global symbols. We impose a minimum so that autocomplete
    // doesn't popup too much.
    m_minAutoCompleteLength = 2;
    m_autoCompleteManager   = NULL;

    m_tipWindow             = NULL;
    m_autoIndented          = false;

    m_enableAutoComplete    = true;
    m_lineMappingDirty      = true;

}

void CodeEdit::SetFontColorSettings(const FontColorSettings& settings)
{

    // For some reason StyleSetFont takes a (non-const) reference, so we need to make
    // a copy before passing it in.
    wxFont font = settings.GetFont();

    SetSelForeground(true, settings.GetColors(FontColorSettings::DisplayItem_Selection).foreColor);
    SetSelBackground(true, settings.GetColors(FontColorSettings::DisplayItem_Selection).backColor);

    StyleSetFont(wxSCI_STYLE_DEFAULT, font);
    StyleClearAll();

    font = settings.GetFont(FontColorSettings::DisplayItem_Default);
    StyleSetFont(wxSCI_LUA_DEFAULT,                 font);
    StyleSetFont(wxSCI_LUA_IDENTIFIER,              font);
    StyleSetForeground(wxSCI_LUA_DEFAULT,           settings.GetColors(FontColorSettings::DisplayItem_Default).foreColor);
    StyleSetBackground(wxSCI_LUA_DEFAULT,           settings.GetColors(FontColorSettings::DisplayItem_Default).backColor);
    StyleSetForeground(wxSCI_STYLE_DEFAULT,         settings.GetColors(FontColorSettings::DisplayItem_Default).foreColor);
    StyleSetBackground(wxSCI_STYLE_DEFAULT,         settings.GetColors(FontColorSettings::DisplayItem_Default).backColor);
    StyleSetForeground(wxSCI_LUA_IDENTIFIER,        settings.GetColors(FontColorSettings::DisplayItem_Default).foreColor);
    StyleSetBackground(wxSCI_LUA_IDENTIFIER,        settings.GetColors(FontColorSettings::DisplayItem_Default).backColor);

    font = settings.GetFont(FontColorSettings::DisplayItem_Comment);
    StyleSetFont(wxSCI_LUA_COMMENT,                 font);
    StyleSetFont(wxSCI_LUA_COMMENTLINE,             font);
    StyleSetFont(wxSCI_LUA_COMMENTDOC,              font);
    StyleSetForeground(wxSCI_LUA_COMMENT,           settings.GetColors(FontColorSettings::DisplayItem_Comment).foreColor);
    StyleSetBackground(wxSCI_LUA_COMMENT,           settings.GetColors(FontColorSettings::DisplayItem_Comment).backColor);
    StyleSetForeground(wxSCI_LUA_COMMENTLINE,       settings.GetColors(FontColorSettings::DisplayItem_Comment).foreColor);
    StyleSetBackground(wxSCI_LUA_COMMENTLINE,       settings.GetColors(FontColorSettings::DisplayItem_Comment).backColor);
    StyleSetForeground(wxSCI_LUA_COMMENTDOC,        settings.GetColors(FontColorSettings::DisplayItem_Comment).foreColor);
    StyleSetBackground(wxSCI_LUA_COMMENTDOC,        settings.GetColors(FontColorSettings::DisplayItem_Comment).backColor);

    font = settings.GetFont(FontColorSettings::DisplayItem_Keyword);
    StyleSetFont(wxSCI_LUA_WORD2,                   font);
    StyleSetForeground(wxSCI_LUA_WORD2,             settings.GetColors(FontColorSettings::DisplayItem_Keyword).foreColor);
    StyleSetBackground(wxSCI_LUA_WORD2,             settings.GetColors(FontColorSettings::DisplayItem_Keyword).backColor);

    font = settings.GetFont(FontColorSettings::DisplayItem_Operator);
    StyleSetFont(wxSCI_LUA_OPERATOR,                font);
    StyleSetForeground(wxSCI_LUA_OPERATOR,          settings.GetColors(FontColorSettings::DisplayItem_Operator).foreColor);
    StyleSetBackground(wxSCI_LUA_OPERATOR,          settings.GetColors(FontColorSettings::DisplayItem_Operator).backColor);

    font = settings.GetFont(FontColorSettings::DisplayItem_String);
    StyleSetFont(wxSCI_LUA_STRING,                  font);
    StyleSetForeground(wxSCI_LUA_STRING,            settings.GetColors(FontColorSettings::DisplayItem_String).foreColor);
    StyleSetBackground(wxSCI_LUA_STRING,            settings.GetColors(FontColorSettings::DisplayItem_String).backColor);
    StyleSetFont(wxSCI_LUA_STRINGEOL,               font);
    StyleSetForeground(wxSCI_LUA_STRINGEOL,         settings.GetColors(FontColorSettings::DisplayItem_String).foreColor);
    StyleSetBackground(wxSCI_LUA_STRINGEOL,         settings.GetColors(FontColorSettings::DisplayItem_String).backColor);
    StyleSetFont(wxSCI_LUA_LITERALSTRING,           font);
    StyleSetForeground(wxSCI_LUA_LITERALSTRING,     settings.GetColors(FontColorSettings::DisplayItem_String).foreColor);
    StyleSetBackground(wxSCI_LUA_LITERALSTRING,     settings.GetColors(FontColorSettings::DisplayItem_String).backColor);
    StyleSetFont(wxSCI_LUA_CHARACTER,               font);
    StyleSetForeground(wxSCI_LUA_CHARACTER,         settings.GetColors(FontColorSettings::DisplayItem_String).foreColor);
    StyleSetBackground(wxSCI_LUA_CHARACTER,         settings.GetColors(FontColorSettings::DisplayItem_String).backColor);

    font = settings.GetFont(FontColorSettings::DisplayItem_Number);
    StyleSetFont(wxSCI_LUA_NUMBER,                  font);
    StyleSetForeground(wxSCI_LUA_NUMBER,            settings.GetColors(FontColorSettings::DisplayItem_Number).foreColor);
    StyleSetBackground(wxSCI_LUA_NUMBER,            settings.GetColors(FontColorSettings::DisplayItem_Number).backColor);

    StyleSetSize(wxSCI_STYLE_LINENUMBER, font.GetPointSize());

    // Set the caret color as the inverse of the background color so it's always visible.
    SetCaretForeground( GetInverse(settings.GetColors(FontColorSettings::DisplayItem_Default).backColor) );

}

void CodeEdit::SetAutoCompleteManager(const AutoCompleteManager* autoCompleteManager)
{
    m_autoCompleteManager = autoCompleteManager;
}

void CodeEdit::SetEditorSettings(const EditorSettings& settings)
{

    m_indentationSize = settings.GetIndentSize();

    SetIndent(m_indentationSize);
    SetTabWidth(m_indentationSize);
    
    bool useTabs = settings.GetUseTabs();
    bool showWhiteSpace = settings.GetShowWhiteSpace();

    SetUseTabs(useTabs);
    SetTabIndents(useTabs);
    SetBackSpaceUnIndents(useTabs);
    SetViewWhiteSpace(showWhiteSpace);

    if (settings.GetShowLineNumbers())
    {

        // Figure out how wide the margin needs to be do display
        // the most number of linqes we'd reasonbly have.
        int marginSize = TextWidth(wxSCI_STYLE_LINENUMBER, "_99999");
        SetMarginWidth(0, marginSize);

        SetMarginType(0,wxSCI_MARGIN_NUMBER);

    }
    else
    {
        SetMarginWidth(0, 0);
    }    

    m_enableAutoComplete = settings.GetEnableAutoComplete();

}    

void CodeEdit::SetDefaultLexer()
{

    SetLexer(wxSCI_LEX_NULL);

    SetKeyWords(1, "");

    // Set the caret width to match MSVC.
    SetCaretWidth(2);

    // Set the marker bitmaps.
    MarkerDefineBitmap(Marker_Breakpoint,  wxMEMORY_BITMAP(Breakpoint_png) );
    MarkerDefineBitmap(Marker_CurrentLine, wxMEMORY_BITMAP(Currentline_png) );
    MarkerDefineBitmap(Marker_BreakLine,   wxMEMORY_BITMAP(Breakline_png) );

    // Setup the dwell time before a tooltip is displayed.
    SetMouseDwellTime(300);

    SetMarginSensitive(1, true);
    SetMarginType(1, wxSCI_MARGIN_SYMBOL);

    // Set the autocomplete icons.

    wxColour maskColor(0xFF, 0x9B, 0x77);

    wxBitmap functionIcon(functionicon, wxBITMAP_TYPE_XPM);
    functionIcon.SetMask(new wxMask(functionicon, maskColor));
    
    RegisterImage(AutoCompleteManager::Type_Function, functionIcon);

    wxBitmap classIcon(classicon, wxBITMAP_TYPE_XPM);
    classIcon.SetMask(new wxMask(classicon, maskColor));
    
    RegisterImage(AutoCompleteManager::Type_Class, classIcon);

}

void CodeEdit::SetLuaLexer()
{

    SetDefaultLexer();

    SetLexer(wxSCI_LEX_LUA);

    const char* keywords =
        "and       break     do        else      elseif "
        "end       false     for       function  if "
        "in        local     nil       not       or "
        "repeat    return    then      true      until     while ";

    SetKeyWords(1, keywords);

}

bool CodeEdit::Untabify()
{

    wxString text = GetText();

    if (Untabify(text))
    {
        SetText(text);
        return true;
    }

    return false;

}

bool CodeEdit::UntabifySelection()
{

    wxString text = GetSelectedText();

    if (Untabify(text))
    {
        ReplaceSelection(text);
        return true;
    }

    return false;

}

bool CodeEdit::Untabify(wxString& text) const
{

    // wxString::Replace is very slow with a big string because the operation
    // is performed in place (which requires a lot of copying). Instead we use
    // a different method with a second string.

    assert(m_indentationSize < 32);

    char indentation[32];
    memset(indentation, ' ', 32);
    indentation[m_indentationSize] = 0;

    wxString result;
    result.reserve(text.Length());

    unsigned int numTabs = 0;

    for (unsigned int i = 0; i < text.Length(); ++i)
    {

        if (text[i] == '\t')
        {
            result += indentation;
            ++numTabs;
        }
        else
        {
            result += text[i];
        }

    }

    if (numTabs > 0)
    {
        text = result;
        return true;
    }
    return false;

}

void CodeEdit::CommentSelection()
{

    if (GetLexer() == wxSCI_LEX_LUA)
    {
        CommentSelection("--");
    }

}

void CodeEdit::CommentSelection(const wxString& comment)
{


    if (GetSelectionStart() < GetSelectionEnd())
    {
  
        int startLine = LineFromPosition(GetSelectionStart());
        int endLine   = LineFromPosition(GetSelectionEnd());

        // Find the minimum indentation level for all of the selected lines.

        int minIndentation = INT_MAX;

        for (int i = startLine; i <= endLine; ++i)
        {
            
            wxString lineText = GetLine(i);
            int firstNonWhitespace = GetFirstNonWhitespace(lineText);

            if (firstNonWhitespace != -1)
            {
                minIndentation = wxMin(firstNonWhitespace, minIndentation);
            }

        }

        // Insert the comment string on each non-blank line.

        wxString result;

        for (int i = startLine; i <= endLine; ++i)
        {

            wxString lineText = GetLine(i);

            if (GetFirstNonWhitespace(lineText) != -1)
            {
                lineText.insert(minIndentation, comment);
            }

            result += lineText;

        }

        SetTargetStart(PositionFromLine(startLine));
        SetTargetEnd(PositionFromLine(endLine + 1));

        ReplaceTarget(result);

        // Select the replaced text.
        SetSelectionStart(GetTargetStart());
        SetSelectionEnd(GetTargetEnd() - 1);
    
    }

}

void CodeEdit::UncommentSelection()
{

    if (GetLexer() == wxSCI_LEX_LUA)
    {
        UncommentSelection("--");
    }

}

void CodeEdit::UncommentSelection(const wxString& commentString)
{

    if (GetSelectionStart() < GetSelectionEnd())
    {

        int startLine = LineFromPosition(GetSelectionStart());
        int endLine   = LineFromPosition(GetSelectionEnd());

        wxString result;
        
        for (int i = startLine; i <= endLine; ++i)
        {

            wxString lineText = GetLine(i);

            unsigned int c = GetFirstNonWhitespace(lineText);

            if (c != -1 && lineText.compare(c, commentString.Length(), commentString) == 0)
            {
                lineText.erase(c, commentString.Length());
            }

            result += lineText;

        }

        SetTargetStart(PositionFromLine(startLine));
        SetTargetEnd(PositionFromLine(endLine + 1));

        ReplaceTarget(result);

        // Select the replaced text.
        SetSelectionStart(GetTargetStart());
        SetSelectionEnd(GetTargetEnd() - 1);

    }

}

unsigned int CodeEdit::GetFirstNonWhitespace(const wxString& text) const
{

    for (unsigned int c = 0; c < text.Length(); ++c)
    {
        if (!IsSpace(text[c]))
        {
            return c;
        }
    }

    return -1;

}

void CodeEdit::Recolor()
{
    ClearDocumentStyle();
    int length = GetLength();
    Colourise(0, length);
}

bool CodeEdit::GetHoverText(int position, wxString& result)
{

    int selectionStart = GetSelectionStart();
    int selectionEnd   = GetSelectionEnd();

    if (position >= selectionStart && position < selectionEnd)
    {
        // We're mousing over the selected text.
        result = GetSelectedText();
        return true;
    }

    // We don't use the : character as a joiner since we don't
    // want to evaulate with that.
    return GetTokenFromPosition(position, ".", result);
    
}

bool CodeEdit::GetIsIdentifierChar(char c) const
{
    return isalnum(c) || c == '_';
}

void CodeEdit::ShowToolTip(int position, const wxString& text)
{

    bool showToolTip = true;

    // There doesn't seem to be a way to determine the window that the mouse is inside
    // in wxWidgets. If we port to another platform, we'll need to handle this.

#ifdef __WXMSW__

    POINT cursorPos;
    GetCursorPos(&cursorPos);

    HWND hActiveWindow = WindowFromPoint(cursorPos);
    showToolTip = hActiveWindow == GetHandle();

#else
    
    wxCOMPILE_TIME_ASSERT(0, "Unportable code");

#endif
    
    HideToolTip();

    if (showToolTip)
    {
        m_tipWindow = new ToolTipWindow(this, text);
    }

}

void CodeEdit::HideToolTip()
{
    if (m_tipWindow != NULL)
    {
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }
}

void CodeEdit::OnMouseLeave(wxMouseEvent& event)
{
    HideToolTip();
    event.Skip();
}

void CodeEdit::OnKillFocus(wxFocusEvent& event)
{
    AutoCompCancel();
    HideToolTip();
    event.Skip();
}

void CodeEdit::OnCharAdded(wxScintillaEvent& event)
{

    // Indent the line to the same indentation as the previous line.
    // Adapted from http://scintilla.sourceforge.net/ScintillaUsage.html

    char ch = event.GetKey();

    if  (ch == '\r' || ch == '\n')
    {

        int line        = GetCurrentLine();
        int lineLength  = LineLength(line);

        if (line > 0 && lineLength <= 2)
        {

            wxString buffer = GetLine(line - 1);
                
            for (unsigned int i =  0; i < buffer.Length();  ++i)
            {
                if (buffer[i] != ' ' && buffer[i] != '\t')
                {
                    buffer.Truncate(i);
                    break;
                }
            }

            ReplaceSelection(buffer);
            
            // Remember that we just auto-indented so that the backspace
            // key will un-autoindent us.
            m_autoIndented = true;
            
        }
        
    }
    else if (m_enableAutoComplete && m_autoCompleteManager != NULL)
    {

        // Handle auto completion.

        wxString token;
        
        if (GetTokenFromPosition(GetCurrentPos() - 1, ".:", token))
        {
            StartAutoCompletion(token);
        }

    }

    event.Skip();

}

void CodeEdit::OnChange(wxScintillaEvent& event)
{
    m_lineMappingDirty = true;
    event.Skip();
}

void CodeEdit::OnModified(wxScintillaEvent& event)
{
    
    event.Skip();    

    int linesAdded = event.GetLinesAdded();
        
    // If we're inserting new lines before a line, so we need to move the
    // markers down. Scintilla doesn't do this automatically for the current line.

    if (linesAdded > 0)
    {
    
        unsigned int position = event.GetPosition();
    
        unsigned int line = LineFromPosition(position);
        unsigned int lineStartPosition = PositionFromLine(line);

        if (position == lineStartPosition)
        {
            
            int markers = MarkerGet(line);

            // Delete all of the markers from the line.
            for (int i = 0; i < 32; ++i)
            {
                MarkerDelete(line, i);
            }

            // Add the markers back on the new line.
            MarkerAddSet(line + linesAdded, markers);

        }

    }    

}

bool CodeEdit::GetTokenFromPosition(int position, const wxString& joiners, wxString& token)
{

    if (position != -1)
    {
       
        // Search the text.

        int line = LineFromPosition(position);
        int seek = position - PositionFromLine(line);

        wxString text = GetLine(LineFromPosition(position));

        if (!isalnum(text[seek]) && joiners.Find(text[seek]) == wxNOT_FOUND)
        {
            return false;
        }
        
        // Search from the seek point to the left until we hit a non-alphanumeric which isn't a "."
        // "." must be handled specially so that expressions like player.health are easy to evaluate. 

        int start = seek;

        while (start > 0 && (GetIsIdentifierChar(text[start - 1]) || joiners.Find(text[start - 1]) != wxNOT_FOUND))
        {
            --start;
        }

        // Search from the seek point to the right until we hit a non-alphanumeric

        unsigned int end = seek;

        while (end + 1 < text.Length() && GetIsIdentifierChar(text[end + 1]))
        {
            ++end;
        }

        token = text.SubString(start, end);
        return true;

    }

    return false;

}

void CodeEdit::StartAutoCompletion(const wxString& token)
{

    wxASSERT(m_autoCompleteManager != NULL);

    wxString items;

    // Get the actual prefix of the thing we're trying to match for autocompletion.
    // If the token refers to a member, the prefix is the member name.

    wxString prefix;
    bool member = false;

    if (GetLexer() == wxSCI_LEX_LUA)
    {

        int end1 = token.Find('.', true);

        if (end1 == wxNOT_FOUND)
        {
            end1 = 0;
        }
        else
        {
            // Skip the '.' character.
            ++end1;
            member = true;
        }

        int end2 = token.Find(':', true);

        if (end2 == wxNOT_FOUND)
        {
            end2 = 0;
        }
        else
        {
            // Skip the ':' character.
            ++end2;
            member = true;
        }

        int end = std::max(end1, end2);
        prefix = token.Right( token.Length() - end );

    }
    else
    {
        // No autocompletion when using the default lexer.
        return;
    }

    if (!member && prefix.Length() < m_minAutoCompleteLength)
    {
        // Don't pop up the auto completion if the user hasn't typed in very
        // much yet.
        return;
    }

    m_autoCompleteManager->GetMatchingItems(prefix, member, items);
    
    if (!AutoCompActive() || m_autoCompleteItems != items)
    {

        // Remember the items in the list so that we don't redisplay the list
        // with the same set of items (reduces flickering).
        m_autoCompleteItems = items;

        if (!items.IsEmpty())
        {
            // Show the autocomplete selection list.
            AutoCompShow(prefix.Length(), items);
        }
        else
        {
            // We have no matching items, so hide the autocompletion selection.
            AutoCompCancel();
        }

    }

}

bool CodeEdit::GetIsLineMappingDirty() const
{
    return m_lineMappingDirty;
}

void CodeEdit::SetIsLineMappingDirty(bool lineMappingDirty)
{
    m_lineMappingDirty = lineMappingDirty;
}

wxColor CodeEdit::GetInverse(const wxColor& color)
{

    unsigned char r = color.Red();
    unsigned char g = color.Green();
    unsigned char b = color.Blue();

    return wxColor( r ^ 0xFF, g ^ 0xFF, b ^ 0xFF );

}