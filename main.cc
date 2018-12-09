#include "simlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <unistd.h>
 #include <getopt.h>
#include <stdbool.h>

Facility Vykon("Vykon systemu");
Stat HoursOfSun("Hodin slunecniho svitu");
Stat EnergieProdukovano("Generovano energie");
Stat Rozdil("Rozdil mezi spotrebou a vyrobou");

double performance;
double difference;
double consumption;
double energy_produced;
double hours_of_sun;
double total_energy;
double energy_used;
bool good_investment;
int month_of_return = -1;

double mean_value[12] = {50.174, 77.342, 129.24, 182.26, 227.02, 233.19, 245.52, 232.85, 165.71, 116.34, 52.926, 41.614};
double std_deviation[12] = {18.858, 27.095, 32.51, 39.259, 46.496, 38.044, 43.766, 41.63, 40.919, 33.602, 17.806, 15.104};
double month_consumptions[12] = {14292.9, 11740.1, 14898.0, 11509.0, 12090.0, 14855.0, 2311.9, 3958.4, 9245.4, 13551.3, 13405.9, 8674.6};

double price_kWh = 1.90;	//default
double investment = 280000;	//default
double solar_panel_output = 305; //default 
int number_of_panels = 65; 	//default
int years = 20;
int i = 0;
int j = 0;

 // spotreba
 class Consumption : public Process {
	void Behavior(){
		consumption = Uniform(month_consumptions[j]*0.90, month_consumptions[j]*1.1);	//generate random consumption from range
		j++;
		if (j == 12){
			j = 0;
		}
		Rozdil(consumption - energy_produced);
		if ((energy_produced - consumption) > 0){ 	//generated more than needed, the rest is going to out for free
			energy_produced = consumption;
		}
		energy_used += energy_produced;
		if ((price_kWh * total_energy) - investment > 0 && !good_investment){	//month when investment is returned
			good_investment = true;
			month_of_return = Time;
		}
	}
 };

class Transakce : public Process {
	void Behavior() {
		Seize(Vykon);
		energy_produced = (hours_of_sun * performance) / 1000;	//convert to kWh
		total_energy += energy_produced;	//computing all energy, including non-used
	 	Release(Vykon);
	 	EnergieProdukovano(energy_produced);
	 	(new Consumption)-> Activate();
		}
};

class Generator : public Event {
	void Behavior() {
	 	hours_of_sun = Normal(mean_value[i], std_deviation[i]);	//generate hours of sun per month
	 	HoursOfSun(hours_of_sun);
		(new Transakce)->Activate();
 		i++;
	 	if(i == 12){	
	 		i = 0;
	 		performance *= 1.0 - Uniform(0.7, 1.0) / 100;	//performance of solar panel decreases every year by 0.7-1%
	 	} 
 		Activate(Time + 1);	//generate new process every month
	}
};

void parse_args(int argc, char *argv[]){
	static struct option long_options[] = {
        {"price", required_argument, nullptr, 'p'},
        {"investment", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"years", required_argument, nullptr, 'y'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };

    int long_index =0;
    int opt= 0;
    while ((opt = getopt_long_only(argc, argv,"", long_options, &long_index )) != -1){
	    switch (opt) {
	        case 'p':
	        	price_kWh = strtod(optarg, NULL);
	        	break;
	        case 'i':
	        	investment = strtod(optarg, NULL);
	        	break;
	        case 'o':
	        	solar_panel_output = strtod(optarg, NULL);
	        	break;
	        case 'y':
	        	years = atoi(optarg);
	        	break;
	        case 'h':
	        	printf("Usage: ./main [price] [investment] [output]\n"
	        		   "Options are:\n"
       				   "\t--help: display help message\n"
       				   "\t--price: set price for kWh\n"
       				   "\t--investment: set total money investment\n"
       				   "\t--output: set energy output per hour of solar panel(Wh)\n"
       				   "\t--years: set number of years of simulation\n");
	        	exit(0);
	        default: 
	        	printf("Usage: ./main [price] [investment] [output]\n"
	        		   "Options are:\n"
       				   "\t--help: display help message\n"
       				   "\t--price: set price for kWh\n"
       				   "\t--investment: set total money investment\n"
       				   "\t--output: set energy output per hour of solar panel(Wh)\n"
       				   "\t--years: set number of years of simulation\n");
	        	exit(-1);
	    }
	}
}

// main modelu
int main (int argc, char *argv[]){
	parse_args(argc, argv);
	performance = solar_panel_output * number_of_panels;	//performance of system consisting of several panels
		
	total_energy = 0;
	energy_used = 0;
	good_investment = false;
	Print("****** Generovani energie ze slunecnich panelu na strese budovy D, FIT VUT ******\n");
	
	Init(0, (years * 12) - 1);
	(new Generator)->Activate();
	RandomSeed(time(0));
	Run();

	HoursOfSun.Output();
	EnergieProdukovano.Output();
	Rozdil.Output();
	printf("\n***** Vysledky simulace za %d let *****\n\n", years);
	printf("Vyrobeno energie: %g kWh\n", total_energy);
	printf("Vyuzito energie: %g kWh (%g%%)\n", energy_used, (energy_used/total_energy) * 100);
	printf("Usetreno: %g kc\n", (price_kWh * total_energy) - investment);
	if (month_of_return >= 0){
		printf("Doba vraceni investice: %d mesicu\n", month_of_return);
	}else{
		printf("Doba vraceni investice: vice nez %d let.\n", years);
	}
}