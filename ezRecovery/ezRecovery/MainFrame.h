#pragma once
#include <wx/wx.h>
#include "mainRecover.h"
class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	void OnButtonClicked(wxCommandEvent& evt);
	void OnPathToRecoverClicked(wxCommandEvent& evt);
	void OnSliderChanged(wxCommandEvent& evt);
	void OnTextChanged(wxCommandEvent& evt);
	void OnChoiceChanged(wxCommandEvent& evt);
	wxGauge* gauge;
	int errorCode;
	wxString path;
	wxString drive;
	wxStaticText* selectedPath;
	wxStaticText* statusMessage;
	wxButton* recover;
	wxButton* pathToRecover;
	wxPanel* panel;
	wxArrayString choices;
	wxChoice* PhysicalDrive;


};

