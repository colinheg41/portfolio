/* Author: Colin Hegarty
 * Date: 10/15/17
 * This program is used to assist in playing a game where four images are
 * given describing a word. The number letters in the word is given to the
 * player, as well as a pool of 12 letters from which the word can be made.
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

int main (int argc, char* argv[]) {
	// The command line input should be of the form "./run.exe <word length> <letter pool>
	// with no spaces in the letter pool.
	int length = atoi(std::string(argv[1]).c_str());
	std::string letters = argv[2];
	std::ifstream file("wordlist.txt");
	std::string word; // Placeholder variable for English words
	std::vector<std::string> results; // All words that meet the criteria
	// Iterate over the file of English words
	bool correct;
	while (!file.eof()) {
		getline(file, word);
		correct = true;
		// Check if length matches (excluding new line character)
		if (word.length() - 1 == length) {
			word = word.substr(0,length);
			std::string letters_copy = letters; // Create copy of pool to manipulate
			// For all letters in the English word, check if the letter is in the pool
			for (unsigned int i = 0; i < word.length(); ++i) {
				bool found = false;
				unsigned int j = 0;
				// Iterate over pool to check for letter
				for (; j < letters_copy.length(); ++j) {
					if (word[i] == letters_copy[j]) {
						found = true;
						break;
					}
				}
				// Remove letter from pool if found
				if (found)
					letters_copy.erase(j,1);
				// If letter is not in the pool, move onto the next word
				else {
					correct = false;
					break;
				}
			}
			// Add word to results list if the word meets the criteria
			if (correct) {
				results.push_back(word);
			}
		}
	}
	// Print results
	for (unsigned int i = 0; i < results.size(); ++i) {
		std::cout << results[i] << std::endl;
	}
}