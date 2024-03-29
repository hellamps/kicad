///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dialog_get_component_base__
#define __dialog_get_component_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_SEL_BY_LISTBOX 1000
#define ID_ACCEPT_KEYWORD 1001
#define ID_LIST_ALL 1002
#define ID_EXTRA_TOOL 1003

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_GET_COMPONENT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_GET_COMPONENT_BASE : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticTextName;
		wxTextCtrl* m_textCmpNameCtrl;
		wxStaticText* m_staticTextHistory;
		wxListBox* m_historyList;
		wxButton* m_buttonOK;
		wxButton* m_buttonKW;
		wxButton* m_buttonCancel;
		wxButton* m_buttonList;
		wxButton* m_buttonBrowse;
		
		// Virtual event handlers, overide them in your derived class
		virtual void Accept( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void GetExtraSelection( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_GET_COMPONENT_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 375,210 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DIALOG_GET_COMPONENT_BASE();
	
};

#endif //__dialog_get_component_base__
