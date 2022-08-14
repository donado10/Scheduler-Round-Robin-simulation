#pragma once
#include "wx/wx.h"
 

class ProcessusDialog :
    public wxDialog
{
private:
    //Attributs(Private)
    void onUpdateOK_2(wxUpdateUIEvent& event);
    void OnApply(wxCommandEvent& WXUNUSED);

public:
    //Attributs(Public)
    wxTextCtrl* programName_Box = nullptr;
    wxTextCtrl* programTime_Box = nullptr;
    wxTextCtrl* programPriority_Box = nullptr;
    wxTextCtrl* programUserid_Box = nullptr;
    wxTextCtrl* programQuantum_Box = nullptr;

    //Constructeurs et destructeur
    ProcessusDialog(wxWindow* parent, wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxASCII_STR(wxDialogNameStr));
    ProcessusDialog(wxWindow* parent);
    virtual ~ProcessusDialog();

    //Méthodes
    wxString getProgramName() const{ return programName_Box->GetValue(); };
    int getProgramTime() const { 
        int num;
        num = wxAtoi(programTime_Box->GetValue());
        return num;
    };
    int getProgramPriority() const { 
        int num;
        num = wxAtoi(programPriority_Box->GetValue());
        return num;
    };
    int getProgramUserid() const { 
        int num;
        num = wxAtoi(programUserid_Box->GetValue());
        return num;
    };
    int getProgramQuantum() const { 
        int num;
        num = wxAtoi(programQuantum_Box->GetValue());
        return num;
    };
    void ReinitialiserField();

    DECLARE_EVENT_TABLE()
};

