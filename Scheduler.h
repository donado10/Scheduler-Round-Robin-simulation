#include "wx/wx.h"
#include <iostream>
#include <string.h>
#include "Processus.h"
#include <chrono>
#include <thread>


#pragma once

using namespace std;

class Scheduler
{
	//Attributs
public:
	int quantum;
	ListeProcessus liste;

	//Constructeurs et Destructeurs
	Scheduler();
	Scheduler(ListeProcessus C_liste, int C_quantum);
	~Scheduler();

	//Méthodes
	void startScheduler();
};

