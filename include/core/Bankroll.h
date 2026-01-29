#ifndef BANKROLL_H
#define BANKROLL_H

class Bankroll {
private:
    double balance;
    double totalMoneyBet;
    static double initialBalance;
public:

    Bankroll(double startBalance = 0);
    
    void deposit(double amount);
    void withdraw(double amount);
    
    double getBalance() const;
    double getTotalMoneyBet() const;
    static double getInitialBalance();
    
    void addTotalBet(double amount);
};

#endif
