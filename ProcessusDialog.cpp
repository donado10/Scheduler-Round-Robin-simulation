#include <wx/valtext.h>
#include <wx/valnum.h>
#include "ProcessusDialog.h"

BEGIN_EVENT_TABLE(ProcessusDialog,wxDialog)
	EVT_BUTTON(wxID_APPLY, ProcessusDialog::OnApply)
	EVT_UPDATE_UI(wxID_OK, ProcessusDialog::onUpdateOK_2)
END_EVENT_TABLE()

//Constructeur pour créer une fenêtre de dialogue
ProcessusDialog::ProcessusDialog(wxWindow* parent, wxWindowID id, const wxString& title, 
	const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxDialog(parent,id,title,pos,size,style,name)
{
		wxBoxSizer* mainsizer = new wxBoxSizer(wxVERTICAL); //Création du box principal

		
		wxBoxSizer* namesizer = new wxBoxSizer(wxHORIZONTAL); //Création du box pour le nom


		//label de nom
		wxStaticText* namelabel = new wxStaticText(this, wxID_ANY, _("Nom"));
		namelabel->SetMinSize(wxSize(150, namelabel->GetMinSize().y));
		namesizer->Add(namelabel);

		programName_Box = new wxTextCtrl(this, wxID_ANY); //Box pour la zone de texte
		programName_Box->SetValidator(wxTextValidator(wxFILTER_ALPHANUMERIC | wxFILTER_SPACE));
		namesizer->Add(programName_Box, 1);

		mainsizer->Add(namesizer, 0, wxEXPAND | wxALL, 5);


		wxBoxSizer* timesizer = new wxBoxSizer(wxHORIZONTAL); //Création du box pour le temps

		//label de temps
		wxStaticText* timelabel = new wxStaticText(this, wxID_ANY, _("Temps d'exécution (en ms)"));
		timelabel->SetMinSize(wxSize(150, timelabel->GetMinSize().y));
		timesizer->Add(timelabel);

		programTime_Box = new wxTextCtrl(this, wxID_ANY); //Box pour la zone de temps
		programTime_Box->SetValidator(wxIntegerValidator<int>());
		timesizer->Add(programTime_Box, 1);

		mainsizer->Add(timesizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);


		wxBoxSizer* prioritysizer = new wxBoxSizer(wxHORIZONTAL);//Création du box pour la priorité

		//label de priorité
		wxStaticText* prioritylabel = new wxStaticText(this, wxID_ANY, _("Priorite (> 0 et < 10)"));
		prioritylabel->SetMinSize(wxSize(150, prioritylabel->GetMinSize().y));
		prioritysizer->Add(prioritylabel);

		programPriority_Box = new wxTextCtrl(this, wxID_ANY);//Box pour la zone de priorité
		wxIntegerValidator<int> programPriority_validator;
		programPriority_validator.SetMin(1);
		programPriority_validator.SetMax(9);
		programPriority_Box->SetValidator(programPriority_validator);
		prioritysizer->Add(programPriority_Box, 1);

		mainsizer->Add(prioritysizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

		wxBoxSizer* useridsizer = new wxBoxSizer(wxHORIZONTAL);//Création du box pour l'ID de l'utilisateur

		//label du ID de l'utilisateur
		wxStaticText* useridlabel = new wxStaticText(this, wxID_ANY, _("UserID"));
		useridlabel->SetMinSize(wxSize(150, useridlabel->GetMinSize().y));
		useridsizer->Add(useridlabel);

		programUserid_Box = new wxTextCtrl(this, wxID_ANY);//Box pour la zone de l'ID de l'utilisateur
		programUserid_Box->SetValidator(wxIntegerValidator<int>());
		useridsizer->Add(programUserid_Box, 1);

		mainsizer->Add(useridsizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

		//Création des boutons APPLY et CLOSE
		wxSizer* buttonsizer = CreateButtonSizer(wxAPPLY | wxCLOSE);
		mainsizer->Add(buttonsizer, 0, wxALIGN_RIGHT);

		SetSizer(mainsizer);
		SetMinSize(wxSize(400, 100));
		Fit();
	}

//Constructeur pour créer une fenêtre de dialogue pour le quantum de temps
ProcessusDialog::ProcessusDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Definir le quantum de temps",
	wxDefaultPosition,wxDefaultSize, wxDEFAULT_DIALOG_STYLE, wxASCII_STR(wxDialogNameStr))
{
	wxBoxSizer* mainsizer_2 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* quantumsizer = new wxBoxSizer(wxHORIZONTAL);//Création du box pour le quantum de temps

	//label de quantum
	wxStaticText* quantumlabel = new wxStaticText(this, wxID_ANY, _("Quantum de temps en ms (> 0)"));
	quantumlabel->SetMinSize(wxSize(180, quantumlabel->GetMinSize().y));
	quantumsizer->Add(quantumlabel);

	programQuantum_Box = new wxTextCtrl(this, wxID_ANY);//Box pour la zone de quantum
	wxIntegerValidator<int> programQuantum_validator;
	programQuantum_validator.SetMin(1);
	programQuantum_Box->SetValidator(programQuantum_validator);
	quantumsizer->Add(programQuantum_Box,1);

	mainsizer_2->Add(quantumsizer, 0, wxEXPAND | wxALL, 5);

	//Création des boutons OK et CANCEL
	wxSizer* buttonsizer_2 = CreateButtonSizer(wxOK | wxCANCEL);
	mainsizer_2->Add(buttonsizer_2, 0, wxALIGN_RIGHT);

	SetSizer(mainsizer_2);
	SetMinSize(wxSize(400, 100));
	Fit();
}

ProcessusDialog::~ProcessusDialog()
{
}

//Procédure pour reinitialiser les champs 
void ProcessusDialog::ReinitialiserField()
{
	this->programName_Box->SetValue("");
	this->programTime_Box->SetValue("");
	this->programPriority_Box->SetValue("");
	this->programUserid_Box->SetValue("");
}

void ProcessusDialog::onUpdateOK_2(wxUpdateUIEvent& event)
{
	event.Enable(false);
	if (!programQuantum_Box->GetValue().empty())
	{
		
		event.Enable(true);
		
	}
}

void ProcessusDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal())
		{
			EndModal(wxID_APPLY);
		}
		else
		{
			// don't change return code from event char if it was set earlier
			if (GetReturnCode() == 0)
			{
				SetReturnCode(wxID_APPLY);
				Show(false);
			}
		}
	}
}
