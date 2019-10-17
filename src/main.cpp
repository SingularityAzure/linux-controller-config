/*
    File: main.cpp
    Author: Philip Haynes
    This is supposed to be a relatively simple program to remap controller inputs on Linux.
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/scrolwin.h>

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
    void OnClose(wxCloseEvent& event);

    void EnableConfigControls(bool enable);
    void RefreshDevices();
    void UpdateJoystickInfo();
};

class ChoiceControl;

class FrameConfig : public wxFrame {
friend ChoiceControl;
    wxDECLARE_EVENT_TABLE();
    wxPanel *panel;
    wxButton *buttonApply;
    wxButton *buttonReset;
    wxScrolledWindow *scrolledWindowMappings;
    wxVector<wxChoice*> choiceAxisMappings;
    wxVector<wxChoice*> choiceButtonMappings;
    jsDevice joystick;
    jsMapping mapping;
public:
    FrameConfig(wxFrame *parent, const jsDevice& device, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    void OnApply(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCloseButton(wxCommandEvent& event);

    void SetChanged(bool changed);
    void GetMapping();
    void SetMapping();
};

class ChoiceControl : public wxChoice {
    wxDECLARE_EVENT_TABLE();
    FrameConfig *frameConfig;
    bool axis; // if false, refers to a button
    int index;
public:
    ChoiceControl(wxWindow *parent, int n, wxString *choices,
        FrameConfig *frame, bool isAxis, int controlIndex);

    void OnChanged(wxCommandEvent& event);
};

enum {
    idMenuExit = wxID_EXIT,
    idMenuAbout = wxID_ABOUT,
    idListDevices = 2,
    idButtonRefresh = 3,
    idButtonConfig = 4,
    idButtonClose = 5,
    idButtonApply = 6,
    idButtonReset = 7,
};

wxBEGIN_EVENT_TABLE(FrameMain, wxFrame)
    EVT_MENU(idMenuExit, FrameMain::OnQuit)
    EVT_MENU(idMenuAbout, FrameMain::OnAbout)
    EVT_LISTBOX(idListDevices, FrameMain::OnDeviceSelect)
    EVT_BUTTON(idButtonRefresh, FrameMain::OnRefresh)
    EVT_BUTTON(idButtonConfig, FrameMain::OnConfig)
    EVT_CLOSE(FrameMain::OnClose)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(FrameConfig, wxFrame)
    EVT_CLOSE(FrameConfig::OnClose)
    EVT_BUTTON(idButtonClose, FrameConfig::OnCloseButton)
    EVT_BUTTON(idButtonApply, FrameConfig::OnApply)
    EVT_BUTTON(idButtonReset, FrameConfig::OnReset)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(ChoiceControl, wxChoice)
    EVT_CHOICE(wxID_ANY, ChoiceControl::OnChanged)
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
    FrameConfig *frame = new FrameConfig(this, joysticks[joystick]);

    frame->Show();
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
    Close();
}

void FrameMain::OnAbout(wxCommandEvent &event) {
    wxMessageBox("This is a gamepad/joystick controller configuration tool for Linux!\nAuthor: Philip Haynes\nVersion: 0.0", "About", wxOK | wxICON_NONE, this);
}

void FrameMain::OnClose(wxCloseEvent &event) {
    if (event.CanVeto()) {
        wxWindowList &windows = GetChildren();
        for (wxWindow* w : windows) {
            if (!w->GetClassInfo()->IsKindOf(GetClassInfo()))
                continue; // We only care about other wxFrames
            if (!w->Close()) {
                event.Veto();
                return;
            }
        }
    }
    Destroy();
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

FrameConfig::FrameConfig(wxFrame *parent, const jsDevice &device, const wxPoint &position, const wxSize &size) :
wxFrame(parent, wxID_ANY, wxString::Format("Configuring %s", device.name), position, size),
joystick(device)
{
    SetIcon(wxICON(icon));

    panel = new wxPanel(this, wxID_ANY);

    wxSizer *sizerV = new wxBoxSizer(wxVERTICAL);

    scrolledWindowMappings = new wxScrolledWindow(panel, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxVSCROLL);

    sizerV->Add(scrolledWindowMappings, wxSizerFlags(1).Expand().Border());

    wxSizer *sizerButtons = new wxBoxSizer(wxHORIZONTAL);

    wxButton *buttonClose = new wxButton(panel, idButtonClose, "Close");
    buttonApply = new wxButton(panel, idButtonApply, "Apply");
    buttonReset = new wxButton(panel, idButtonReset, "Reset");

    sizerButtons->Add(buttonClose, wxSizerFlags(2).Expand().Border());
    sizerButtons->Add(buttonReset, wxSizerFlags(2).Expand().Border());
    sizerButtons->Add(buttonApply, wxSizerFlags(3).Expand().Border());

    sizerV->Add(sizerButtons, wxSizerFlags(0).Expand().Border());

    panel->SetSizer(sizerV);

    SetMinClientSize(panel->GetBestSize());

    GetMapping();
}

void FrameConfig::OnCloseButton(wxCommandEvent &event) {
    Close();
}

void FrameConfig::OnClose(wxCloseEvent &event) {
    if (event.CanVeto() && buttonApply->IsEnabled()) {
        int ret = wxMessageBox("Do you want to apply your changes?", "Confirm",
                               wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT);
        if (ret == wxYES) {
            SetMapping();
        } else if (ret == wxCANCEL) {
            event.Veto();
            return;
        }
    }

    Destroy();
}

void FrameConfig::OnReset(wxCommandEvent &event) {
    GetMapping();
}

void FrameConfig::OnApply(wxCommandEvent &event) {
    SetMapping();
}

void FrameConfig::SetChanged(bool changed) {
    buttonApply->Enable(changed);
    buttonReset->Enable(changed);
}

void FrameConfig::GetMapping() {
    SetChanged(false);
    int ret = jsMappingGet(&mapping, &joystick);
    if (ret != JS_OK) {
        wxLogError(wxString::Format("Failed to get mapping: %s", jsErrorString(ret)));
        Close(true);
    }
    wxWindow *windowMappings = scrolledWindowMappings->GetTargetWindow();
    for (wxWindow* w : windowMappings->GetChildren()) {
        w->Destroy();
    }
    choiceAxisMappings.clear();
    choiceButtonMappings.clear();
    wxSizer *sizerH1 = new wxBoxSizer(wxHORIZONTAL);
    scrolledWindowMappings->SetSizer(sizerH1);

    wxSizer *sizerAxisMaps = new wxBoxSizer(wxVERTICAL);
    wxSizer *sizerButtonMaps = new wxBoxSizer(wxVERTICAL);

    sizerH1->Add(sizerAxisMaps, wxSizerFlags().Expand().Border());
    sizerH1->Add(sizerButtonMaps, wxSizerFlags().Expand().Border());

    wxString axisChoices[25];
    for (int i = 0; i < 24; i++) {
        axisChoices[i] = jsMapStrings[i];
    }
    axisChoices[24] = jsMapStrings[JS_TOTAL_MAP_STRINGS-1];
    for (int i = 0; i < joystick.numAxes; i++) {
        wxSizer *sizerH2 = new wxBoxSizer(wxHORIZONTAL);
        sizerH2->Add(new wxStaticText(windowMappings, wxID_ANY,
            wxString::Format("Axis %i", i)),
            wxSizerFlags(1).Border().Expand());
        wxChoice *choice = new ChoiceControl(windowMappings,
            25, axisChoices, this, true, i);
        int cur = jsMapActualIndexToString(mapping.axis[i]);
        if (cur > 24) cur = 24;
        choice->SetSelection(cur);
        sizerH2->Add(choice, wxSizerFlags().Expand());
        sizerAxisMaps->Add(sizerH2, wxSizerFlags().Expand());
        choiceAxisMappings.push_back(choice);
    }

    wxString buttonChoices[52];
    for (int i = 0; i < 51; i++) {
        buttonChoices[i] = jsMapStrings[i+24];
    }
    buttonChoices[51] = jsMapStrings[JS_TOTAL_MAP_STRINGS-1];
    for (int i = 0; i < joystick.numButtons; i++) {
        wxSizer *sizerH2 = new wxBoxSizer(wxHORIZONTAL);
        sizerH2->Add(new wxStaticText(windowMappings, wxID_ANY,
            wxString::Format("Button %i", i)),
            wxSizerFlags(1).Border().Expand());
        wxChoice *choice = new ChoiceControl(windowMappings,
            52, buttonChoices, this, false, i);
        int cur = jsMapActualIndexToString(mapping.button[i])-24;
        if (cur > 51) cur = 51;
        choice->SetSelection(cur);
        sizerH2->Add(choice, wxSizerFlags().Expand());
        sizerButtonMaps->Add(sizerH2, wxSizerFlags().Expand());
        choiceButtonMappings.push_back(choice);
    }
    scrolledWindowMappings->SetScrollbars(0, choiceButtonMappings[0]->GetSize().y, 0, wxMax((int)joystick.numAxes, (int)joystick.numButtons), 0, 0);
    sizerH1->SetSizeHints(panel);
    SetMinClientSize(panel->GetBestSize());
}

void FrameConfig::SetMapping() {
    int ret = jsMappingSet(&mapping, &joystick);
    if (ret != JS_OK) {
        wxLogError(wxString::Format("Failed to set mapping: %s", jsErrorString(ret)));
        Close(true);
    }
    SetChanged(false);
}

ChoiceControl::ChoiceControl(wxWindow *parent, int n, wxString *choices,
    FrameConfig *frame, bool isAxis, int controlIndex) :
wxChoice(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, n, choices),
frameConfig(frame), axis(isAxis), index(controlIndex) {

}

void ChoiceControl::OnChanged(wxCommandEvent &event) {
    frameConfig->SetChanged(true);
    int newVal = jsMapStringIndexToActual(event.GetInt() + (axis ? 0 : 24));
    printf("event.GetInt() = %i, newVal = %i\n", event.GetInt(), newVal);
    if (newVal == JS_INVALID_ARGUMENT) {
        wxLogError("Invalid choice!");
    } else {
        if (axis) {
            frameConfig->mapping.axis[index] = newVal;
        } else {
            frameConfig->mapping.button[index] = newVal;
        }
    }
}
