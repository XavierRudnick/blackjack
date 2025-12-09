#ifndef BANKROLL_H
#define BANKROLL_H

class Bankroll {
private:
    double balance;
    double totalMoneyBet;
    
public:
    Bankroll(double initialBalance = 0);
    
    void deposit(double amount);
    void withdraw(double amount);
    
    double getBalance() const;
    double getTotalMoneyBet() const;
    
    void addTotalBet(double amount);
};

#endif
