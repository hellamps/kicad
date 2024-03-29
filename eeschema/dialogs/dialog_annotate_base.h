///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dialog_annotate_base__
#define __dialog_annotate_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobut.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_ENTIRE_SCHEMATIC 1000
#define ID_CURRENT_PAGE 1001
#define ID_KEEP_ANNOTATION 1002
#define ID_RESET_ANNOTATION 1003
#define ID_SORT_BY_X_POSITION 1004
#define ID_SORT_BY_Y_POSITION 1005
#define ID_CLEAR_ANNOTATION_CMP 1006

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_ANNOTATE_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_ANNOTATE_BASE : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticTextScope;
		wxRadioButton* m_rbEntireSchematic;
		wxRadioButton* m_rbCurrPage;
		wxStaticLine* m_staticline1;
		wxRadioButton* m_rbKeepAnnotation;
		wxRadioButton* m_rbResetAnnotation;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticTextOrder;
		wxRadioButton* m_rbSortBy_X_Position;
		
		wxStaticBitmap* annotate_down_right_bitmap;
		wxRadioButton* m_rbSortBy_Y_Position;
		
		wxStaticBitmap* annotate_right_down_bitmap;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticTextAnnotateAlgo;
		wxRadioButton* m_rbUseIncremental;
		
		wxRadioButton* m_rbUseSheetNum;
		
		wxRadioButton* m_rbStartSheetNumLarge;
		
		wxStaticLine* m_staticline4;
		wxButton* m_btnClose;
		wxButton* m_btnClear;
		wxButton* m_btnApply;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancelClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAnnotationCmpClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnApplyClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_ANNOTATE_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Annotate Schematic"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 432,382 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DIALOG_ANNOTATE_BASE();
	
};

#endif //__dialog_annotate_base__
