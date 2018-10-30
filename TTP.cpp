#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

class City{
private:
	int cityNo;
	int locX;
	int locY;
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
};

class Item{
private:
	int itemNo;
	int itemProfit;
	int itemWeight;
	int itemLocation;
public:
	Item(){

	};
	Item(int N, int P, int W, int L){
		itemNo = N;
		itemProfit = P;
		itemWeight = W;
		itemLocation = L;	
	};

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
	City test1,test2,test3,test4;
	test1 = City(1,0,0);
	test2 = City(2,0,10);
	test3 = City(3,10,0);
	test4 = City(4,10,10);
	vector<City> CityHolder;
	vector<Item> Knapsack;
	CityHolder.push_back(test1);
	CityHolder.push_back(test2);
	CityHolder.push_back(test3);
	CityHolder.push_back(test4);
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