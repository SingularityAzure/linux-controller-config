/*
    File: main.cpp
    Author: Philip Haynes
    This is supposed to be a relatively simple program to remap controller inputs on Linux.
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#include <wx/scrolwin.h>
#include <wx/mstream.h>
#include <wx/imagtga.h>

#include "joysticks.h"

#ifndef wxOVERRIDE
    #define wxOVERRIDE override
#endif

// These are png files inside of const char arrays.
#include "icon.c"
#include "circle_0.c"
#include "circle_1.c"
#include "circle_2.c"
#include "circle_3.c"
#include "circle_4.c"
#include "circle_5.c"
#include "circle_6.c"
#include "circle_7.c"
#include "circle_8.c"

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
    wxImage imageControls[9]; // 0 to 8 eigths filled
    int numJoysticks = 0;
    jsDevice joysticks[32];
    wxIcon icon;
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
    wxBitmap bitmapControls[9];
    wxScrolledWindow *scrolledWindowMappings;
    wxVector<wxStaticBitmap*> axisStatusImages;
    wxVector<wxStaticBitmap*> buttonStatusImages;
    wxTimer timer;
    jsDevice joystick;
    jsMapping mapping;
public:
    FrameConfig(wxFrame *parent, const jsDevice& device,
        const wxImage imageControls[9],
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize);

    void OnApply(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCloseButton(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

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
    idTimerUpdateController = 8,
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
    EVT_TIMER(idTimerUpdateController, FrameConfig::OnTimer)
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
    wxImage imageIcon;
    wxMemoryInputStream inputStreamIcon(::icon, icon_length);
    imageIcon.AddHandler(new wxPNGHandler);
    imageIcon.LoadFile(inputStreamIcon, wxBITMAP_TYPE_PNG);
    icon.CopyFromBitmap(wxBitmap(imageIcon));
    SetIcon(icon);

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

    wxMemoryInputStream mstreamCircles[9] = {
        {circle_0, circle_0_length},
        {circle_1, circle_1_length},
        {circle_2, circle_2_length},
        {circle_3, circle_3_length},
        {circle_4, circle_4_length},
        {circle_5, circle_5_length},
        {circle_6, circle_6_length},
        {circle_7, circle_7_length},
        {circle_8, circle_8_length},
    };

    for (int i = 0; i < 9; i++) {
        imageControls[i].AddHandler(new wxPNGHandler);
        imageControls[i].LoadFile(mstreamCircles[i], wxBITMAP_TYPE_PNG);
    }
}

void FrameMain::OnConfig(wxCommandEvent &event) {
    int joystick = listDevices->GetSelection();
    if (joystick == wxNOT_FOUND) {
        wxLogMessage("This shouldn't be possible!");
    }
    FrameConfig *frame = new FrameConfig(this, joysticks[joystick], imageControls);
    frame->SetIcon(icon);
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
    wxMessageBox(wxString::Format(
        "This is a gamepad/joystick controller configuration tool for Linux!\n"
        "Author: Philip Haynes\nVersion: %i.%i", VERSION_MAJOR, VERSION_MINOR),
        "About", wxOK | wxICON_NONE, this);
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
        jsMapping mapping;
        int ret = jsMappingLoad(&mapping, &joysticks[numJoysticks]);
        if (ret == JS_OK) {
            jsMappingSet(&mapping, &joysticks[numJoysticks]);
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

FrameConfig::FrameConfig(wxFrame *parent, const jsDevice &device,
    const wxImage imageControls[9], const wxPoint &position, const wxSize &size) :
wxFrame(parent, wxID_ANY, wxString::Format("Configuring %s", device.name), position, size),
bitmapControls{imageControls[0],
               imageControls[1],
               imageControls[2],
               imageControls[3],
               imageControls[4],
               imageControls[5],
               imageControls[6],
               imageControls[7],
               imageControls[8]},
timer(this, idTimerUpdateController),
joystick(device)
{
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

    timer.Start(20);
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

void FrameConfig::OnTimer(wxTimerEvent &event) {
    js_event ev;
    int ret;
    while ((ret = jsEventGet(&ev, &joystick))) {
        if (ret == JS_DEVICE_LOST) {
            wxLogError("Device unplugged!");
            Close(true);
            return;
        }
        if (ev.type == JS_EVENT_BUTTON) {
            buttonStatusImages[ev.number]->SetBitmap(bitmapControls[(ev.value == 1) ? 8 : 0]);
        } else if (ev.type == JS_EVENT_AXIS) {
            int image = (ev.value + 32768) / 7282;
            axisStatusImages[ev.number]->SetBitmap(bitmapControls[image]);
        }
    }
    // Update();
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
    ret = jsMappingLoad(&mapping, &joystick);
    if (ret == JS_OK) {
        jsMappingSet(&mapping, &joystick);
    }
    wxWindow *windowMappings = scrolledWindowMappings->GetTargetWindow();
    for (wxWindow* w : windowMappings->GetChildren()) {
        w->Destroy();
    }
    axisStatusImages.clear();
    buttonStatusImages.clear();
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
    wxChoice *oneChoice = nullptr;
    for (int i = 0; i < joystick.numAxes; i++) {
        wxSizer *sizerH2 = new wxBoxSizer(wxHORIZONTAL);
        sizerH2->Add(new wxStaticText(windowMappings, wxID_ANY,
            wxString::Format("Axis %i", i)),
            wxSizerFlags(1).Center());

        wxStaticBitmap *statusImage =
            new wxStaticBitmap(windowMappings, wxID_ANY, bitmapControls[4]);

        sizerH2->Add(statusImage, wxSizerFlags().Center().Border());
        axisStatusImages.push_back(statusImage);

        wxChoice *choice = new ChoiceControl(windowMappings,
            25, axisChoices, this, true, i);
        int cur = jsMapActualIndexToString(mapping.axis[i]);
        if (cur > 24) cur = 24;
        choice->SetSelection(cur);
        sizerH2->Add(choice, wxSizerFlags().Expand());
        sizerAxisMaps->Add(sizerH2, wxSizerFlags().Expand());
        oneChoice = choice;
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
            wxSizerFlags(1).Center());

        wxStaticBitmap *statusImage =
            new wxStaticBitmap(windowMappings, wxID_ANY, bitmapControls[0]);

        sizerH2->Add(statusImage, wxSizerFlags().Center().Border());
        buttonStatusImages.push_back(statusImage);

        wxChoice *choice = new ChoiceControl(windowMappings,
            52, buttonChoices, this, false, i);
        int cur = jsMapActualIndexToString(mapping.button[i])-24;
        if (cur > 51) cur = 51;
        choice->SetSelection(cur);
        sizerH2->Add(choice, wxSizerFlags().Expand());
        sizerButtonMaps->Add(sizerH2, wxSizerFlags().Expand());
        oneChoice = choice;
    }
    int heightOfChoice = oneChoice != nullptr ? oneChoice->GetSize().y : 32;
    scrolledWindowMappings->SetScrollbars(0, heightOfChoice,
        0, wxMax((int)joystick.numAxes, (int)joystick.numButtons), 0, 0);
    sizerH1->SetSizeHints(panel);
    SetMinClientSize(panel->GetBestSize());
}

void FrameConfig::SetMapping() {
    int ret;
    ret = jsMappingSave(&mapping, &joystick);
    if (ret != JS_OK) {
        wxLogError(wxString::Format("Failed to save mapping: %s", jsErrorString(ret)));
    }
    ret = jsMappingSet(&mapping, &joystick);
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
