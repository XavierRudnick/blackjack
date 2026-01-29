#include "Bankroll.h"

double Bankroll::initialBalance = 0.0;

Bankroll::Bankroll(double startBalance) {
    balance = startBalance;
    initialBalance = startBalance;
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

double Bankroll::getInitialBalance() {
    return initialBalance;
}
