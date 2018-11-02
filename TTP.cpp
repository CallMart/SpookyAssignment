#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <climits>

using namespace std;

class Item{
private:
	int itemNo;
	int itemProfit;
	int itemWeight;
	// int itemLocation;
	bool stolen;
public:
	Item(){
		stolen = false;
	};
	Item(int N, int P, int W){
		itemNo = N;
		itemProfit = P;
		itemWeight = W;
		// itemLocation = L;
		stolen = false;
	};
	int giveNo(){
		return itemNo;
	}
	int giveProfit(){
		return itemProfit;
	}
	int giveWeight(){
		return itemWeight;
	}
	bool isStolen(){
		return false;
	}
	void stealItem(){
		stolen = true;
	}
};

class City{
private:
	int cityNo;
	int locX;
	int locY;
public:
	vector<Item> cityItems;
	City(){
	};
	City(int N,int X, int Y){
		cityNo = N;
		locX = X;
		locY = Y;
	};
	double calculateDistance(int X, int Y){
		return sqrt(pow(locX-X,2)+pow(locY-Y,2));
	}
	int giveX(){
		return locX;
	}
	int giveY(){
		return locY;
	}
	int giveNo(){
		return cityNo;
	}
	void addItem(Item item){
		cityItems.push_back(item);
	}
};

class Tour{
public:
	Tour(){
	};

	double giveTourDistance(vector<City> * TouredCities){
		double distance = 0;
		int currentX = TouredCities->at(0).giveX();
		int currentY = TouredCities->at(0).giveY();

		for(int i=1;i<TouredCities->size();i++){
			distance += TouredCities->at(i).calculateDistance(currentX,currentY);
			currentX = TouredCities->at(i).giveX();
			currentY = TouredCities->at(i).giveY();
		}

		distance += TouredCities->at(0).calculateDistance(currentX,currentY);

		return distance;
	}

	int closestCity(int X, int Y,vector<City> * Cities){
		// find the closest city by searcing through the whole vector
		int cityNumber = 0;
		double closestDistance = Cities->at(0).calculateDistance(X,Y);

		for(int i = 1; i < Cities->size(); i++){

			double currentDistance = Cities->at(i).calculateDistance(X,Y);

			if(currentDistance < closestDistance){
				cityNumber = i;
				closestDistance = currentDistance;
			}

		}

		return cityNumber;
	}

	vector<City> nearestNeighbourTour(vector<City> Cities){
		vector<City> NNTour;

		int currentX = Cities[0].giveX();
		int currentY = Cities[0].giveY();

		// fill the NN tour with the first city and erase it from visitable cities
		NNTour.push_back(Cities[0]);
		Cities.erase(Cities.begin());

		// get the nearest neighbour at each iteration
		while(Cities.size() > 0){
			int theclosestcity = closestCity(currentX,currentY,&Cities);
			currentX = Cities[theclosestcity].giveX();
			currentY = Cities[theclosestcity].giveY();
			NNTour.push_back(Cities[theclosestcity]);
			Cities.erase(Cities.begin() + theclosestcity);
		}

		return NNTour;
	}

	vector<City> twoOptSwap(vector<City> * Cities, int i, int k){
	  vector<City> Swapped;
	  // following the pseudocode steps on wikipedia for two-opt: https://en.wikipedia.org/wiki/2-opt

	  // 1. take route[0] to route[i-1] and add them in order to new_route
	  for(int a=0;a<=i-1;a++){
	    Swapped.push_back(Cities->at(a));
	  }

	  // 2. take route[i] to route[k] and add them in reverse order to new_route
		int reverseOffset = 0;
	  for(int a=i;a<=k;a++){
	    Swapped.push_back(Cities->at(k-reverseOffset));
			reverseOffset++;
	  }

	  // 3. take route[k+1] to end and add them in order to new_route
	  for(int a=k+1;a<Cities->size();a++){
	    Swapped.push_back(Cities->at(a));
	  }

	  return Swapped;
	}

	vector<City> twoOptTour(vector<City> Cities){
	  vector<City> Optimised = Cities;

		// took ideas from this http://pedrohfsd.com/2017/08/09/2opt-part1.html, implementation combined with wikipedia


	  // we want to improve the tour, check the distance travelled in the tour without 2-opt
		double minDist = giveTourDistance(&Optimised);
	  bool improvement = true;

	  // check if theres an improvement
	  while(improvement){
	    // there is no improvement by default
	    improvement = false;

			// start at i=1 since you can't change the starting city
	    for(int i=1;i<Optimised.size()-1;i++){
	      for(int j=i+1;j<Optimised.size();j++){
	        // do a two opt swap and find the distance that is
	        vector<City> temp = twoOptSwap(&Optimised,i,j);
	        double currentDist = giveTourDistance(&temp);

	        // reset since we found an improvement
	        if(currentDist < minDist){
	          improvement = true;
	          Optimised = temp;
	          minDist = currentDist;
	        }
	      }
	    }
	  }

	  return Optimised;
	}

	vector<Item> generateRandomKnapsack(vector<City> * TouringCities, int maxWeight, double minSpeed, double maxSpeed, double rentingRatio){
		vector<Item> Stolen;

		double bestProfit = INT_MIN;
		int improvements = 0;
		int numTimesToImprove;

		// shorten it down for huge inputs
		if(TouringCities->size() < 2000) numTimesToImprove = 100;
		else numTimesToImprove = 25;

		// do at least some iterations to check for improvements to stealing
		while(improvements < numTimesToImprove){
			Stolen.clear();

			// cout << "check for improvement" << endl;

			double currentHours = 0,currentSpeed = maxSpeed;
			int currentWeight = 0, currentX = TouringCities->at(0).giveX(), currentY = TouringCities->at(0).giveY();
			// skip the first town
			for(int i=1;i<TouringCities->size();i++){
				// generate a random number of items to steal
				int numToSteal = rand() % TouringCities->at(i).cityItems.size() + 1;
				// check the very last case to see if that improved at all
				if(improvements == numTimesToImprove-1) numToSteal = TouringCities->at(i).cityItems.size();
				vector<int> stolenItems;

				double distanceTravelled = TouringCities->at(i).calculateDistance(currentX,currentY);
				currentSpeed = maxSpeed - double(currentWeight)*((maxSpeed-minSpeed)/double(maxWeight));
				currentX = TouringCities->at(i).giveX();
				currentY = TouringCities->at(i).giveY();

				// time taken to travel that far
				currentHours += distanceTravelled/currentSpeed;

				for(int j=0;j<numToSteal;j++){
					int toSteal = rand() % TouringCities->at(i).cityItems.size();
					// make sure we are getting a non stolen item
					while(find(stolenItems.begin(),stolenItems.end(),toSteal) != stolenItems.end()){
						toSteal = rand() % TouringCities->at(i).cityItems.size();
					}

					if(currentWeight + TouringCities->at(i).cityItems[toSteal].giveWeight() > maxWeight){
						// cout << "item " << TouringCities[i].cityItems[toSteal].giveNo() << " is too heavy to add to sack " << endl;
						continue;
					}else{
						// cout << "adding item "  << TouringCities->at(i).cityItems[toSteal].giveNo() << " to knapsack" << endl;
						stolenItems.push_back(toSteal);
						currentWeight += TouringCities->at(i).cityItems[toSteal].giveWeight();
						Stolen.push_back(TouringCities->at(i).cityItems[toSteal]);
					}
				}
			}

			// traversal back to start
			double distanceTravelled = TouringCities->at(0).calculateDistance(currentX,currentY);
			currentSpeed = maxSpeed - double(currentWeight)*((maxSpeed-minSpeed)/double(maxWeight));
			currentHours += distanceTravelled/currentSpeed;

			double currentProfit = 0;

			// add all the items together
			for(int i=0;i<Stolen.size();i++){
				currentProfit+=Stolen[i].giveProfit();
			}

			// check the profit made
			currentProfit -= (currentHours*rentingRatio);

			improvements++;

			// we made an improvement, reset and look for another one
			if(bestProfit < currentProfit){
				// cout << "previous profit: " << bestProfit << " new profit: " << currentProfit << " profit improved, resetting" << endl;
				improvements = 0;
				bestProfit = currentProfit;
			}
		}



		return Stolen;
	}
};

int main(int argc, char* argv[]){
	FILE *input;
	char buff[512];
	char inputName[512];

	// initialise random seed
	srand (time(NULL));

	int numCities,numItems,knapCapacity,gottenCities=0,gottenItems=0;
	double minSpeed,maxSpeed,rentingRatio;

	// variables for the problem
	vector<City> CityHolder;
	vector<Item> Knapsack;
	vector<City> CurrentTour;
	Tour TravelingThief;

	strcpy(inputName,"cat ");
	strcat(inputName,argv[1]);

	// read the input file
	if(!(input = popen(inputName, "r"))){
		return 1;
	}

	bool gettingCities=false,gettingItems=false;

	// read each single substring of the buffer
	while(fgets(buff, sizeof(buff), input)!=NULL){
		istringstream str(buff);
		string current;
		while(str >> current)
		{
			// get number of cities
			if(current == "DIMENSION:"){
				str >> numCities;
				//cout << "number of cities is " << numCities << endl;
			}
			// get number of items
			if(current == "ITEMS:"){
				str >> numItems;
				//cout << "number of items is " << numItems << endl;
			}
			// get knapsack capacity
			if(current == "KNAPSACK:"){
				str >> knapCapacity;
				//cout << "knapsack capacity is " << knapCapacity << endl;
			}
			// get min and max speed
			if(current == "MIN"){
				str >> current;
				str >> minSpeed;
				//cout << "min speed is " << minSpeed << endl;
			}
			if(current == "MAX"){
				str >> current;
				str >> maxSpeed;
				//cout << "max speed is " << maxSpeed << endl;
			}
			// get renting ratio
			if(current == "RATIO:"){
				str >> rentingRatio;
				//cout << "renting ratio is " << rentingRatio << endl;
			}
			// get the cities
			if(gettingCities && gottenCities<numCities){
				// cout << "current is " << current << endl;
				int index,x,y;
				index = stoi(current,nullptr,10);
				str >> x;
				str >> y;
				// cout << "creating city " << index << " " << x << " " << y << endl;
				CityHolder.push_back(City(index,x,y));
				gottenCities++;
			} else if(gettingCities && gottenCities>=numCities){
				gettingCities = false;
			}
			if(current == "Y):"){
				gettingCities = true;
			}
			// get the knapsack items
			if(gettingItems && gottenItems<numItems){
				int index,profit,weight,node;
				index = stoi(current,nullptr,10);
				str >> profit;
				str >> weight;
				str >> node;
				// cout << "creating item " << index << " profit:" << profit << " weight:" << weight << " for city " << node << endl;
				CityHolder[node-1].addItem(Item(index,profit,weight));
				gottenItems++;
			} else if(gettingItems && gottenItems>=numItems){
				gettingItems = false;
			}
			if(current == "NUMBER):"){
				gettingItems = true;
			}
		}
	}

	pclose(input);

	// generate a nearest neighbour tour
	CurrentTour = TravelingThief.nearestNeighbourTour(CityHolder);

	// run 2-opt on the nearest neighbour tour if the input isn't ridicolously large
	if(numCities<2000) CurrentTour = TravelingThief.twoOptTour(CurrentTour);

	ofstream outputfile;
	outputfile.open ("fnl_soln.ttp");

	outputfile << "[";
	for(int i = 0 ; i < CurrentTour.size(); i++){
		outputfile << CurrentTour[i].giveNo();
		if(i != CurrentTour.size()-1){
			outputfile << ",";
		}
	}
	outputfile << "]" << endl;

	// generate a random knapsack that tries to improve itself
	Knapsack = TravelingThief.generateRandomKnapsack(&CurrentTour,knapCapacity,minSpeed,maxSpeed,rentingRatio);

	outputfile << "[";
	for(int i = 0 ; i < Knapsack.size(); i++){
		outputfile << Knapsack[i].giveNo();
		if(i != Knapsack.size()-1){
			outputfile << ",";
		}
	}

	outputfile << "]";
	outputfile.close();

	return 0;
}
