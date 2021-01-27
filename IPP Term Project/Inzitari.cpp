#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <utility> // std::pair
#include <sstream>
#include <semaphore.h>
#include "timer.h"

#include <stdexcept> // std::runtime_error




class ChessGame {
public:
	std::string id;
	bool rated;
	double elapsed;
	int turns;
	std::string victory_status;
	int winner; // -1 = black, 0 = draw, 1 = white
	int white_rating;
	int black_rating;

};

/*
std::vector<Pokemon> myPokes;
std::vector<Pokemon> myTempPokes;
*/

std::vector<ChessGame> myGames;
std::vector<ChessGame> myTempGames;

long thread_count;
long long n;
double sum;
int notZeroTime_c;
double longestTime;
pthread_mutex_t mutex_sum;
pthread_mutex_t mutex_max;

void *Thread_RateSum(void* rank);
void *notZeroTime(void* rank);	


void Usage (char* prog_name)
{
		std::cout << "\nUsage: \n\t" << prog_name << "<Thread Count> <lower turn Limit>\n\n";
		std::cout <<"Turn limit is least amount of turns allowed\n";
		exit(0);
}


// Reads the Pokemon CSV for the desired length and stores it into the structure that can be used
// each item in the Vector is an individual pokemon
std::vector<ChessGame> read_csv(std::string filename, std::vector<ChessGame>& games){

	//in file
	std::ifstream myFile(filename);

	// error check open file
	if(!myFile.is_open()) throw std::runtime_error("Can not open file");


	std::string line;
	std::string tmp = "";
	double start, end;
	// parse csv
	if(myFile.good())
	{
		// dump the first line (column names are in struct)
		std::getline(myFile, line);


		// extract data
		while(std::getline(myFile, line))
		{
			ChessGame myGame;
			std::stringstream ss(line);

			//id
			std::getline(ss, myGame.id, ',');

			// rated
			std::getline(ss, tmp, ',');
			if (tmp == "FALSE")
				myGame.rated = false;
			else
				myGame.rated = true;

			//CreatedBy and End time
			std::getline(ss, tmp, ',');
			start = std::stod(tmp, nullptr);

			std::getline(ss, tmp, ',');
			end = std::stod(tmp, nullptr);

			myGame.elapsed = end - start;

			// turns
			std::getline(ss, tmp, ',');
			myGame.turns = std::stoi(tmp, nullptr, 10);

			//victory status
			std::getline(ss, myGame.victory_status, ',');


			// winner
			std::getline(ss,tmp, ',');
			if (tmp == "white")
				myGame.winner =1;
			else if (tmp == "draw")
				myGame.winner = 0;
			else if (tmp == "black")
				myGame.winner = -1;
			else 
				myGame.winner = 2; // error

			// increment code ignored
			std::getline(ss, tmp, ',');

			// white id ignored
			std::getline(ss, tmp, ',');

			// white rating
			std::getline(ss, tmp, ',');
			myGame.white_rating = std::stoi(tmp, nullptr, 10);

			// black id ignored
			std::getline(ss, tmp, ',');

			// black rating
			std::getline(ss, tmp, ',');
			myGame.black_rating = std::stoi(tmp, nullptr, 10);

			// ignore game sequence
			std::getline(ss, tmp, ',');
			// ignore opening eco
			std::getline(ss, tmp, ',');
			// ignore opening name
			std::getline(ss, tmp, ',');
			// ignore opening ply
			std::getline(ss, tmp, ',');

			games.push_back(myGame);

		}



	}

	myFile.close();

	return games;
}



int main(int argc, char* argv[]) 
{
	longestTime = 0.0;
	double start = 0.0, finish = 0.0, elapsed = 0.0;
	myGames.clear();
	myTempGames.clear();
	int lowerLimit;
	double average;

	long thread;
	pthread_t* thread_handles;

	// Args
			// get Thread counts
	if (argc != 3) 
		Usage(argv[0]);

   	thread_count = std::stol(argv[1], NULL, 10);
   	lowerLimit = std::stoi((argv[2]));
   	
   //	std::cout << myGen << "\n\n";


   	thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));
   	pthread_mutex_init(&mutex_sum, NULL);
   	pthread_mutex_init(&mutex_max, NULL);
   	sum = 0.0;

	// LOAD DATA
   	//set from https://www.kaggle.com/datasnaek/chess
	 myTempGames = read_csv("games.csv", myTempGames);
	//std::ofstream myFile("maxTimes.csv", std::ofstream::app);	
	// get desired subset
	//for (lowerLimit = 5; lowerLimit <= 100; lowerLimit *= 2){

	for (int i = 0;i < myTempGames.size(); i++)
	{
		if(myTempGames[i].turns >= lowerLimit){

				myGames.push_back(myTempGames[i]);
			}
	}

	n = myGames.size();

	std::cout << "Done Loading CSV\n";
	std::cout << "Games of " << lowerLimit<< " or more turns: " << myGames.size() << "\n";
	//std::cout << myPokes[3].name;
	//std::ofstream myFile("perf.csv", std::ofstream::app);
	//for(thread_count = 1; thread_count <= 16; thread_count*=2)
	//{
	/*
	GET_TIME(start);
	pokemon_sort(myGen, myPokes);
	GET_TIME(finish);



	std::cout << "\nList is Sorted\n";


	std::cout << "\n";
	for (int i = 0; i < myPokes.size(); i++)
		std::cout << myPokes[i].name << "\n";
	std::cout << "\n";
	 */

	//std::cout << std::fixed << "\nFor Array Size: " << myPokes.size() << ", thread_count: " << thread_count << " elapsed time is " << std::scientific << finish - start << std::fixed << " seconds\n";

	
	//myFile << std::scientific << finish-start <<",";
	
	//}
	//myFile <<"\n";
	//myFile.close();



	

	GET_TIME(start);
	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, Thread_RateSum, (void*)thread);

	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);
	GET_TIME(finish);

	elapsed = finish - start;
	average = sum / (double)n;

	//myFile << std::scientific << finish-start <<",";
	std::cout << "With Game Count of " << n << " and Thread Count of " << thread_count << " and atleast " << lowerLimit << " turns played. The average white player raiting was: " << average <<" and it took " << std::scientific << elapsed << std::fixed << " seconds to complete. \n\n";
	
	//sum = 0.0;
	

	GET_TIME(start);
	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, notZeroTime, (void*)thread);

	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);
	GET_TIME(finish);
	elapsed = finish - start;

	std::cout << "With Array Size: " << n << " and thread count " << thread_count << " and atleast " << lowerLimit << " turns. NonZero time count is " << notZeroTime_c << " and max among those is " << std::scientific<< longestTime <<" milliseconds. \n This Process took " << std::scientific << elapsed << " seconds.\n\n\n" << std::fixed;
	//myFile << std::scientific << finish-start <<",";
	//myFile << std::fixed << notZeroTime_c <<",";
	//myFile << std::scientific << longestTime <<',';
//	myFile << std::fixed << n <<",";
	//longestTime = 0.0;
	//notZeroTime_c = 0;
	//myGames.clear();
//}
	//myFile <<"\n";
	//myFile.close();
	pthread_mutex_destroy(&mutex_sum);
	pthread_mutex_destroy(&mutex_max);
	free(thread_handles);
	return 0;

}

void *notZeroTime(void* rank){
	long my_rank = (long) rank;
	long long my_n = n/thread_count;
	long long i;
	long long my_first_i = my_n * my_rank;
	long long my_last_i = my_first_i + my_n;

	int my_count = 0;
	int my_max = 0.0;

	for (i = my_first_i; i < my_last_i; i++)
	{
		if (myGames[i].elapsed != 0.0)
		{
			my_count += 1;
			if (myGames[i].elapsed > my_max)
				my_max = myGames[i].elapsed;
		}
	}

	pthread_mutex_lock(&mutex_max);
	notZeroTime_c += my_count;
	if (my_max > longestTime)
		longestTime = my_max;

	pthread_mutex_unlock(&mutex_max);

	return NULL;
}


void *Thread_RateSum(void* rank) {
	long my_rank = (long) rank;
	long long my_n = n/thread_count;
	long long i;
	long long my_first_i = my_n * my_rank;
	long long my_last_i = my_first_i + my_n;
	double my_sum = 0.0;

	// Generate local sum
	for (i = my_first_i; i < my_last_i; i++)
	{
		my_sum += myGames[i].white_rating;
	}

	pthread_mutex_lock(&mutex_sum);
	sum += my_sum;
	pthread_mutex_unlock(&mutex_sum);

	return NULL;

}