// MPI_lernendeSysteme.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "Pattern.h"
#include "Generator.h"

#include <fstream> 
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>
#include <algorithm>
#include "MPI_lernendeSysteme.h"

using namespace std;

#define READTAG     1
#define DIETAG     2

CPattern LangeWiehagen;

std::vector<std::string> alphabet = { "a", "b" ,"c","d","e","f","g"};
std::vector<std::string> pattern = { "c1","c2", "c3", "b","e","e","c3" };

CGenerator Generator(pattern, alphabet);

vector<string> fileToVector(string _filename);
vector<string> SplitStringMultipleStrParameters(string s, const vector<string>& separators);
void ParseStringByStringSeparator(string s, const string separator, vector<string>& result);
string LangeWiehagenLerner(string _wort);
bool comparePatterns(vector<string> _pattern, string &foundPattern);

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);
	
	LangeWiehagen.setAlphabet(alphabet);

	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//
	// MASTER WORKER
	//	
	if (rank == 0)
	{

		bool end = false;

		while (!end)
		{
			cout << "1 Generator" << endl;
			cout << "2 Lernen" << endl << endl;
			cout << "0 EXIT" << endl;
			int a;
			cin >> a;

			switch (a)
			{
			case 0:
				return 0;
				break;
			case 1:
				Generator.GenerateFile("words");
				break;
			case 2:
				end = true;
				break;
			}
			cout << endl;
		}
		//Wordlist Shuffle


		vector<string> vstrings = fileToVector("words.csv");
		vector<vector<string>> listForMPI;

		for (int i = 0; i < size; i++)
		{
			//create shuffled vector
			vector<string> tempvec(vstrings);
			random_shuffle(tempvec.begin(), tempvec.end());

			listForMPI.push_back(tempvec);

			//safe shuffeld to file
			string Name = string("list") + to_string(i) + string(".txt");
			ofstream myfile(Name);

			int vsize = listForMPI.at(i).size();
			for (int n = 0; n<vsize; n++)
			{
				myfile << listForMPI.at(i)[n] << '\t';
				myfile << ",";
			}
		}



		/*
		Send messages after files were created and use mpi recv as a buffer
		*/
		int	ntasks;

		MPI_Comm_size(
			MPI_COMM_WORLD,   /* always use this */
			&ntasks);          /* #processes in application */
							   /*
							   * Seed the slaves.
							   */
		for (rank = 1; rank < ntasks; ++rank)
		{
			char temp;
			int a;
			MPI_Send(&a,         /* message buffer */
				1,              /* one data item */
				MPI_INT,        /* data item is an integer */
				rank,           /* destination process rank */
				0,        /* user chosen message tag */
				MPI_COMM_WORLD);/* always use this */
		}

		//receive pattern from slaves
		//compare them or wait until they worked through their dataset
		bool EndByTag = false, Matches = false;
		vector<string> patternvector;
		patternvector.resize(size);
		std::fill(patternvector.begin(), patternvector.end(), "0");

		string foundPattern;
		while (!EndByTag && !Matches) 
		{
			MPI_Status          status;
			int					rank,rankFromSlave;	

			MPI_Recv(&rankFromSlave,       /* message buffer */
				1,              /* one data item */
				MPI_INT,     /* of type double real */
				MPI_ANY_SOURCE, /* receive from any sender */
				MPI_ANY_TAG,    /* any type of message */
				MPI_COMM_WORLD, /* always use this */
				&status);       /* received message info */

			if (status.MPI_TAG == READTAG) 
			{
				string patternfilename = string("pattern") + to_string(rankFromSlave) + string(".txt");

				ifstream ifs(patternfilename);
				string newPattern((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

				ofstream file("log.log" + to_string(rankFromSlave));
				file << newPattern << endl;

				patternvector.at(rankFromSlave) = newPattern;

				Matches = comparePatterns(patternvector, foundPattern);
			}
			if (status.MPI_TAG == DIETAG)
			{
				cout << "Liste durchlaufen" << endl;
				EndByTag = true;
			}
		}
		/*
		Master();
		char helloStr[] = "Hello World";
		MPI_Send(helloStr, _countof(helloStr), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
		*/
	}
	//
	// SLAVE WORKER
	//
	else if(rank != 0)
	{
		MPI_Status	status;
		double a;
		//emfangen für start der berechnung
		MPI_Recv(&a,       /* message buffer */
			1,              /* one data item */
			MPI_DOUBLE,     /* of type double real */
			MPI_ANY_SOURCE, /* receive from any sender */
			MPI_ANY_TAG,    /* any type of message */
			MPI_COMM_WORLD, /* always use this */
			&status);       /* received message info */

		int myRank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
		//cout << myRank;

		string filenameWords = string("list") + to_string(myRank)+string(".txt");

		vector<string> slavefile = fileToVector(filenameWords);
		//cout << myRank << slavefile.at(slavefile.size()-1);

		string pattern;
		long long counter = 0;

		//durchlaufen des vectors und einzelne wörter an langewiehagen lerner übergeben 
		// alle 5 pattern wird verglichen
		for(auto& var : slavefile)
		{
		
			var.erase(remove(var.begin(), var.end(), '\t'), var.end());

			pattern = LangeWiehagenLerner(var);

			cout << to_string(myRank) + ": " << var << "-->"  << pattern << endl;
			counter++;
			
			if (counter % 5 == 0) 
			{
				string filenamePattern = string("pattern") + to_string(myRank)+ string(".txt");

				//speichern pattern in file
				ofstream file(filenamePattern);
				file << pattern << endl;

				MPI_Send(&myRank, 1, MPI_INT, 0, READTAG, MPI_COMM_WORLD);
			}
		}

		MPI_Send(&myRank, 1, MPI_INT, 0, DIETAG, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;

}

vector<string> fileToVector(string _filename) 
{
	/*
	*
	* Reads file into Vector
	*
	*/
	ifstream ifs(_filename);
	string str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

	return SplitStringMultipleStrParameters(str, { "," });
}


vector<string> SplitStringMultipleStrParameters(string s, const vector<string>& separators) 
{

	//The result to be returned
	vector<string> result = { s };

	//Iterate on the list of separators, so for each separators
	for (const auto& sep : separators) {
		//toReplaceBy will be the next vector of strings where it will iterate
		vector<string> toReplaceBy, tempRes;

		//For each strings, we will split on "sep", the separator
		for (auto&a : result) {
			//Because of the result vector being cleared every time your function get called
			//It get in a temp vector that we will concatenate after
			ParseStringByStringSeparator(a, sep, tempRes);
			//Concatenation of theses vectors
			toReplaceBy.insert(toReplaceBy.end(), tempRes.begin(), tempRes.end());
		}

		//Erasing all strings that are empty. C++11 code here required because of the lambda
		toReplaceBy.erase(std::remove_if(toReplaceBy.begin(), toReplaceBy.end(),
			[](const std::string& i) {
			return i == "";
		}), toReplaceBy.end());

		//The vector containing strings to be splited is replaced by the split result on this iteration
		result = toReplaceBy;
		//And we will split those results using the next separator, if there's more separator to iterate on
	}
	return result;
}


void ParseStringByStringSeparator(string s, const string separator, vector<string>& result)
{
	result.clear();
	size_t pos = 0;
	string token;

	while ((pos = s.find(separator)) != string::npos) {
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + separator.length());
	}
	result.push_back(s);
}

bool comparePatterns(vector<string> _pattern,string &foundPattern) 
{
	int counter = -1;
	int max		= 0;

	// vergleiche alle pattern und wenn mehr als x% dann return true
	for (auto val : _pattern) 
	{
		for (auto toCompare : _pattern)
		{
			if (val == toCompare)
				counter++;
		}
		if (counter > max)
		{
			max = counter;
			foundPattern = val;
		}

	}

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (counter > (size*0.5)) 
	{
		cout << "Genügend Überschneidung";
		return true;
	}
		

	return false;
}

string LangeWiehagenLerner(string _wort) 
{
	LangeWiehagen.checkPattern(_wort);
	return LangeWiehagen.getPattern();
}