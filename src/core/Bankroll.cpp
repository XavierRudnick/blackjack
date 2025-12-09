#include "Bankroll.h"

Bankroll::Bankroll(double initialBalance) {
    balance = initialBalance;
    totalMoneyBet = 0;
}

void Bankroll::deposit(double amount) {
    balance += amount;
}

void Bankroll::withdraw(double amount) {
    balance -= amount;
}

double Bankroll::getBalance() const {
    return balance;
}

double Bankroll::getTotalMoneyBet() const {
    return totalMoneyBet;
}

void Bankroll::addTotalBet(double amount) {
    totalMoneyBet += amount;
}
