#include "wx/wx.h"
#include <string>
#include <stdio.h>
#include "Processus.h"
#include "Scheduler.h"
#include "ProcessusDialog.h"

using namespace std;


int identifiant = 0;

WX_DEFINE_ARRAY_PTR(wxThread*, wxArrayThread);

// Classe pour la création d'un thread
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

    //Méthodes
    virtual void* Entry();// L'exécution du thread commence ici

};

// Classe pour l'initialisation de la fenêtre graphique et pour l'utilisation des sémaphores
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

// Classe pour la création d'une fenêtre graphique
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
// ID pour les contrôles de menu et d'événements
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

// Fonction de type MyApp équivalente à Main
bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    MyFrame* frame = new MyFrame("Simulateur RR & PS");
    frame->Show(true);
    return true;
}

// Constructeur de type MyFrame pour la génération d'une fenêtre 
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
    fileMenu->Append(ID_MNU_QTM, "&Définir le quantum de temps \tAlt-Q", "Set quantum");
    fileMenu->Append(ID_MNU_QUIT, "Quitter\tAlt-X", "Quit this program");
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&Fichier");
    menuBar->Append(helpMenu, "&Aide");
    SetMenuBar(menuBar);
    CreateStatusBar(1);
    SetStatusText("by Moussa Kane and Adama Coulibaly");

    btnStart = new wxButton(this, ID_BTN_START, wxT("Démarrer"),
        wxPoint(20, 20), wxSize(100, 25));
    btnStop = new wxButton(this, ID_BTN_STOP, wxT("Arrêter"),
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

//Procédure pour afficher les traces d'exécution dans une section log
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
    if (mes == "Thread mis en pause." || mes == "Aucun thread à exécuter!" || mes == "Aucun thread à arrêter!"
        || mes == "Thread arrêté." || mes == "Il n'a aucun thread!" || mes == "Thread mis en pause." 
        || mes == "Aucun thread à reprendre!" || mes == "Reprise de l'exécution du Thread." 
        || mes == "Thread mis en pause." || mes == "Thread en cours d'exécution (priorité = %u)." 
        || mes == "Fin d'exécution du thread.")
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

//Procédure pour enregistrer les log génerés
void MyFrame::DoLogRecord(wxLogLevel level,
    const wxString& msg, const wxLogRecordInfo& info)
{
    this->WriteList(msg.ToStdString());
}

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de quitter
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de information
void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
    (
        "Simulateur Round Robin et Priority Scheduling\n"
        "\n"
        "Auteur: Moussa Kane et Adama Coulibaly\n"
        "Professeur: Mr Akinocho\n"
        "Classe: MPSSI-1\n"
        "Année: 2021-2022\n"
        "Ecole: ESMT Senegal"
    ),
        "Information",
        wxOK | wxICON_INFORMATION,
        this);
}

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de ajouter des processus
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
            wxMessageBox(wxString::Format("Processus ajouté à la liste."));
            dlg->ReinitialiserField();
        }
        else
        {
            wxMessageBox(wxString::Format("Veuillez remplir les champs!"));
        }
    }
    dlg->Destroy();
}

//Procédure qui réagit à l'événement qui suit lors du clic au de définir le quantum de temps
void MyFrame::OnQuantum(wxCommandEvent& event)
{
    ProcessusDialog* dlg1 = new ProcessusDialog(this);
    if (dlg1->ShowModal() == wxID_OK)
    {
        this->mythread->myscheduler.quantum = dlg1->getProgramQuantum();
    }
    dlg1->Destroy(); 
}

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de Démarrer
void MyFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{
    MyThread* thread = CreateThread(); 
    if (thread->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogStatus(wxT("Impossible d'exécuter un thread!"));
    }
    else if (thread->myscheduler.liste == NULL)
    {
        wxLogStatus(wxT("Aucun thread à exécuter!"));
    }
}

//Fonction de type thread pour la création d'un thread
MyThread* MyFrame::CreateThread()
{
    if (this->mythread->m_count != 0)
    {
        this->mythread = new MyThread();
    }
    MyThread* thread = this->mythread;

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        wxLogStatus(wxT("Impossible d'exécuter un thread!"));
    }
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de Arrêter
void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    wxThread* toDelete = NULL;
    {
        wxCriticalSectionLocker enter(wxGetApp().m_critsect);

        // stop the last thread
        if (wxGetApp().m_threads.IsEmpty())
        {
            wxLogStatus(wxT("Aucun thread à arrêter!"));
        }
        else
        {
            toDelete = wxGetApp().m_threads.Last();
        }
    }

    if (toDelete)
    {
        toDelete->Kill();
        wxLogStatus(wxT("Thread arrêté."), 1);
    }
}

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de Pause
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

//Procédure qui réagit à l'événement qui suit lors du clic au niveau de Reprendre
void MyFrame::OnResume(wxCommandEvent& WXUNUSED(event))
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    size_t n = 0, count = wxGetApp().m_threads.Count();
    while (n < count && !wxGetApp().m_threads[n]->IsPaused())
        n++;

    if (n == count) {
        wxLogStatus(wxT("Aucun thread à reprendre!"));
    }
    else {
        wxGetApp().m_threads[n]->Resume();
        wxLogStatus(wxT("Reprise de l'exécution du Thread."), 1);
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

//Méthode de type Mythread pour commencer l'exécution après le clic sur le botton démarrer
wxThread::ExitCode MyThread::Entry()
{
    if (this->myscheduler.liste != NULL)
    {
        wxLogMessage("Thread en cours d'exécution (priorité = %u).", GetPriority());
   
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

        wxLogMessage("Fin d'exécution du thread.");
        this->myscheduler.liste = NULL;
        s.liste = NULL;
        return NULL;
    }
    else
    {
        return NULL;
    }
    
}