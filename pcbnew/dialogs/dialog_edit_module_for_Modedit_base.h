///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dialog_edit_module_for_Modedit_base__
#define __dialog_edit_module_for_Modedit_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_NOTEBOOK 1000
#define ID_BROWSE_3D_LIB 1001
#define ID_REMOVE_3D_SHAPE 1002

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_MODULE_MODULE_EDITOR_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_MODULE_MODULE_EDITOR_BASE : public wxDialog 
{
	private:
		wxBoxSizer* m_GeneralBoxSizer;
		wxBoxSizer* m_PropRightSizer;
	
	protected:
		wxNotebook* m_NoteBook;
		wxPanel* m_PanelProperties;
		wxTextCtrl* m_DocCtrl;
		wxTextCtrl* m_KeywordCtrl;
		
		wxTextCtrl* m_ReferenceCtrl;
		wxButton* m_button4;
		wxTextCtrl* m_ValueCtrl;
		wxButton* m_button5;
		
		wxTextCtrl* m_FootprintNameCtrl;
		
		wxRadioBox* m_AttributsCtrl;
		wxRadioBox* m_AutoPlaceCtrl;
		wxStaticText* m_staticText11;
		wxSlider* m_CostRot90Ctrl;
		wxStaticText* m_staticText12;
		wxSlider* m_CostRot180Ctrl;
		wxStaticText* m_staticTextInfo;
		wxStaticText* m_staticTextNetClearance;
		wxTextCtrl* m_NetClearanceValueCtrl;
		wxStaticText* m_NetClearanceUnits;
		wxStaticLine* m_staticline1;
		wxStaticLine* m_staticline2;
		wxStaticLine* m_staticline3;
		wxStaticText* m_MaskClearanceTitle;
		wxTextCtrl* m_SolderMaskMarginCtrl;
		wxStaticText* m_SolderMaskMarginUnits;
		wxStaticText* m_staticTextSolderPaste;
		wxTextCtrl* m_SolderPasteMarginCtrl;
		wxStaticText* m_SolderPasteMarginUnits;
		wxStaticText* m_staticTextRatio;
		wxTextCtrl* m_SolderPasteMarginRatioCtrl;
		wxStaticText* m_SolderPasteRatioMarginUnits;
		wxPanel* m_Panel3D;
		wxStaticText* m_staticText3Dname;
		wxListBox* m_3D_ShapeNameListBox;
		wxButton* m_buttonBrowse;
		wxButton* m_buttonRemove;
		wxStdDialogButtonSizer* m_sdbSizerStdButtons;
		wxButton* m_sdbSizerStdButtonsOK;
		wxButton* m_sdbSizerStdButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnEditReference( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditValue( wxCommandEvent& event ){ event.Skip(); }
		virtual void On3DShapeNameSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void BrowseAndAdd3DLib( wxCommandEvent& event ){ event.Skip(); }
		virtual void Remove3DShape( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCancelClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOkClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		wxStaticBoxSizer* m_Sizer3DValues;
		DIALOG_MODULE_MODULE_EDITOR_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Module properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 541,540 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DIALOG_MODULE_MODULE_EDITOR_BASE();
	
};

#endif //__dialog_edit_module_for_Modedit_base__
