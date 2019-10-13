/*
    File: main.cpp
    Author: Philip Haynes
    This is supposed to be a relatively simple program to remap controller inputs on Linux.
*/

#include <wx/wxprec.h>

#ifndef WK_PRECOMP
    #include <wx/wx.h>
#endif

#ifndef wxOVERRIDE
    #define wxOVERRIDE override
#endif

class App : public wxApp {
    wxLocale locale;
public:
    App();

    virtual bool OnInit() wxOVERRIDE;
};

class FrameMain : public wxFrame {
    wxDECLARE_EVENT_TABLE();
public:
    FrameMain(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

wxBEGIN_EVENT_TABLE(FrameMain, wxFrame)
    EVT_MENU(wxID_EXIT, FrameMain::OnQuit)
    EVT_MENU(wxID_ABOUT, FrameMain::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(App);

App::App() :
locale(wxLANGUAGE_DEFAULT) {}

bool App::OnInit() {
    if (!wxApp::OnInit()) return false;

    FrameMain *frame = new FrameMain("Controller Config");

    frame->Show();

    return true;
}

FrameMain::FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
wxFrame((wxFrame*)nullptr, wxID_ANY, title, pos, size) {
    // SetIcon(wxICON());

#ifndef wxUSE_MENUS
#error "Get that shit out of here."
#endif
    wxMenu *menuFile = new wxMenu
    menuFile->Append(wxID_EXIT, "E&xit\tAlt-X", "Close this program.");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("I dunno what a status bar is...");
}

void FrameMain::OnQuit(wxCommandEvent &event) {
    Close(true);
}

void FrameMain::OnAbout(wxCommandEvent &event) {
    wxMessageBox("This is a gamepad/joystick controller configuration tool for Linux!\nAuthor: Philip Haynes\nVersion: 0.0", "About", wxOK | wxICON_NONE, this);
}
