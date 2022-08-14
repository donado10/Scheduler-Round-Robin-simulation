#include "wx/wx.h"
#include <string>
#include <stdio.h>
#include "Processus.h"
#include "Scheduler.h"
#include "ProcessusDialog.h"

using namespace std;


int identifiant = 0;

WX_DEFINE_ARRAY_PTR(wxThread*, wxArrayThread);

// Classe pour la cr�ation d'un thread
class MyThread : public wxThread
{
public:
    //Attributs
    int m_count;
    Processus myprocess;
    Scheduler myscheduler;

    //Constructeur et Destructeur virtuel
    MyThread();
    virtual ~MyThread();

    //M�thodes
    virtual void* Entry();// L'ex�cution du thread commence ici

};

// Classe pour l'initialisation de la fen�tre graphique et pour l'utilisation des s�maphores
class MyApp : public wxApp
{
public:
    MyApp();
    virtual ~MyApp() {};
    virtual bool OnInit();
    wxCriticalSection m_critsect;
    wxArrayThread m_threads;

    wxSemaphore m_semAllDone;
    bool m_shuttingDown;
};

// Classe pour la cr�ation d'une fen�tre graphique
class MyFrame : public wxFrame, private wxLog
{
public:
    MyFrame(const wxString& title);
    ~MyFrame();
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnConfigure(wxCommandEvent& event);
    void OnQuantum(wxCommandEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnResume(wxCommandEvent& event);
    void WriteList(string mes);
protected:
    virtual void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info);
private:
    wxListBox* logList;
    wxButton* btnStart;
    wxButton* btnStop;
    wxButton* btnPause;
    wxButton* btnResume;
    wxLog* m_oldLogger;  
    MyThread* mythread = new MyThread();
    MyThread* CreateThread();


    wxDECLARE_EVENT_TABLE();
};
// --------------------------------------------------------
// ID pour les contr�les de menu et d'�v�nements
enum
{
    ID_MNU_QUIT = wxID_EXIT,
    ID_MNU_ABOUT = wxID_ABOUT,
    ID_MNU_CONF,
    ID_MNU_QTM,
    ID_LST_LOG,
    ID_BTN_START,
    ID_BTN_STOP,
    ID_BTN_PAUSE,
    ID_BTN_RESUME
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_MNU_QUIT, MyFrame::OnQuit)
EVT_MENU(ID_MNU_ABOUT, MyFrame::OnAbout)
EVT_MENU(ID_MNU_CONF, MyFrame::OnConfigure)
EVT_MENU(ID_MNU_QTM, MyFrame::OnQuantum)
wxEND_EVENT_TABLE()

//Constructeurs pour l'initialisation d'un objet de type MyApp
MyApp::MyApp()
{
    m_shuttingDown = false;
}

IMPLEMENT_APP(MyApp)

// Fonction de type MyApp �quivalente � Main
bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    MyFrame* frame = new MyFrame("Simulateur RR & PS");
    frame->Show(true);
    return true;
}

// Constructeur de type MyFrame pour la g�n�ration d'une fen�tre 
MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(620, 800),
        wxDEFAULT_FRAME_STYLE^ wxRESIZE_BORDER)
{
    m_oldLogger = wxLog::GetActiveTarget();
    SetIcon(wxICON(sample));
    wxMenu* fileMenu = new wxMenu;
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(ID_MNU_ABOUT, "&Information\tF1", "Show about dialog");
    fileMenu->Append(ID_MNU_CONF, "&Ajouter des processus\tAlt-P", "Set process");
    fileMenu->Append(ID_MNU_QTM, "&D�finir le quantum de temps \tAlt-Q", "Set quantum");
    fileMenu->Append(ID_MNU_QUIT, "Quitter\tAlt-X", "Quit this program");
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&Fichier");
    menuBar->Append(helpMenu, "&Aide");
    SetMenuBar(menuBar);
    CreateStatusBar(1);
    SetStatusText("by Moussa Kane and Adama Coulibaly");

    btnStart = new wxButton(this, ID_BTN_START, wxT("D�marrer"),
        wxPoint(20, 20), wxSize(100, 25));
    btnStop = new wxButton(this, ID_BTN_STOP, wxT("Arr�ter"),
        wxPoint(140, 20), wxSize(100, 25));
    btnPause = new wxButton(this, ID_BTN_PAUSE, wxT("Pause"),
        wxPoint(260, 20), wxSize(100, 25));
    btnResume = new wxButton(this, ID_BTN_RESUME, wxT("Reprendre"),
        wxPoint(380, 20), wxSize(100, 25));
    logList = new wxListBox(this, ID_LST_LOG, wxPoint(20, 70), wxSize(560, 600));

    Connect(ID_BTN_START, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MyFrame::OnStart));
    Connect(ID_BTN_STOP, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MyFrame::OnStop));
    Connect(ID_BTN_PAUSE, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MyFrame::OnPause));
    Connect(ID_BTN_RESUME, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MyFrame::OnResume));
    wxLog::SetActiveTarget(this);
}

//Destructeur de type MyFrame
MyFrame::~MyFrame()
{
    wxLog::SetActiveTarget(m_oldLogger);


    // NB: although the OS will terminate all the threads anyhow when the main
    //     one exits, it's good practice to do it ourselves -- even if it's not
    //     completely trivial in this example

    // tell all the threads to terminate: note that they can't terminate while
    // we're deleting them because they will block in their OnExit() -- this is
    // important as otherwise we might access invalid array elements

    {
        wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        // check if we have any threads running first
        const wxArrayThread& threads = wxGetApp().m_threads;
        size_t count = threads.GetCount();

        if (!count)
            return;

        // set the flag indicating that all threads should exit
        wxGetApp().m_shuttingDown = true;
    }

    // now wait for them to really terminate
    wxGetApp().m_semAllDone.Wait();
}

//Proc�dure pour afficher les traces d'ex�cution dans une section log
void MyFrame::WriteList(string mes)
{
    int nl = logList->GetCount();
    
    if (identifiant == 1) { 
        if (mes != "Thread mis en pause.")
        {
            logList->Clear();
            identifiant = 0;
        }
    }

    if (mes == "|-----------------------------------------------------------------------------------------|\n")
    {
        identifiant = 1;
    }
    if (mes == "Thread mis en pause." || mes == "Aucun thread � ex�cuter!" || mes == "Aucun thread � arr�ter!"
        || mes == "Thread arr�t�." || mes == "Il n'a aucun thread!" || mes == "Thread mis en pause." 
        || mes == "Aucun thread � reprendre!" || mes == "Reprise de l'ex�cution du Thread." 
        || mes == "Thread mis en pause." || mes == "Thread en cours d'ex�cution (priorit� = %u)." 
        || mes == "Fin d'ex�cution du thread.")
    {
        wxString wstr = mes;
        wxDateTime wdt;
        wdt.SetToCurrent();
        wstr = wdt.Format(wxT("%Y-%m-%d %H:%M:%S"), wxDateTime::Local) +
            " : " + wstr; // add timestamp
        wxArrayString astr;
        astr.Add(wstr);
        nl = logList->GetCount();
        logList->InsertItems(astr, nl);
    }
    else
    {
        wxString wstr = mes;

        wxArrayString astr;
        astr.Add(wstr);
        nl = logList->GetCount();
        logList->InsertItems(astr, nl);
    }
}

//Proc�dure pour enregistrer les log g�ner�s
void MyFrame::DoLogRecord(wxLogLevel level,
    const wxString& msg, const wxLogRecordInfo& info)
{
    this->WriteList(msg.ToStdString());
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de quitter
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de information
void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
    (
        "Simulateur Round Robin et Priority Scheduling\n"
        "\n"
        "Auteur: Moussa Kane et Adama Coulibaly\n"
        "Professeur: Mr Akinocho\n"
        "Classe: MPSSI-1\n"
        "Ann�e: 2021-2022\n"
        "Ecole: ESMT Senegal"
    ),
        "Information",
        wxOK | wxICON_INFORMATION,
        this);
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de ajouter des processus
void MyFrame::OnConfigure(wxCommandEvent& event)
{
    if (this->mythread->m_count != 0)
    {
        this->mythread = new MyThread();
    }
    ProcessusDialog* dlg = new ProcessusDialog(this, wxID_ANY, _("Ajouter des processus"));
    while (dlg->ShowModal() != wxID_CLOSE)
    {
        this->mythread->myprocess.ProgramName = dlg->getProgramName();
        this->mythread->myprocess.Duration = dlg->getProgramTime();
        this->mythread->myprocess.Priority = dlg->getProgramPriority();
        this->mythread->myprocess.Userid = dlg->getProgramUserid();
        if (dlg->getProgramName() != "" && dlg->getProgramTime() != NULL && dlg->getProgramPriority() != NULL
            && dlg->getProgramUserid() != NULL)
        {
            this->mythread->myprocess.setArrivalTime();
            this->mythread->myscheduler.liste = insererEnFin(this->mythread->myscheduler.liste, this->mythread->myprocess);
            this->mythread->m_count = 0;
            wxMessageBox(wxString::Format("Processus ajout� � la liste."));
            dlg->ReinitialiserField();
        }
        else
        {
            wxMessageBox(wxString::Format("Veuillez remplir les champs!"));
        }
    }
    dlg->Destroy();
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au de d�finir le quantum de temps
void MyFrame::OnQuantum(wxCommandEvent& event)
{
    ProcessusDialog* dlg1 = new ProcessusDialog(this);
    if (dlg1->ShowModal() == wxID_OK)
    {
        this->mythread->myscheduler.quantum = dlg1->getProgramQuantum();
    }
    dlg1->Destroy(); 
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de D�marrer
void MyFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{
    MyThread* thread = CreateThread(); 
    if (thread->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogStatus(wxT("Impossible d'ex�cuter un thread!"));
    }
    else if (thread->myscheduler.liste == NULL)
    {
        wxLogStatus(wxT("Aucun thread � ex�cuter!"));
    }
}

//Fonction de type thread pour la cr�ation d'un thread
MyThread* MyFrame::CreateThread()
{
    if (this->mythread->m_count != 0)
    {
        this->mythread = new MyThread();
    }
    MyThread* thread = this->mythread;

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        wxLogStatus(wxT("Impossible d'ex�cuter un thread!"));
    }
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de Arr�ter
void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    wxThread* toDelete = NULL;
    {
        wxCriticalSectionLocker enter(wxGetApp().m_critsect);

        // stop the last thread
        if (wxGetApp().m_threads.IsEmpty())
        {
            wxLogStatus(wxT("Aucun thread � arr�ter!"));
        }
        else
        {
            toDelete = wxGetApp().m_threads.Last();
        }
    }

    if (toDelete)
    {
        toDelete->Kill();
        wxLogStatus(wxT("Thread arr�t�."), 1);
    }
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de Pause
void MyFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // pause last running thread
    int n = wxGetApp().m_threads.Count() - 1;
    while (n >= 0 && !wxGetApp().m_threads[n]->IsRunning()) n--;

    if (n < 0) {
        wxLogStatus(wxT("Il n'a aucun thread!"));
    }
    else {
        wxGetApp().m_threads[n]->Pause();
        wxLogStatus(wxT("Thread mis en pause."), 1);
    }
}

//Proc�dure qui r�agit � l'�v�nement qui suit lors du clic au niveau de Reprendre
void MyFrame::OnResume(wxCommandEvent& WXUNUSED(event))
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    size_t n = 0, count = wxGetApp().m_threads.Count();
    while (n < count && !wxGetApp().m_threads[n]->IsPaused())
        n++;

    if (n == count) {
        wxLogStatus(wxT("Aucun thread � reprendre!"));
    }
    else {
        wxGetApp().m_threads[n]->Resume();
        wxLogStatus(wxT("Reprise de l'ex�cution du Thread."), 1);
    }
}

//--------------------------------------------------------

//Contructeur de type MyThread 
MyThread::MyThread()
    : wxThread(), myprocess(),myscheduler()
{
    m_count = 0;
    myscheduler.quantum = 1000; // Valeur par defaut 
}

//Destructeur de type Mythread
MyThread::~MyThread()
{

    wxCriticalSectionLocker locker(wxGetApp().m_critsect);

    wxArrayThread& threads = wxGetApp().m_threads;
    if (threads != NULL)
    {
        threads.Remove(this);
    }

    if (threads.IsEmpty())
    {
        if (wxGetApp().m_shuttingDown)
        {
            wxGetApp().m_shuttingDown = false;

            wxGetApp().m_semAllDone.Post();
        }
    }
}

//M�thode de type Mythread pour commencer l'ex�cution apr�s le clic sur le botton d�marrer
wxThread::ExitCode MyThread::Entry()
{
    if (this->myscheduler.liste != NULL)
    {
        wxLogMessage("Thread en cours d'ex�cution (priorit� = %u).", GetPriority());
   
        Scheduler s;
        s = this->myscheduler;
   
        s.quantum = this->myscheduler.quantum;
        s.liste = sortList(s.liste);
        s.startScheduler();

        {
            wxCriticalSectionLocker locker(wxGetApp().m_critsect);
            if (wxGetApp().m_shuttingDown)
                return NULL;
        }

        wxLogMessage("Fin d'ex�cution du thread.");
        this->myscheduler.liste = NULL;
        s.liste = NULL;
        return NULL;
    }
    else
    {
        return NULL;
    }
    
}