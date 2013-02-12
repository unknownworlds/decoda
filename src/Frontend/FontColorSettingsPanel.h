//=============================================================================
//
// FontColorSettingsPanel.h
// 
// Created by Max McGuire (max@unknownworlds.com)
// Copyright 2007, Unknown Worlds Entertainment
//
//=============================================================================

#ifndef FONT_COLOR_SETTINGS_PANEL_H
#define FONT_COLOR_SETTINGS_PANEL_H

#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>

#include "FontColorSettings.h"

//
// Forward declarations.
//

class StaticTextEx;

/**
 * Panel for selecting font and color settings.
 */
class FontColorSettingsPanel : public wxPanel 
{
	
public:

    /**
     * Constructor.
     */
    FontColorSettingsPanel(wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL);

    /**
     * Finializes the initialization. Should be called after the display items have been added.
     */
    void Initialize();

    /**
     * Called when the user clicks the Select button to pick a font.
     */
    void OnSelectFont(wxCommandEvent& event);

    /**
     * Called when the user clicks the Reset to Defaults button.
     */
    void OnResetToDefaults(wxCommandEvent& event);

    /**
     * Called when the user selects an item in the display items list.
     */
    void OnDisplayItemsListItemSelected(wxCommandEvent& event);

    /**
     * Called when the user selects an item in the foreground color list.
     */
    void OnForeColorChoiceItemSelected(wxCommandEvent& event);

    /**
     * Called when the user selects an item in the background color list.
     */
    void OnBackColorChoiceItemSelected(wxCommandEvent& event);

    /**
     * Called when the user clicks the Custom button for the foreground color.
     */
    void OnForeColorCustom(wxCommandEvent& event);

    /**
     * Called when the user clicks the Custom button for the background color.
     */
    void OnBackColorCustom(wxCommandEvent& event);

    /**
     * Called when the user changes the value in the Bold check box.
     */
    void OnBoldCheck(wxCommandEvent& event);
    
    /**
     * Called when the user changes the value in the Italic check box.
     */
    void OnItalicCheck(wxCommandEvent& event);

    /**
     * Adds a new item to be displayed.
     */
    void SetSettings(const FontColorSettings& settings);

    /**
     * Returns the settings set by the user.
     */
    const FontColorSettings& GetSettings() const;

    DECLARE_EVENT_TABLE()

private:

    /**
     * Updates the control that displays the name of the currently selected font.
     */
    void UpdateSelectFontTextCtrl();

    /**
     * Redraws the window that shows a preview.
     */
    void UpdatePreview();

    /**
     * Returns the index of the default color that matches the specified color, or -1
     * if it doesn't match any of them.
     */
    unsigned int GetMatchingDefaultColor(const wxColor& color) const;

    /**
     * Selects the default color in the choice box. If the index is -1 the custom option
     * is selected.
     */
    void SetColorSelection(wxChoice* colorChoice, unsigned int index);

private:

    enum ID
    {
        ID_SelectFont           = 1,
        ID_DisplayItemsList,
        ID_ForeColorChoice,
        ID_BackColorChoice,
        ID_ForeColorCustom,
        ID_BackColorCustom,
        ID_ResetToDefaults,
        ID_BoldCheck,
        ID_ItalicCheck,
    };

    static const unsigned int   s_numDefaultColors  = 16;
    static const wxString       s_defaultColorName[s_numDefaultColors];
    static const wxColor        s_defaultColor[s_numDefaultColors];

	wxStaticText*       m_staticText3;
	wxTextCtrl*         m_selectedFontTextCtrl;
	wxButton*           m_button6;
	wxStaticText*       m_staticText9;
	wxListBox*          m_displayItemsListBox;
	wxStaticText*       m_staticText12;
	
    wxChoice*           m_foreColorChoice;
    wxChoice*           m_backColorChoice;

    wxCheckBox*         m_boldCheck;
    wxCheckBox*         m_italicCheck;

	wxButton*           m_button4;
	wxStaticText*       m_staticText13;

	wxButton*           m_button5;
    wxButton*           m_button51;
	wxStaticText*       m_staticText10;
	StaticTextEx*       m_previewPanel;

    FontColorSettings   m_settings;
	
};

#endif