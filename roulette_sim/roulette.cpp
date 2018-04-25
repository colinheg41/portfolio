/* Author: Colin Hegarty
 * Date: 2/15/18
 * This program is used to simulate the use of a particular strategy in a game of roulette
 * over the course of many games.
 */

#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

int n = 0; // how many games are played
int sum = 0; // running sum of money lost/gained
int sumOfSquares = 0; // running sum of squares of money lost/gained
int initialBalance; // balance at the start of each game
int balance; // current balance
int goal; // amount of money such that when the balance reaches or exceeds this value, the game ends
int lowBet; // default bet size/minimum bet

void playGame () {
    int bet = lowBet;
    balance = initialBalance;
    int roll;

    while (balance < goal && balance > 0) { // plays until goal is met or all money lost
        roll = rand() % 19;
        if (roll < 9) { // if the round is won
            balance += bet; // gain money
            bet = lowBet; // reset bet
        } else { // if the round is lost
            balance -= bet; // lose money
            bet *= 2; // double bet size
        }
        if (bet > balance) // ensures that player cannot bet more than their current balance
            bet = balance;
    }
    // game finishes
    sum += balance; // adds balance, whether it's 0 or above the goal, to running sum
    sumOfSquares += pow(balance, 2); // updates sumOfSquares
}

void analyze () {
    int totalInitialBalance = n*initialBalance;
    float mean = (float)sum/(float)n;
    float stdev = pow((sumOfSquares - pow(sum, 2)/n)/n, 0.5);
    float meanLoss = initialBalance - mean;
    cout << "You played a total of this many games:    " << n << endl;
    cout << "You started with a total of:              $" << totalInitialBalance << endl;
    cout << "You ended with a total of:                $" << sum << endl;
    cout << "You ended each game with an average of:   $" << mean << endl;
    cout << "On average, you lost this much each game: $" << meanLoss << endl;
    cout << "The standard deviation of your games:     $" << stdev << endl;
}

int main()
{

    int i; // for loop counter

    srand(time(NULL)); // generates random seed

    cout << "This program simulates games of roulette using a strategy of placing small bets" << endl;
    cout << "on red, doubling the bet size after each loss, and resetting the bet after each win." << endl;
    cout << "How many games do you want to simulate?" << endl;
    cin >> n;
    cout << "How much money do you want to start with?" << endl;
    cin >> initialBalance;
    cout << "What is your goal balance (i.e. how much money do you want to leave the table with)?" << endl;
    cin >> goal;
    cout << "How large do you want the default bet to be?" << endl;
    cin >> lowBet;

    for (i = 0; i < n; ++i) {
        playGame();
    }

    analyze();

    return 0;
}
