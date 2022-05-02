/*
* Copyright Megan Jordan 2022
* Code to test how the glitch in Ranked Choice Voting works.
* 
* Place a file called "rankedVoterData.csv" in the same directory as
* this cpp file in order to get the code to work.
* 
* This was built to work with Google Forms data.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <tuple>

using namespace std;

namespace vd {
    vector<pair<string, size_t>> candidates;
    vector<string> losers;
    signed int round = 1;
}

vector<string> split(const string& s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

bool countVotes(vector<vector<string>>& voterData) {
    cout << "ROUND " << vd::round++ << endl;

    for (auto& c : vd::candidates) {
        c.second = 0;
    }

    // Read the votes of a single person
    for (auto& v : voterData) {
        bool voted = false;

        // Run through each candidate they voted for
        for (int i = 0; i < v.size(); i++) {
            // If the candidate has already lost a round of votes, ignore that candidate.
            if (find(vd::losers.begin(), vd::losers.end(), v.at(0)) != vd::losers.end()) {
                v.erase(v.begin());
            }
        }

        // If their candidate was accounted for, increase their vote count.
        bool inCandidates = false;
        for (auto& t : vd::candidates) {
            if (t.first == v.at(0)) {
                inCandidates = true;
                t.second++;
            }
        }

        // If they voted for someone not yet accounted for, add them to the candidate list.
        if (!inCandidates) {
            vd::candidates.push_back(std::make_pair(v.at(0), 1));
        }
    }

    // Either find a winner or remove a loser.
    int minPos = 0;
    short winner = -1;
    double proportionOfVote;

    for (int i = 0; i < vd::candidates.size(); i++) {
        if (winner == -1) {
            proportionOfVote = static_cast<double>(vd::candidates.at(i).second) 
                / voterData.size();
        }
        cout << vd::candidates.at(i).first << ": " << proportionOfVote * 100 << "% ("
            << static_cast<double>(vd::candidates.at(i).second) << "/" << voterData.size() 
            << ")" << endl;
        if (proportionOfVote >= 0.5) { // Technically a tie is possible
            winner = i;
        }

        if (vd::candidates.at(i).second < vd::candidates.at(minPos).second) {
            minPos = i;
        }
    }

    if (winner != -1) {
        cout << "----------" << endl << "Winner is " << vd::candidates.at(winner).first 
            << " with " << proportionOfVote * 100 
            << "% (" << static_cast<double>(vd::candidates.at(winner).second) 
            << "/" << voterData.size() << ")!!!" << endl;
        return true; // Terminates recursion
    }

    cout << endl;

    // Adds person with lowest # of first place votes to "losers"
    vd::losers.push_back(vd::candidates.at(minPos).first);

    return countVotes(voterData); // Recurses until winner found
}

int main()
{
    ifstream voterDataFile;
    string line;
    vector<vector<string>> voterData;

    voterDataFile.open("rankedVoterData.csv");
    if (voterDataFile.fail()) {
        cerr << "File failed to open." << endl;
        return 1;
    }

    voterDataFile >> line;
    while (voterDataFile >> line) {
        voterData.push_back(split(line, ','));
    }

    countVotes(voterData);
    voterDataFile.close();
    return 0;
}