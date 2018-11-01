#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <power>
#include <set>

using namespace std;

class Item{
private:
	int itemNo;
	int itemProfit;
	int itemWeight;
	bool stolen;
public:
	Item(){
		stolen = false;
	};
	Item(int N, int P, int W){
		itemNo = N;
		itemProfit = P;
		itemWeight = W;
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
	Tour(vector<City> cities){
		tourCities = cities;
	}

	double giveTourDistance(){
			double distance = 0;
			// get the total distance of the tour so far
			for(int i=1;i<tour.size();i++){
				distance += currentTourEdges[tour[i-1]][tour[i]];
			}
			// add from the last city to the first
			distance += currentTourEdges[tour[tour.size()-1]][tour[0]];
			return distance;
	}

	void reverseTour(int start, int end){
		vector<int> temp;

		// fill the temp vector with the reversed vector
		for(int i=end;i>start;i--){
			temp.push_back(tour[i]);
		}

		// replace the numbers in the tour
		for(int i=start;i<end;i++){
			tour[i] = temp[i-start];
		}
	}

	bool inVecPair(pair<int, int> comPair, vector<pair<int, int> > pairs){
		for(int i=0;i<pairs.size();i++){
			if(pairs[i].first == comPair.first && pairs[i].second == comPair.second) return true;
		}
		return false;
	}

	void startTour(int start){
		double initialDistance = giveTourDistance();
		vector<pair<int, int> > badCombos, visitedCombos;
		vector<int> optimisedTour = tour;
		// make some possibile city variables
		int prevPossibleNext, nextFrom;
		double edge = 0, localOptimal = 0, localEdge, localOptimal, optimalLength;

		int prevNext = start;
		// set starting city
		int from = tour[prevNext];
		int nextCity = INT_MAX;

		while(nextCity > 0){
			// we don't want to update our tour if we don't need to
			nextCity = INT_MIN;


			// make a pair of the cities we're visiting and get the distance between them
			pair<int, int> currentEdge = sortedPair(prevNext,from);
			double currentEdgeLength = currentTourEdges[currentEdge.first][currentEdge.second];

			// check if we have visited this combo already
			if(inVecPair(currentEdge,visitedCombos)) break;

			for(int i=tour[from];i!=start;i=tour[i]){
				if(nextCity != INT_MIN) break;

				// variables to check optimisation gain
				localEdge = currentEdgeLength - edgeDistances[from][i];

				// check if it's a valid edge
				if( ! (!inVecPair(sortedPair(from,i),badCombos) && !inVecPair(sortedPair(prevPossibleNext,i)) && edge + localEdge > 0 && tour[i] != 0 && i != tour[from]) ){
					prevPossibleNext = i;
					continue;
				}

				// moving to tour[i]
				cout << "moving to " << i << endl;
				nextCity = i;

			}

			if(nextCity != INT_MIN){
				badCombos.push_back(currentEdge);
				visitedCombos.push_back(sortedPair(from,nextCity));

				localOptimal = edge + (currentEdgeLength - currentTourEdges[from][start]);

				if(localOptimal > optimalLength){
					optimalLength = localOptimal;
					optimisedTour = tour;
					optimisedTour[start] = from;
				}

				edge += currentEdgeLength - currentTourEdges[from][to];

				reverseTour(from, prevPossibleNext);

				nextFrom = prevPossibleNext;

				tour[from] = nextCity;

				prevNext = nextCity;
				from = nextFrom;
			}
		}

		// we don't want to set the tour to the new tour if its worse
		if(initialDistance > giveTourDistance()) return;

		// check the new tour
		tour = optimisedTour;
		printTour();
	}

	void optimalTour(){
		double difference,old_dist=0,new_dist=0;

		// check 20 generations of tours and see if it made anything better
		for(int i=0;i<20;i++){
			generateRandomTour();
			for(int j=0;j<tourCities.size();j++){
				startTour(j);
			}
			new_dist = giveTourDistance();
			difference = old_dist - new_dist;
			// the tour distance is the same
			if(i>0 && difference == 0) break;
			old_dist = new_dist;
		}
	}

	void printTour(){
		// debug feature
		for(int i=0;i<tour.size();i++){
			cout << tour[i] << " ";
		}
		cout << endl;
	}

	vector<int> getCurrentTour(){
		return tour;
	}

	~Tour();
private:
	vector<City> tourCities;
	vector<int> tour;
	vector<vector<double> > currentTourEdges;

	void generateRandomTour(){
		// clear tour vector
		tour.clear();

		// randomly shuffle all the cities except the starting one
		random_shuffle(tourCities.begin()+1,tourCities.end());

		// calculate the distance between each city
		calculateEdgeDistances();

		// generate tour from cities
		for(int i=0;i<tourCities.size();i++){
				tour.push_back(tourCities[i].giveNo());
				//tour.push_back( (i+1 % tourCities.size()) );
		}
	}

	void calculateEdgeDistances(){
		// resize edges vector to proper size
		currentTourEdges.resize(tourCities.size());
		for(int i=0;i<currentTourEdges.size();i++){
			currentTourEdges[i].resize(tourCities.size());
		}

		// set the distance between cities
		double distance;
		for(int i=0;i<tourCities.size();i++){
			for(int j=0;j<tourCities.size();j++){
				// pythagoras: a^2 + b^2 = c^2
				distance = sqrt(pow(tourCities[i].giveX() - tourCities[j].giveX(), 2) + pow(tourCities[i].giveY() - tourCities[j].giveY(), 2));
				currentTourEdges[i][j] = distance;
				currentTourEdges[j][i] = distance;
			}
		}
	}

	pair<int, int> sortedPair(int a, int b){
		if(a > b) return make_pair(a, b);
		else return make_pair(b, a);
	}

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
				// cout << "number of cities is " << numCities << endl;
			}
			// get number of items
			if(current == "ITEMS:"){
				str >> numItems;
				// cout << "number of items is " << numItems << endl;
			}
			// get knapsack capacity
			if(current == "KNAPSACK:"){
				str >> knapCapacity;
				// cout << "knapsack capacity is " << knapCapacity << endl;
			}
			// get min and max speed
			if(current == "MIN"){
				str >> current;
				str >> minSpeed;
				// cout << "min speed is " << minSpeed << endl;
			}
			if(current == "MAX"){
				str >> current;
				str >> maxSpeed;
				// cout << "max speed is " << maxSpeed << endl;
			}
			// get renting ratio
			if(current == "RATIO:"){
				str >> rentingRatio;
				// cout << "renting ratio is " << rentingRatio << endl;
			}
			// get the cities
			if(gettingCities && gottenCities<numCities){
				// cout << "current is " << current << endl;
				int index,x,y;
				index = stoi(current,nullptr,10);
				// make index start at 0
				index--;
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
				// make index start at 0
				index--;
				str >> profit;
				str >> weight;
				str >> node;
				// node started at 0
				node--;
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
