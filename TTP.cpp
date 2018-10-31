#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <cstring>
#include <sstream>
#include <algorithm>

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

};

class City{
private:
	int cityNo;
	int locX;
	int locY;
	vector<Item> cityItems;
public:
	City(){
	};
	City(int N,int X, int Y){
		cityNo = N;
		locX = X;
		locY = Y;
	};
	int calculateDistance(int X, int Y){
		int Distance;
		Distance = sqrt(pow(locX-X,2)+pow(locY-Y,2));
		return Distance;
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
	Tour();
	~Tour();
};

int closestCity(int X, int Y,vector<City> cities){
	int cityNumber;
	int closestDistance = cities[0].calculateDistance(X,Y);
	for(int i = 0; i < cities.size(); i++){
		int calculatedDistance = cities[i].calculateDistance(X,Y);
		if(closestDistance > calculatedDistance && calculatedDistance > 0){
			closestDistance = calculatedDistance;
		} else if(closestDistance > calculatedDistance * -1 && calculatedDistance < 0){
			closestDistance = calculatedDistance;
		}
	}
	for(int i = 0; i < cities.size(); i++){
		int calculatedDistance = cities[i].calculateDistance(X,Y);
		if(closestDistance == calculatedDistance){
			cityNumber = i;
			break;
		}
	}
	return cityNumber;
}

int main(){
	FILE *input;
	char buff[512];

	int numCities,numItems,knapCapacity,gottenCities=0,gottenItems=0;
	double minSpeed,maxSpeed,rentingRatio;

	vector<City> CityHolder;
	vector<Item> Knapsack;

	// read the input file
	if(!(input = popen("cat simple4_n6.ttp", "r"))){
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
				cout << "number of cities is " << numCities << endl;
			}
			// get number of items
			if(current == "ITEMS:"){
				str >> numItems;
				cout << "number of items is " << numItems << endl;
			}
			// get knapsack capacity
			if(current == "KNAPSACK:"){
				str >> knapCapacity;
				cout << "knapsack capacity is " << knapCapacity << endl;
			}
			// get min and max speed
			if(current == "MIN"){
				str >> current;
				str >> minSpeed;
				cout << "min speed is " << minSpeed << endl;
			}
			if(current == "MAX"){
				str >> current;
				str >> maxSpeed;
				cout << "max speed is " << maxSpeed << endl;
			}
			// get renting ratio
			if(current == "RATIO:"){
				str >> rentingRatio;
				cout << "renting ratio is " << rentingRatio << endl;
			}
			// get the cities
			if(gettingCities && gottenCities<numCities){
				// cout << "current is " << current << endl;
				int index,x,y;
				index = stoi(current,nullptr,10);
				str >> x;
				str >> y;
				cout << "creating city " << index << " " << x << " " << y << endl;
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
				cout << "creating item " << index << " " << profit << " " << weight << " for city " << node << endl;
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

	// cout << endl << "Knapsack type is " << knapsackDataType << endl;

	int currentX = CityHolder[0].giveX();
	int currentY = CityHolder[0].giveY();

	vector<City> VisitableCities = CityHolder;
	vector<City> VisitedCities;

	VisitedCities.push_back(VisitableCities[0]);
	VisitableCities.erase(VisitableCities.begin() + 0);

	while(VisitableCities.size() > 0){
		int theclosestcity = closestCity(currentX,currentY,VisitableCities);
		currentX = VisitableCities[theclosestcity].giveX();
		currentY = VisitableCities[theclosestcity].giveY();
		VisitedCities.push_back(VisitableCities[theclosestcity]);
		VisitableCities.erase(VisitableCities.begin() + theclosestcity);
	}

	ofstream outputfile;
	outputfile.open ("solution.txt");

	outputfile << "[";
	for(int i = 0 ; i < VisitedCities.size(); i++){
		outputfile << VisitedCities[i].giveNo();
		if(i != VisitedCities.size()-1){
			outputfile << ",";
		}
	}

	outputfile << "]" << endl;
	outputfile << "[";
	for(int i = 0 ; i < Knapsack.size(); i++){
		if(i != Knapsack.size()-1){
			outputfile << ",";
		}
	}

	outputfile << "]";
	outputfile.close();

	return 0;
}
