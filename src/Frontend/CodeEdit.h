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

#ifndef CODE_EDIT_H
#define CODE_EDIT_H

#include <wx/wx.h>
#include <wx/wxscintilla.h>

//
// Forward declarations.
//

class FontColorSettings;
class EditorSettings;
class ToolTipWindow;
class AutoCompleteManager;

/**
 * Code editor component.
 */
class CodeEdit : public wxScintilla
{

    DECLARE_EVENT_TABLE()

public:

    enum Marker
    {
        Marker_Breakpoint = 1,
        Marker_CurrentLine,
        Marker_BreakLine,
    };

    /**
     * Constructor.
     */
    CodeEdit();

    /**
     * Sets the auto completion manager used to supply the editor with data
     * for trying to complete the user's typing.
     */
    void SetAutoCompleteManager(const AutoCompleteManager* autoCompleteManager);

    /**
     * Sets the font and color settings used in the editor.
     */
    void SetFontColorSettings(const FontColorSettings& settings);

    /**
     * Sets the general editor settings.
     */
    void SetEditorSettings(const EditorSettings& settings);

    /**
     * Sets the editor to display default (i.e. no) syntax coloring.
     */
    void SetDefaultLexer();

    /**
     * Sets the editor to display syntax coloring for Lua.
     */
    void SetLuaLexer();

    /**
     * Replaces tabs in the entire document with spaces. Returns true if any
     * replacements were made.
     */
    bool Untabify();

    /**
     * Replaces tabs in the selected text with spaces. Returns true if any
     * replacements were made.
     */
    bool UntabifySelection();

    /**
     * Adds a comment character before each line of the selection based on the
     * document's language.
     */
    void CommentSelection();

    /**
     * Removes a comment character before each line of the selection based on the
     * document's language.
     */
    void UncommentSelection();

    /**
     * Forces recoloring based on the current lexer.
     */
    void Recolor();

    /**
     * Returns the text under the position for the "hovering watch".
     */
    bool GetHoverText(int position, wxString& result);

    /**
     * Shows a tool tip.
     */
    void ShowToolTip(int position, const wxString& text);

    /**
     * Hides a tool tip.
     */
    void HideToolTip();

    /**
     * Called when the mouse pointer leaves the bounds of the window.
     */
    void OnMouseLeave(wxMouseEvent& event);

    /**
     * Called when the window loses keyboard focus.
     */
    void OnKillFocus(wxFocusEvent& event);

    /**
     * Called when a new character is typed in the editor.
     */
    void OnCharAdded(wxScintillaEvent& event);

    /**
     * Called when the contents of the editor change.
     */
    void OnChange(wxScintillaEvent& event);

    /**
     *
     */
    void OnModified(wxScintillaEvent& event);

    /**
     * Returns true if the line mapping is dirty due to modifications.
     */
    bool GetIsLineMappingDirty() const;

    /**
     * Sets whether or not the line mapping is dirty.
     */
    void SetIsLineMappingDirty(bool lineMappingDirty);

private:

    /**
     * Comments the selection using the spefified comment string (i.e. "//")
     */
    void CommentSelection(const wxString& comment);

    /**
     * Uncomments the selection using the spefified comment string (i.e. "//")
     */
    void UncommentSelection(const wxString& comment);

    /**
     * Returns the first non-whitespace character in the text.
     */
    unsigned int GetFirstNonWhitespace(const wxString& text) const;

    /**
     * Returns true if the character can appear in an identifier name.
     */
    bool GetIsIdentifierChar(char c) const;

    /**
     * Gets the complete token that contains the specified position. The joints string
     * is a set of characters which are treated as part of the token when parsing. Typically
     * these would be the scope resolution characters from the language such as '.' and ':'.
     */
    bool GetTokenFromPosition(int position, const wxString& joiners, wxString& token);

    /**
     * Starts the autocompletion operation with the specified token.
     */
    void StartAutoCompletion(const wxString& token);

    /**
     * Replaces tabs in the specified text with spaces. Returns true if any tabs were
     * replaced.
     */
    bool Untabify(wxString& text) const;

    /**
     * Returns the inverse of the specified color. The inverse color is always
     * visible on top of the original color.
     */ 
    wxColor GetInverse(const wxColor& color);

private:

    int                             m_indentationSize;
    ToolTipWindow*                  m_tipWindow;

    bool                            m_autoIndented;

    bool                            m_enableAutoComplete;
    unsigned int                    m_minAutoCompleteLength;
    wxString                        m_autoCompleteItems;

    const AutoCompleteManager*      m_autoCompleteManager;

    bool                            m_lineMappingDirty;

};

#endif