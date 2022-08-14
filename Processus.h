#include "wx/wx.h"
#include <iostream>
#include <string.h>

using namespace std;
#pragma once

class Processus
{
	//Attributs
private:
	int Nbre_de_passage;
	int waiting_time;
public:
	int Id;
	int Duration;
	int RemainingTime;
	string ProgramName;
	int Priority;
	int Userid;
	string arrival_time; 
	Processus* suivant;

	//Constructeurs et destructeurs
	Processus();
	~Processus();


	//Méthodes
	void AfficherProcessus(Processus* liste);
	void ProcessusEnAttente();
	void update_pass();
	void add_waiting_time(Processus* Liste,Processus *q,int time);
	void setArrivalTime();

};

// Fonctions et procédures pour gérer une liste chaîné de processus
typedef Processus* ListeProcessus;

ListeProcessus initialiser();
ListeProcessus insererEnTete(ListeProcessus ancienneListe, Processus p);
ListeProcessus insererEnFin(ListeProcessus ancienneListe, Processus p);
void afficherListeProcessus(ListeProcessus liste, Processus *q);
ListeProcessus supprimer(ListeProcessus ancienneListe);
ListeProcessus supprimerMilieu(ListeProcessus ancienneListe,int processus_ID);
ListeProcessus vider(ListeProcessus ancienneListe);
ListeProcessus sortList(ListeProcessus ancienneListe);