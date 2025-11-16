#include <iostream>
#include <string>
#include <map>
using namespace std;

enum CASH
{
    HUNDRED,
    FIVE_HUNDRED
};
enum TransactionType
{
    CHECK_BALANCE,
    WITHDRAW_CASH
};

class Card
{
    string cardNumber;
    int pin;
    string accountNumber;

public:
    Card(string cardNumber, int pin, string accountNumber)
    {
        this->cardNumber = cardNumber;
        this->pin = pin;
        this->accountNumber = accountNumber;
    }

    string getCardNumber()
    {
        return cardNumber;
    }

    bool validatePin(int enteredPin)
    {
        return this->pin == enteredPin;
    }

    string getAccountNumber()
    {
        return accountNumber;
    }
};

class Account
{
public:
    string accountNumber;
    double balance;

    Account(string accountNumber, double initialBalance)
    {
        this->accountNumber = accountNumber;
        this->balance = initialBalance;
    }

    bool withdraw(double amount)
    {
        if (balance >= amount)
        {
            balance -= amount;
            return true;
        }
        return false;
    }

    void deposit(double amount)
    {
        balance += amount;
    }
};

class State
{
public:
    virtual string getStateName() = 0;
    virtual void advanceState() = 0;
};

class IdleState : public State
{
public:
    ATMContext *context;

    string getStateName()
    {
        return "Idle state";
    }

    void advanceState()
    {
        context->setNextState(new HasCardState());
    }
};

class HasCardState : public State
{
public:
    ATMContext *context;

    string getStateName()
    {
        return "Has card state";
    }
    void advanceState()
    {
        context->setNextState(new SelectOperationState());
    }
};

class SelectOperationState : public State
{
public:
    ATMContext *context;

    string getStateName()
    {
        return "Selection operation state";
    }
    void advanceState()
    {
        context->setNextState(new TransactionState());
    }
};

class TransactionState : public State
{
public:
    ATMContext *context;

    TransactionState()
    {
    }

    string getStateName()
    {
        return "Transaction state";
    }

    void advanceState()
    {
        context->setNextState(new IdleState());
    }
};

class ATMInventory
{
public:
    unordered_map<CASH, int> cashCount;
    vector<pair<CASH, int>> types;

    ATMInventory()
    {
        types.push_back({CASH::FIVE_HUNDRED, 500});
        types.push_back({CASH::HUNDRED, 100});
    }

    bool dispenseCash(int amount)
    {
        unordered_map<CASH, int> dispense;
        for (auto note : types)
        {
            int req = amount / note.second;
            if (cashCount[note.first] >= req)
            {
                amount -= note.second * req;
                cashCount[note.first] -= req;
                dispense[note.first] = req;
            }

            if (amount == 0)
            {
                return true;
            }
        }

        if (amount > 0)
        {
            return false;
            // again put the dispense value into the inventory;
        }

        return true;
    }

    void addCash(CASH cash, int count)
    {
        cashCount[cash] += count;
    }
};

class ATMContext
{
public:
    State *currentState;
    Card *currentCard;
    Account *currentAccount;
    ATMInventory *atmInventory;
    map<string, Account> accounts;
    TransactionType selectedOperation;

    void setNextState(State *state)
    {
        this->currentState = state;
    }

    void insertCard(Card *card)
    {
        if (this->currentState->getStateName() == "idle state")
        {
            this->currentCard = card;
            currentState->advanceState();
        }

        cout << "Incorrect state";
        return;
    }

    void insertPIN(int pin)
    {
        if (this->currentState->getStateName() == "has card state")
        {
            if (currentCard->validatePin(pin))
            {
                currentState->advanceState();
                currentAccount = accounts.get(currentCard->getAccountNumber());
            }
        }
    }

    void selectOperationState(TransactionType type)
    {
        if (this->currentState->getStateName() == "select operation state")
        {
            this->selectedOperation = type;
        }

        return;
    }

    void performTransaction(double amount)
    {
        if (this->currentState->getStateName() == "transaction state")
        {
            if (selectedOperation == TransactionType::WITHDRAW_CASH)
            {
                performWithdrawal(amount);
            }
            else if (selectedOperation == TransactionType::CHECK_BALANCE)
            {
                checkBalance();
            }

            currentState->advanceState();
        }
    }

    void performWithdrawal(int amount)
    {
        if (atmInventory->dispenseCash(amount))
        {
            currentAccount->withdraw(amount);
        }
    }

    void checkBalance()
    {
        cout << "Your current balance is: $" << currentAccount->balance;
    }

    void returnCard()
    {
        this->currentAccount = nullptr;
        this->currentCard = nullptr;
        this->currentState = new IdleState();
    }
};

int main()
{
}