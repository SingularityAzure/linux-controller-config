/*
    File: main.cpp
    Author: Philip Haynes
    This is supposed to be a relatively simple program to remap controller inputs on Linux.
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "joysticks.h"

#ifndef wxOVERRIDE
    #define wxOVERRIDE override
#endif

#include "icon.xpm"

class App : public wxApp {
public:
    App();

    virtual bool OnInit() wxOVERRIDE;
};

class FrameMain : public wxFrame {
    wxDECLARE_EVENT_TABLE();
    wxPanel *panel;
    wxListBox *listDevices;
    wxButton *buttonConfig;
    wxStaticText *textJoystickInfo;
    int numJoysticks = 0;
    jsDevice joysticks[32];
public:
    FrameMain(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    void OnConfig(wxCommandEvent& event);
    void OnDeviceSelect(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void EnableConfigControls(bool enable);
    void RefreshDevices();
    void UpdateJoystickInfo();
};

enum {
    idMenuExit = wxID_EXIT,
    idMenuAbout = wxID_ABOUT,
    idListDevices = 2,
    idButtonRefresh = 3,
    idButtonConfig = 4,
};

wxBEGIN_EVENT_TABLE(FrameMain, wxFrame)
    EVT_MENU(idMenuExit, FrameMain::OnQuit)
    EVT_MENU(idMenuAbout, FrameMain::OnAbout)
    EVT_LISTBOX(idListDevices, FrameMain::OnDeviceSelect)
    EVT_BUTTON(idButtonRefresh, FrameMain::OnRefresh)
    EVT_BUTTON(idButtonConfig, FrameMain::OnConfig)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(App);

App::App() {}

bool App::OnInit() {
    if (!wxApp::OnInit()) return false;

    FrameMain *frame = new FrameMain("Controller Config");

    frame->Show();

    return true;
}

FrameMain::FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
wxFrame((wxFrame*)nullptr, wxID_ANY, title, pos, size) {
    SetIcon(wxICON(icon));

#ifndef wxUSE_MENUS
#error "Get that shit out of here."
#endif
    wxMenu *menuFile = new wxMenu;
    menuFile->AppendSeparator();
    menuFile->Append(idMenuExit);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(idMenuAbout);

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    CreateStatusBar();

    panel = new wxPanel(this, wxID_ANY);

    wxButton *buttonRefresh = new wxButton(panel, idButtonRefresh, "Refresh");
    buttonRefresh->SetWindowStyle(wxBU_EXACTFIT);

    listDevices = new wxListBox(panel, idListDevices);
    listDevices->SetMinClientSize(wxSize(400, 200));

    buttonConfig = new wxButton(panel, idButtonConfig, "Config");
    wxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox *staticBoxDevices = new wxStaticBox(panel, wxID_ANY, "Devices");

    wxSizer *sizerV1 = new wxStaticBoxSizer(staticBoxDevices, wxVERTICAL);

    sizerV1->Add(buttonRefresh, wxSizerFlags(0).Expand().Border(wxALL & ~(wxBOTTOM)));
    sizerV1->Add(listDevices, wxSizerFlags(1).Expand().Border());
    sizerV1->Add(buttonConfig, wxSizerFlags(0).Expand().Border(wxALL & ~(wxTOP)));

    wxSizer *sizerV2 = new wxBoxSizer(wxVERTICAL);

    sizerV2->Add(
        new wxStaticText(panel, wxID_ANY, "Device",
            wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL),
        wxSizerFlags(0).Expand().Border()
    );

    textJoystickInfo = new wxStaticText(panel, wxID_ANY, "No device selected");

    sizerV2->Add(textJoystickInfo, wxSizerFlags(0).Border());

    sizerH->Add(sizerV1, wxSizerFlags(3).Expand().Border());
    sizerH->Add(sizerV2, wxSizerFlags(2).Expand().Border());

    panel->SetSizer(sizerH);

    wxSize minSize = panel->GetBestSize() + wxSize(0, GetBestSize().y);
    SetMinSize(minSize);

    RefreshDevices();
}

void FrameMain::OnConfig(wxCommandEvent &event) {
    int joystick = listDevices->GetSelection();
    if (joystick == wxNOT_FOUND) {
        wxLogMessage("This shouldn't be possible!");
    }
    jsMapping mapping;
    int ret = jsMappingGet(&mapping, &joysticks[joystick]);
    if (ret != JS_OK) wxLogMessage(jsErrorString(ret));

}

void FrameMain::OnDeviceSelect(wxCommandEvent &event) {
    if (event.IsSelection()) {
        EnableConfigControls(true);
        UpdateJoystickInfo();
    }
}

void FrameMain::OnRefresh(wxCommandEvent &event) {
    RefreshDevices();
}

void FrameMain::OnQuit(wxCommandEvent &event) {
    for (int i = 0; i < numJoysticks; i++) {
        jsDeviceClean(&joysticks[i]);
    }
    Close(true);
}

void FrameMain::OnAbout(wxCommandEvent &event) {
    wxMessageBox("This is a gamepad/joystick controller configuration tool for Linux!\nAuthor: Philip Haynes\nVersion: 0.0", "About", wxOK | wxICON_NONE, this);
}

void FrameMain::EnableConfigControls(bool enable) {
    buttonConfig->Enable(enable);
}

void FrameMain::RefreshDevices() {
    for (int i = 0; i < numJoysticks; i++) {
        jsDeviceClean(&joysticks[i]);
    }
    numJoysticks = 0;
    listDevices->Clear();
    EnableConfigControls(false);
    UpdateJoystickInfo();
    for (int i = 0; i < 32; i++) {
        if (jsDeviceInit(&joysticks[numJoysticks], i) != JS_OK) {
            continue;
        }
        listDevices->Append(wxString::Format("/dev/input/js%i\t%s", i, joysticks[numJoysticks].name));
        numJoysticks++;
    }
}

void FrameMain::UpdateJoystickInfo() {
    int joystick = listDevices->GetSelection();
    if (joystick == wxNOT_FOUND) {
        textJoystickInfo->SetLabel("No device selected");
    } else {
        jsDevice &device = joysticks[joystick];
        textJoystickInfo->SetLabel(
            wxString::Format("%s\nAxes: %i\nButtons: %i",
                device.name, (int)device.numAxes, (int)device.numButtons)
        );
    }
}
