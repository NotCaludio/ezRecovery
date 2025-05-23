#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/dirdlg.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	panel = new wxPanel(this);
	wxFont headlineFont(wxFontInfo(wxSize(0, 20)).Bold());
	wxStaticText* headlineText = new wxStaticText(panel, wxID_ANY, "Recuperación de archivos", wxPoint(0, 22), wxSize(500, -1), wxALIGN_CENTRE_HORIZONTAL);
	headlineText->SetFont(headlineFont);

	choices.Add("A:"); choices.Add("B:"); choices.Add("C:"); choices.Add("D:"); choices.Add("E:"); choices.Add("F:"); choices.Add("G:"); choices.Add("H:");
	choices.Add("I:"); choices.Add("J:"); choices.Add("K:"); choices.Add("L:"); choices.Add("M:"); choices.Add("N:"); choices.Add("O:"); choices.Add("P:");
	choices.Add("Q:"); choices.Add("R:"); choices.Add("S:"); choices.Add("T:"); choices.Add("U:"); choices.Add("V:"); choices.Add("W:"); choices.Add("X:");
	choices.Add("Y:"); choices.Add("Z:"); 
	PhysicalDrive = new wxChoice(panel, wxID_ANY, wxPoint(200, 200), wxSize(100, -1), choices, wxCB_SORT);
	PhysicalDrive->Select(6);
	PhysicalDrive->Bind(wxEVT_CHOICE, &MainFrame::OnChoiceChanged, this);
	drive = PhysicalDrive->GetStringSelection();

	pathToRecover = new wxButton(panel, wxID_ANY, "Lugar de recuperacion", wxPoint(150, 300), wxSize(200, 50));
	pathToRecover->Bind(wxEVT_BUTTON, &MainFrame::OnPathToRecoverClicked, this);

	selectedPath = new wxStaticText(panel, wxID_ANY, path, wxPoint(0, 350), wxSize(500, -1), wxALIGN_CENTRE_HORIZONTAL);
	selectedPath->SetBackgroundColour(*wxLIGHT_GREY);

	recover = new wxButton(panel, wxID_ANY, "Recuperar", wxPoint(150, 400), wxSize(200, 50));
	recover->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

	gauge = new wxGauge(panel, wxID_ANY, 100, wxPoint(150, 450), wxSize(200, -1));
	gauge->SetValue(0);

	statusMessage = new wxStaticText(panel, wxID_ANY, path, wxPoint(0, 100), wxSize(500, -1), wxALIGN_CENTRE_HORIZONTAL);
	statusMessage->SetBackgroundColour(*wxBLUE);
	CreateStatusBar();

}

void MainFrame::OnButtonClicked(wxCommandEvent& evt) {
	wxLogStatus("Button Clicked");
	gauge->SetValue(25);
	recover->Disable();

	if (!path.empty() && !drive.empty())
		errorCode = recoverMain(path.ToStdString(), drive.ToStdString());
	gauge->SetValue(100);
	switch (errorCode)
	{
	case 0:
		statusMessage->SetBackgroundColour(*wxGREEN);
		statusMessage->SetLabel("Se hizo la recuperacion de archivos correctamente");
		break;
	case 1:
		statusMessage->SetBackgroundColour(*wxRED);
		statusMessage->SetLabel("El disco no existe o no se pudo abrir");
		break;
	default:
		break;
	}

		

}

void MainFrame::OnPathToRecoverClicked(wxCommandEvent& evt) {
	wxLogStatus("Path to Recover Clicked");
	wxDirDialog dialog(this, "Selecciona un folder", "", wxFD_DEFAULT_STYLE| wxDD_DIR_MUST_EXIST);

	if (dialog.ShowModal() == wxID_OK) {
		path = dialog.GetPath();
		wxMessageBox("Folder: " + path, "Folder seleccionado", wxOK | wxICON_INFORMATION);
	}
	selectedPath->SetLabel( path);
	
}

void MainFrame::OnSliderChanged(wxCommandEvent& evt) {
	wxString str = wxString::Format("Slider Value: %d", evt.GetInt());
	wxLogStatus(str);
}
void MainFrame::OnTextChanged(wxCommandEvent& evt) {
	wxString str = wxString::Format("TEXT: %s ", evt.GetString());
	wxLogStatus(str);
}
void MainFrame::OnChoiceChanged(wxCommandEvent& evt) {
	wxString str = wxString::Format("Choice: %s ", evt.GetString());
	drive = PhysicalDrive->GetStringSelection();
	wxLogStatus(str);
}