#include "simlib.h"
#include <stdio.h>
#include <map>


//Deklarace globalnich objektu
//mozno tu maju byt normalove rozdelenia jednotlivych mesiacov
Facility Vykon("Vykon systemu");
Stat HoursOfSun("Hodin slunecniho svitu");
Stat EnergieProdukovano("Produkovano energie");
Stat Rozdil("Rozdil meze spotrebou a vytobou");
double vykon;
double rozdil;
double spotreba;
double energie;
double hours_of_sun; 
double mean_value[12] = {50.174, 77.342, 129.24, 182.26, 227.02, 233.19, 245.52, 232.85, 165.71, 116.34, 52.926, 41.614};
double std_deviation[12] = {18.858, 27.095, 32.51, 39.259, 46.496, 38.044, 43.766, 41.63, 40.919, 33.602, 17.806, 15.104};
int i = 0;

 // spotreba
 class Consumption : public Process {
	void Behavior(){		//popis chovania panela
		double spotreba;
		spotreba = 12000;
		Rozdil(spotreba - energie);
	}
 };

class Transakce : public Process {
	void Behavior() {
		Seize(Vykon);
		energie = hours_of_sun * vykon;
	 	Release(Vykon);
	 	EnergieProdukovano(energie);
	 	(new Consumption)-> Activate();
		}
};

class Generator : public Event {
	void Behavior() {
	 	hours_of_sun = Normal(mean_value[i], std_deviation[i]);
	 	HoursOfSun(hours_of_sun);
		(new Transakce)->Activate();
 		i++;
	 	if(i == 12) i = 0;
 		Activate(Time + 1);
	}
};

class Panel : public Process {	// trieda panel, neviem ci to ma byt process
	/*
	TODO generovat vykon panelu jedneho na streche
	 */

	void Behavior(){		//popis chovania panela

	}


};


// main modelu
int main (){
	vykon = 305;
	Print("******MODEL D budovy******\n");
	Init(0,300);						// inicializacia experimentu		// panel, aktivovani
	(new Generator)->Activate();
	Run();
	Vykon.Output();
	HoursOfSun.Output();
	EnergieProdukovano.Output();
	Rozdil.Output();
}