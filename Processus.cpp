#include "Processus.h"
using namespace std;

//Variable global pour la gestion 
int Nbre_Processus = 0;

//Constructeur par defaut 
Processus::Processus(): Id{0}, Duration{0},ProgramName{NULL}, Priority{9}, Userid{0}, RemainingTime{0}, suivant{NULL}, 
Nbre_de_passage{0}, waiting_time{0},arrival_time{NULL}
{
}

//Destructeur
Processus::~Processus()
{
}

//Fonction pour initialiser une liste
ListeProcessus initialiser()
{
    return NULL;
}

//Fonction pour inserer en tête dans une liste
ListeProcessus insererEnTete(ListeProcessus ancienneListe, Processus processus)
{
    Processus* ptr_processus;
    ptr_processus = new Processus();
    if (processus.Id == 0)
    {
        ptr_processus->Id = ++Nbre_Processus;
    }
    else
    {
        ptr_processus->Id = processus.Id;
    }
    ptr_processus->Duration = processus.Duration;
    ptr_processus->RemainingTime = ptr_processus->Duration;
    ptr_processus->ProgramName = processus.ProgramName;
    ptr_processus->Priority = processus.Priority;
    ptr_processus->Userid = processus.Userid;
    ptr_processus->suivant = ancienneListe;
    return ptr_processus;
}

//Fonction pour inserer en fin dans une liste
ListeProcessus insererEnFin(ListeProcessus ancienneListe, Processus processus)
{
    Processus* ptr_processus, * p;
    ptr_processus = new Processus();
    if (processus.Id == 0)
    {
        ptr_processus->Id = ++Nbre_Processus;
    }
    else
    {
        ptr_processus->Id = processus.Id;
    }
    ptr_processus->Duration = processus.Duration;
    ptr_processus->RemainingTime = ptr_processus->Duration;
    ptr_processus->ProgramName = processus.ProgramName;
    ptr_processus->Priority = processus.Priority;
    ptr_processus->Userid = processus.Userid;
    ptr_processus->arrival_time = processus.arrival_time;
    ptr_processus->suivant = NULL;

    if (ancienneListe == NULL)
    {
        return ptr_processus;
    }
    else
    {
        p = ancienneListe;
        while (p->suivant != NULL)
        {
            p = p->suivant;
        }
        p->suivant = ptr_processus;
        return ancienneListe;
    }

}

//Procédure pour afficher une liste
void afficherListeProcessus(ListeProcessus liste, Processus *q)
{
    Processus* p;
    p = liste;
    if (liste == NULL)
    {
        cout << "Il y a plus de processus dans la liste d'attente" << endl;
    }
    else
    {
        while (p != NULL)
        {
            if (p != q)
            {
                p->ProcessusEnAttente();
                p = p->suivant;
            }
            else
            {
                p = p->suivant;
            }
        }
    }
    
}

//Fonction pour supprimer le premier élément d'une liste
ListeProcessus supprimer(ListeProcessus ancienneListe)
{
    Processus* q;

    if (ancienneListe != NULL)
    {
        q = ancienneListe;
        ancienneListe = ancienneListe->suivant;
        free(q);
    }
    return ancienneListe;
}

//Fonction pour supprimer un processus dans une liste après avoir donné son ID
ListeProcessus supprimerMilieu(ListeProcessus ancienneListe, int processus_ID)
{
    Processus* q,*p = nullptr;

    if ((ancienneListe->Id == processus_ID) )
        ancienneListe = supprimer(ancienneListe);
    else
    {
        q = ancienneListe;
        while (q->suivant->Id != processus_ID) {
            q = q->suivant;
        }

        if (q->suivant->Id == processus_ID)
        {
            p = q->suivant;
            q->suivant = q->suivant->suivant;
            free(p);
        }
    }

    return ancienneListe;
}

//Fonction pour supprimer tous les éléments d'une liste
ListeProcessus vider(ListeProcessus ancienneListe)
{
    if (ancienneListe == NULL)
    {
        return ancienneListe;
    }
    else
    {
        ancienneListe = supprimer(ancienneListe);
        return vider(ancienneListe);
    }
}

/*Fonction pour trier de manière décroissante les processus de la liste en fonction de leur priorité.
* Cette fonction identifie d'abord le processus ayant la priorité la plus élevé,ensuite il met 
*le processus identifié dans une nouvelle liste puis le supprime de l'ancienne liste.Si deux processus 
* ont la même priorité, il selectionne le processus ayant l'ID la plus faible qui représente l'ordre 
*d'arrivé du processus dans la base.
* Il repéte cette séquence jusqu'à ce que l'ancienne liste soit vide.
*/
ListeProcessus sortList(ListeProcessus ancienneListe)
{
    Processus* p;
    Processus q;
    ListeProcessus newliste;
    newliste = initialiser();

    if (ancienneListe == NULL)
    {
        return ancienneListe;
    }
    
    do
    {
        p = ancienneListe;
        q.Priority = 1;
        if (p->suivant == NULL)
        {
            q = *p;
        }
        while (p->suivant != NULL)
        {
            if ((p->Priority) >= (p->suivant->Priority))
            {
                if ((p->Priority) > (q.Priority))
                {
                    q = *p;
                    p = p->suivant;
                }
                else if ((p->Priority) == (q.Priority))
                {
                    if ((p->Id) < (q.Id))
                    {
                        q = *p;
                        p = p->suivant;
                    }
                    else
                    {
                        p = p->suivant;
                    }
                }
                else
                {
                    p = p->suivant;
                }
            }
            else
            {
                if ((p->suivant->Priority) > (q.Priority))
                {
                    q = *p->suivant;
                    p = p->suivant;
                }
                else if ((p->suivant->Priority) == (q.Priority))
                {
                    if ((p->suivant->Id) < (q.Id))
                    {
                        q = *p;
                        p = p->suivant;
                    }
                    else
                    {
                        p = p->suivant;
                    }
                }
                else
                {
                    p = p->suivant;
                }
            }
        }
        newliste = insererEnFin(newliste, q);
        ancienneListe = supprimerMilieu(ancienneListe, q.Id);
    } while (ancienneListe != NULL);
 
    return newliste;
}

/*Méthode qui appartient à la classe Processus qui affiche les informations d'un processus et fait appel
*à une autre procédure afin d'afficher la liste des processus en attente.
*/
void Processus::AfficherProcessus(Processus* liste)
{
    std::string wstr;
    wxDateTime wdt;
    wdt.SetToCurrent();
    wstr = wdt.Format(wxT("%Y-%m-%d %H:%M:%S"), wxDateTime::Local); // add timestamp

    wxLogMessage("Date et heure: %s\n", wstr);
    wxLogMessage("|----------------------------Processus en cours d'exécution----------------------------|\n");
    wxLogMessage("\n");
    wxLogMessage("Nom du programme: %s\n", this->ProgramName);
    wxLogMessage("ID du processus en cours: %u\n", this->Id);
    wxLogMessage("Priorité: %u\n", this->Priority);
    wxLogMessage("Date de passage dans la base: %s\n", this->arrival_time);
    wxLogMessage("Nombre de fois en exécution: %u\n", this->Nbre_de_passage);
    wxLogMessage("Duree restante : %u ms\n", this->RemainingTime);
    wxLogMessage("\n");
    wxLogMessage("|----------------------------Liste des processus en attente------------------------------|\n");
    wxLogMessage("\n");
    afficherListeProcessus(liste,this);
    wxLogMessage("\n");
    wxLogMessage("|-----------------------------------------------------------------------------------------|\n");
}

//Méthode de type Processus qui affiche les informations d'un processus en attente
void Processus::ProcessusEnAttente()
{
    wxLogMessage("-----------------------------Priorité du processus: %u  ---------------------------\n", this->Priority);
    wxLogMessage("Nom du programme: %s\n", this->ProgramName);
    wxLogMessage("ID du processus: %u:\n", this->Id);
    wxLogMessage("Date de passage dans la base: %s\n", this->arrival_time);
    wxLogMessage("Duree restante : %u ms\n", this->RemainingTime);
    wxLogMessage("Temps d'attente: %u ms\n", this->waiting_time);
    wxLogMessage("Nombre de fois en exécution: %u\n", this->Nbre_de_passage);
}

//Méthode de type Processus qui met à jour le nombre de passage d'un processus dans la base
void Processus::update_pass()
{
    Nbre_de_passage++;
}

//Méthode de type Processus qui met à jour le temps d'attente d'un processus 
void Processus::add_waiting_time(Processus* liste,Processus *q,int time)
{
    Processus *p;
    p = liste;
    while (p != NULL)
    {
        if (p->Id != q->Id)
        {
            p->waiting_time = p->waiting_time + time;
            p = p->suivant;
        }
        else
        {
            p = p->suivant;
        }
        
    }
}

void Processus::setArrivalTime()
{
    wxDateTime wdt;
    wdt.SetToCurrent();
    this->arrival_time = wdt.Format(wxT("%Y-%m-%d %H:%M:%S"), wxDateTime::Local);
}
