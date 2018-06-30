#include "Generator.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>

using namespace std;

CGenerator::CGenerator(vector<string> _pattern, vector<string> _alphabet)
	: m_Pattern(_pattern)
	, m_Alphabet(_alphabet)
{
}

vector<vector<string> >  CGenerator::GenerateWords()
{
	mt19937 rng;
	rng.seed(random_device()());
	uniform_int_distribution<mt19937::result_type> AlphabetDist(0, m_Alphabet.size() - 1);
	uniform_int_distribution<mt19937::result_type> AmountDist(1, 5);

	// dist(rng)

	vector<string> usedReplacerStrings; //because diffrent Variables need to have diffrent raplacer strings they need to be saved to compare
	vector<vector<string> > wordList;

	int wordAmount = 10000;

	for (int wordIndex = 0; wordIndex < wordAmount; wordIndex++) //generate wordAmount of words
	{
		wordList.push_back(m_Pattern);
		usedReplacerStrings.clear();
		for (int patternIndex = 0; patternIndex < wordList[wordIndex].size(); patternIndex++)//go through the pattern and pick the single elements
		{
			string currentElement = wordList[wordIndex].at(patternIndex);

			if (m_Alphabet.end() == find(m_Alphabet.begin(), m_Alphabet.end(), currentElement)) //check if variable or constant (does m_Alphabet contains this part of the pattern)
			{
				string replacerString;
				int randIndex;

				do
				{
					replacerString.clear();
					//generate a string consisting of a random lenght(1-5) and letters from the alpahbet
					int replaceAmount = AmountDist(rng);
					for (int i = 0; i < replaceAmount; i++)
					{
						randIndex = AlphabetDist(rng);
						string temp(m_Alphabet[randIndex]);
						replacerString.append(temp + " ");
					}

				} while (usedReplacerStrings.end() != find(usedReplacerStrings.begin(), usedReplacerStrings.end(), replacerString)); //check if the generated string already exist

				usedReplacerStrings.push_back(replacerString);

				auto foundOccurence = find(wordList[wordIndex].begin(), wordList[wordIndex].end(), currentElement); //find first occurence of the variable

				while (foundOccurence != wordList[wordIndex].end())
				{
					int index = distance(wordList[wordIndex].begin(), foundOccurence);

					wordList[wordIndex].erase(foundOccurence); //remove variable (makes found Occurence invalid --> usage of begin()+index)

					istringstream iss(replacerString);
					vector<string> tokens{ istream_iterator<string>{iss},istream_iterator<string>{} }; //explode the string into a strin list using spaces as delimiter

					wordList[wordIndex].insert(wordList[wordIndex].begin() + index, tokens.begin(), tokens.end()); //insert random string

					foundOccurence = find(wordList[wordIndex].begin() + index, wordList[wordIndex].end(), currentElement); //find next occurence
				}
			}
		}
	}

	return wordList;
}

void CGenerator::GenerateFile(string _fileName)
{
	ofstream wordFile;
	wordFile.open(_fileName + ".csv");

	vector<vector<string> > wordList = GenerateWords();

	for (auto& word : wordList)
	{
		std::string result;
		for (auto const& s : word) { result += s; }

		wordFile << result << ",";
	}
}