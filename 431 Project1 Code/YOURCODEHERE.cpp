#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (912345679+911111111)

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = 2;
bool currentDimDone = false;
bool isDSEComplete = false;
// 
int l1block[4] = {8, 16, 32, 64};
int dl1sets[9] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
int dl1assoc[3] = {1, 2, 4};
int il1assoc[3] = {1, 2, 4};
int il1sets[9] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

int ul2sets[10] = {256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
int ul2block[4] = {16, 32, 64, 128};
int ul2assoc[5] = {1, 2, 4, 8, 16};
/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig) {

	string latencySettings;

	//
	//YOUR CODE BEGINS HERE
	//
	//implmenting block size
	int dl1_block=l1block[extractConfigPararm(halfBackedConfig, 2)];
	int il1_block=l1block[extractConfigPararm(halfBackedConfig, 2)];
	int ul2_block=ul2block[extractConfigPararm(halfBackedConfig, 8)];

	//implmenting set size
	int dl1_set=dl1sets[extractConfigPararm(halfBackedConfig, 3)];
	int il1_set=il1sets[extractConfigPararm(halfBackedConfig, 5)];
	int ul2_set=ul2sets[extractConfigPararm(halfBackedConfig, 7)];

	//implementing assoc
	int dl1_assoc=dl1assoc[extractConfigPararm(halfBackedConfig, 4)];
	int il1_assoc=il1assoc[extractConfigPararm(halfBackedConfig, 6)]; 
	int ul2_assoc=ul2assoc[extractConfigPararm(halfBackedConfig, 9)];

	//implementing cach size(((block*set*assoc)/1024) unit KB)
	int dl1_size=(dl1_block*dl1_set*dl1_assoc)/1024;
	int il1_size=(il1_block*il1_set*il1_assoc)/1024;
	int ul2_size=(ul2_block*ul2_set*ul2_assoc)/1024;

	int dl1_latency, il1_latency, ul2_latency;
	int dl1_latencySetting,il1_latencySetting, ul2_latencySetting;
	//implementing il1 size to il1 latency
		switch (il1_size){
			case 2:
				il1_latency=1;
				break;
			case 4:
				il1_latency=2;
				break;
			case 8:
				il1_latency=3;
				break;
			case 16:
				il1_latency=4;
				break;
			case 32:
				il1_latency=5;
				break;
			case 64:
				il1_latency=6;
				break;
			default:
				il1_latency=1;
				break;
		}//il1 switch end

	//implementing dl1 size to dl1 latency
		switch (dl1_size){ 
			case 2:
				dl1_latency=1;
				break;
			case 4:
				dl1_latency=2;
				break;
			case 8:
				dl1_latency=3;
				break;
			case 16:
				dl1_latency=4;
				break;
			case 32:
				dl1_latency=5;
				break;
			case 64:
				dl1_latency=6;
				break;
			default:
				dl1_latency=1;
				break;
		}//dl1 swith end

	//implementing ul2 size to ul2 latency
  		switch (ul2_size){
			case 32:
				ul2_latency=5;
				break;
			case 64:
				ul2_latency=6;
				break;
			case 128:
				ul2_latency=7;
				break;
			case 256:
				ul2_latency=8;
				break;
			case 512:
				ul2_latency=9;
				break;
			case 1024:
				ul2_latency=10;
				break;
			default:
				ul2_latency=5;
				break;
			}//ul2 switch end
	//implementing dl1_latency with dl1_assoc	
	  switch (dl1_assoc){
		case 2:
			dl1_latency+=1;
			break;
		case 4:
			dl1_latency+=2;
			break;
		case 8:
			dl1_latency+=3;
		default:
			dl1_latency=dl1_latency;
			break;
		}//dl1assoc switch end
	
	//implementing il1_latency with il1_assoc
		switch (il1_assoc){
			case 2:
				il1_latency+=1;
				break;
			case 4:
				il1_latency+=2;
				break;
			case 8:
				dl1_latency+=3;
			default:
				il1_latency=il1_latency;
				break;
			}//il1assoc switch end

	//implementing ul2_latency with ul2_assoc
		switch (ul2_assoc){
			case 2:
				ul2_latency+=1;
				break;
			case 4:
				ul2_latency+=2;
				break;
			case 8:
				ul2_latency+=3;
				break;
			case 16:
				ul2_latency+=4;
				break;
			default:
				ul2_latency=ul2_latency;
				break;
			}
	//implementing il1_latencySetting				
	if(il1_latency<=10 && il1_latency>=1){
		il1_latencySetting=il1_latency-1;
	}
	else{
		il1_latencySetting=0;
	}

	//implementing dl1_latencySetting
	if(dl1_latency<=10 && dl1_latency>=1){
		dl1_latencySetting=dl1_latency-1;
	}
	else{
		dl1_latencySetting=0;
	}
	
	//implementing ul2_latencySetting
	if(ul2_latency<=14 && ul2_latency>=5){
		ul2_latencySetting=ul2_latency-5;
	}
	else{
		ul2_latencySetting=0;
	}
	//converting int to string 
	string str_dl1_latencySetting,str_il1_latencySetting,str_ul2_latencySetting;
	stringstream s1,s2,s3;
	s1<<dl1_latencySetting;
	s1>>str_dl1_latencySetting;
	s2<<il1_latencySetting;
        s2>>str_il1_latencySetting;
	s3<<ul2_latencySetting;
        s3>>str_ul2_latencySetting;

	latencySettings=str_dl1_latencySetting + " " + str_il1_latencySetting + " " + str_ul2_latencySetting;

	//
	//YOUR CODE ENDS HERE
	//

	return latencySettings; // need str
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration) {

	// FIXME - YOUR CODE HERE
	int Array1[18];
	//implmenting block size
	int dl1_block=l1block[extractConfigPararm(configuration, 2)];
	int il1_block=l1block[extractConfigPararm(configuration, 2)];
	int ul2_block=ul2block[extractConfigPararm(configuration, 8)];

	//implmenting set size
	int dl1_set=dl1sets[extractConfigPararm(configuration, 3)];
	int il1_set=il1sets[extractConfigPararm(configuration, 5)];
	int ul2_set=ul2sets[extractConfigPararm(configuration, 7)];

	//implementing assoc
	int dl1_assoc=dl1assoc[extractConfigPararm(configuration, 4)];
	int il1_assoc=il1assoc[extractConfigPararm(configuration, 6)]; 
	int ul2_assoc=ul2assoc[extractConfigPararm(configuration, 9)];

	//implementing cach size(((block*set*assoc)/1024) unit KB)
	int dl1_size=(dl1_block*dl1_set*dl1_assoc)/1024;
	int il1_size=(il1_block*il1_set*il1_assoc)/1024;
	int ul2_size=(ul2_block*ul2_set*ul2_assoc)/1024;

	//configuration validation check
	for (int i=0; i<18;i++)//implemeting the fieldArray
	{
		Array1[i]>= extractConfigPararm(configuration,i);
	}

	
	if (isNumDimConfiguration(configuration)!=1){ // simple check of each dimension within bounds
		return 0;
	}

	//constraint 1: il1>=ifq, and il1 block size=dl1 block size (il1 block always = dl1 block size base on the given detfinition)
	if (extractConfigPararm(configuration,2)<extractConfigPararm(configuration,0) ){
		return 0;
	}

	//constraint 2: ul2_block need to be at least twice il1 and dl1 block size and < 128
	if (ul2_block<(2*il1_block)||ul2_block>128){
		return 0;
	}

	//constraint 3: il1 size and dl1 size: Minimum = 2 KB; Maximum = 64 KB
	if ((il1_size<2)||(il1_size>64)||(dl1_size<2)||(dl1_size>64)){
		return 0;
	}
	
	//constraint 4:ul2 size: Minimum = 32 KB; Maximum = 1 MB
	if ((ul2_size<32)||(ul2_size>1024)){
	  return 0;
	}


	// The below is a necessary, but insufficient condition for validating a
	// configuration.
	return 1;
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 */
std::string generateNextConfigurationProposal(std::string currentconfiguration,
		std::string bestEXECconfiguration, std::string bestEDPconfiguration,
		int optimizeforEXEC, int optimizeforEDP) {

	//
	// Some interesting variables in 431project.h include:
	//
	// 1. GLOB_dimensioncardinality
	// 2. GLOB_baseline
	// 3. NUM_DIMS
	// 4. NUM_DIMS_DEPENDENT
	// 5. GLOB_seen_configurations
         string bestConfig;
		if (optimizeforEXEC == 1){
			bestConfig = bestEXECconfiguration;}

	 	if (optimizeforEDP == 1){
	    	bestConfig = bestEDPconfiguration;}
		int nextValue = 0;
	std::string nextconfiguration = currentconfiguration;
	// Continue if proposed configuration is invalid or has been seen/checked before.
	while (!validateConfiguration(nextconfiguration) ||
		GLOB_seen_configurations[nextconfiguration]) {

		// Check if DSE has been completed before and return current
		// configuration.
		if(isDSEComplete) {
			return currentconfiguration;
		}

		std::stringstream ss;


		// Fill in the dimensions already-scanned with the already-selected best
		// value.
		for (int dim = 0; dim < currentlyExploringDim; ++dim) {
			ss << extractConfigPararm(bestConfig, dim) << " ";
		}

	
		
		switch(currentlyExploringDim){
			//Cache cases
			case 2: //ilblock (4)
			if (nextValue<4){
				nextValue++;
				cout << "ilblock next Value: "<<nextValue<<"\n";
				break;
			}
			
			case 3: // dl1sets (9)
			if (nextValue<9){
				nextValue++;
				cout << "dl1sets next Value: "<<nextValue<<"\n";
				break;
			}

			case 4: // dl1assoc (3)
			if (nextValue<9){
				nextValue++;
				cout << "dl1assoc next Value: "<<nextValue<<"\n";
				break;
			}

			case 5: // il1sets (9)
			if (nextValue<9){
				nextValue++;
				cout << "il1sets next Value: "<<nextValue<<"\n";
				break;
			}

			case 6: // il1assoc (3)
			if (nextValue<3){
				nextValue++;
				cout << "il1assoc next Value: "<<nextValue<<"\n";
				break;
			}

			case 7: // ul1sets (10)
			if (nextValue<10){
				nextValue++;
				cout << "ul1sets next Value: "<<nextValue<<"\n";
				break;
			}

			case 8: // ul2block (4)
			if (nextValue<4){
				nextValue++;
				cout << "ul2block next Value: "<<nextValue<<"\n";
				break;
			}

			case 9: // ulsassoc (5)
			if (nextValue<5){
				nextValue++;
				cout << "ulsassoc next Value: "<<nextValue<<"\n";
				break;
			}

			case 10: // replacement Policy (3)
			if (nextValue<3){
				nextValue++;
				cout << "replacement Policy next Value: "<<nextValue<<"\n";
				break;
			}

			//Core
			case 0: //width (4)
			if (nextValue<4){
				nextValue++;
				cout << "width next Value: "<<nextValue<<"\n";
				break;
			}

			case 1: //scheduling (2)
			if (nextValue<2){
				nextValue++;
				cout << "scheduling next Value: "<<nextValue<<"\n";
				break;
			}

			//FPU
			case 11: //fpwidth (4)
			if (nextValue<4){
				nextValue++;
				cout << "fpwidth next Value: "<<nextValue<<"\n";
				break;
			}
			
			//BP
			case 12: //brachSettings (5)
			if (nextValue<5){
				nextValue++;
				cout << "brachSettings next Value: "<<nextValue<<"\n";
				break;
			}

			case 13: //RAS (4)
			if (nextValue<4){
				nextValue++;
				cout << "RAS next Value: "<<nextValue<<"\n";
				break;
			}

			case 14: //btb (5)
			if (nextValue<5){
				nextValue++;
				cout << "btb next Value: "<<nextValue<<"\n";
				break;
			}

			default:
			nextValue=nextValue;
			cout << "next Value: "<<nextValue<<"\n";
			break;
		}// end of switch 1


		if (nextValue >= GLOB_dimensioncardinality[currentlyExploringDim]) {//checking is the Nextvalue out of bound
			nextValue = GLOB_dimensioncardinality[currentlyExploringDim] - 1;
			currentDimDone = true; 
		}

		ss << nextValue << " ";

		// Fill in remaining independent params with what is suppose to be.
		for (int dim = (currentlyExploringDim + 1);
				dim < (NUM_DIMS - NUM_DIMS_DEPENDENT); ++dim) {
			ss <<extractConfigPararm(bestConfig, dim) << " " ;
		}

		//
		// Last NUM_DIMS_DEPENDENT3 configuration parameters are not independent.
		// They depend on one or more parameters already set. Determine the
		// remaining parameters based on already decided independent ones.
		//
		string configSoFar = ss.str();

		// Populate this object using corresponding parameters from config.
		ss << generateCacheLatencyParams(configSoFar);

		// Configuration is ready now.
		nextconfiguration = ss.str();

		// Make sure we start exploring next dimension in next iteration.
		if (currentDimDone) {
			switch(currentlyExploringDim){
			//starting wich Cache
			case 2: // exploring l1block 
				cout << "exploring l1block\n";
				currentlyExploringDim =3;
				currentDimDone=false;
				break;
			
			case 3: // exploring d1sets 
				cout << "exploring d1sets\n";
				currentlyExploringDim =4;
				currentDimDone=false;
				break;

			case 4: // exploring dl1assoc 
				cout << "exploring d1assoc\n";
				currentlyExploringDim =5;
				currentDimDone=false;
				break;

			case 5: // exploring il1sets
				cout << "exploring il1sets\n";
				currentlyExploringDim =6;
				currentDimDone=false;
				break;
			
			case 6: // exploring il1assoc 
				cout << "exploring il1assoc\n";
				currentlyExploringDim =7;
				currentDimDone=false;
				break;

			case 7: // exploring ul2sets
				cout << "exploring ul2sets\n";
				currentlyExploringDim =8;
				currentDimDone=false;
				break;	

			case 8: // exploring ul2block
				cout << "exploring ul2block\n";
				currentlyExploringDim =9;
				currentDimDone=false;
				break;	

			case 9: // exploring ul2assoc 
				cout << "exploring ul2assoc\n";
				currentlyExploringDim =10;
				currentDimDone=false;
				break;
			
			case 10: // exploring replace policy
				cout << "exploring replace policy\n";
				currentlyExploringDim =0;
				currentDimDone=false;
				break;
		      
			// exploring core
			case 0: // exploring width
				cout << "exploring witdh\n";
				currentlyExploringDim =1;
				currentDimDone=false;
				break;
			
			case 1: // exploring scheduling 
				cout << "exploring scheduling\n";
				currentlyExploringDim =11;
				currentDimDone=false;
				break;
			
			//exploring FPU
			case 11: // exploring fpwidth
				cout << "exploring fpwidth\n";
				currentlyExploringDim =12;
				currentDimDone=false;
				break;
			//exploring BP
			case 12: // exploring branchsettings
				cout << "exploring branchsettings\n";
				currentlyExploringDim =13;
				currentDimDone=false;
				break;
			case 13: // exploring ras 
				cout << "exploring ras\n";
				currentlyExploringDim =14;
				currentDimDone=false;
				break;
			case 14: // exploring btb 
				cout << "exploring btb\n";
				currentDimDone=false;
				isDSEComplete=true;
				break;
			default:
				currentDimDone=false;
				break;
			}//end of switch2
		} //end of if (currentDimDone)


		// Signal that DSE is complete after this configuration.
		if (currentlyExploringDim == (NUM_DIMS - NUM_DIMS_DEPENDENT))
			isDSEComplete = true;

		if (!validateConfiguration(nextconfiguration)){
			cout<<"failed the valid test\n";
		}
	}
	return nextconfiguration;
}

