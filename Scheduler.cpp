#include "Scheduler.h"

using std::this_thread::sleep_for;
using namespace std;

//Constructeur par defaut 
Scheduler::Scheduler() : liste(NULL), quantum(0)
{
}

//Constructeur qui prend en param�tre une liste de processus et le quantum de temps
Scheduler::Scheduler(ListeProcessus C_liste, int C_quantum) : liste(C_liste), quantum(C_quantum)
{
}

//Destructeur
Scheduler::~Scheduler()
{
}

/*Cette m�thode de type Scheduler d�marre le simulateur d'ordonnancement. Il parcours une
* liste de processus puis ex�cute chaque processus en fonction du quantum de temps. Cette 
* ex�cution est simul�e gr�ce � un chronom�tre.
*/
void Scheduler::startScheduler()
{
	Processus* p = nullptr;
	Processus* q = nullptr;

	liste->AfficherProcessus(liste);

	do
	{
		p = liste;
		while (p != NULL)
		{
			if (p->RemainingTime <= quantum)
			{
				if (p->RemainingTime != 0)
				{
					sleep_for(std::chrono::milliseconds(p->RemainingTime));
					liste->add_waiting_time(liste, p, p->RemainingTime);
					p->update_pass();
					p->RemainingTime = 0;
					p->AfficherProcessus(liste);
					q = p;
					p = p->suivant;
					liste = supprimerMilieu(liste, q->Id);
				}
				else
				{
					q = p;
					p = p->suivant;
					liste = supprimerMilieu(liste,q->Id);
				}
			}
			else
			{
				sleep_for(std::chrono::milliseconds(quantum));
				liste->add_waiting_time(liste, p, quantum);
				p->update_pass();
				p->RemainingTime = p->RemainingTime - quantum;
				p->AfficherProcessus(liste);
				p = p->suivant;
			}
		}
	} while (liste != NULL);
}
