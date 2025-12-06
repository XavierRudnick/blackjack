#ifndef BANKROLL_H
#define BANKROLL_H

class Bankroll {
public:
    Bankroll(double initialBalance = 0);
    
    void deposit(double amount);
    void withdraw(double amount);
    
    double getBalance() const;
    double getTotalMoneyBet() const;
    
    void addTotalBet(double amount);

private:
    double balance;
    double totalMoneyBet;
};

#endif
