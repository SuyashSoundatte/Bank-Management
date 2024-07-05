#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <exception>
#include <ctime>
#include <chrono>
#include <sstream>
#include <unordered_set>
#include <random>

class TransactionIDGenerator {
private:
    static std::unordered_set<std::string> generatedIDs;

public:
    static std::string generateTransactionID() {
        std::string id;
        do {
            id = generateUniqueID();
        } while (!isUnique(id));

        generatedIDs.insert(id);
        return id;
    }

private:
    static std::string generateUniqueID() {
        // Get current time in milliseconds since epoch
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        // Generate a random number for uniqueness
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);
        int randomNumber = dis(gen);

        // Construct transaction ID in the format: timestamp-randomnumber
        std::ostringstream oss;
        oss << millis << "-" << randomNumber;
        return oss.str();
    }

    static bool isUnique(const std::string& id) {
        return generatedIDs.find(id) == generatedIDs.end();
    }
};

// Initialize static member variable
std::unordered_set<std::string> TransactionIDGenerator::generatedIDs;

// Get Current Date and time
std::string getCurrentDate(){
    time_t tt; 

	// Declaring variable to store return value of 
	// localtime() 
	struct tm* ti; 

	// Applying time() 
	time(&tt); 

	// Using localtime() 
	ti = localtime(&tt); 
    
    std::string time = asctime(ti);
    return time; 
}

// Custom exceptions

class DepositAmountNegative: public std::exception{
    private:
        std::string message;

    public:
        DepositAmountNegative(const std::string& msg) : message(msg) {}

        virtual const char* what() const noexcept override {
            return message.c_str();
        }
};

class InsufficientFundsException : public std::exception {
private:
    std::string message;

public:
    InsufficientFundsException(const std::string& msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class WithdrawalLimitExceedException : public std::exception {
private:
    std::string message;

public:
    WithdrawalLimitExceedException(const std::string& msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

// Transaction class
class Transaction {
private:
    std::string transactionId;
    std::string date;
    std::string type;
    double amount;
    std::string description;

public:
    Transaction( const std::string& date, const std::string& type, double amt, const std::string& desc)
        : transactionId(TransactionIDGenerator::generateTransactionID()), date(date), type(type), amount(amt), description(desc) {}

    std::string getTransactionDetails() const {
        return "Transaction ID: " + transactionId + "\nDate: " + date + "\nType: " + type + "\nAmount: ₹" + std::to_string(amount) + "\nDescription: " + description;
    }
};

class BankUser {
private:
    std::string accountNumber;
    std::string accountHolderName;
    std::string atmPin;

public:
    BankUser(std::string accNum, std::string accHolderName, std::string pin)
        : accountNumber(accNum), accountHolderName(accHolderName), atmPin(pin) {}

    std::string getAccountNumber() const {
        return accountNumber;
    }

    std::string getAccountHolderName() const {
        return accountHolderName;
    }

    void getAccountHolderDetails(std::string inputPin) {
        if (inputPin == atmPin) {
            std::cout << "Account Holder Name: " << accountHolderName << "\nAccount Number: " << accountNumber << std::endl;
        } else {
            std::cout << "Invalid PIN." << std::endl;
        }
    }
};

// CurrentAccount base class
class CurrentAccount : public BankUser {
private:
    std::vector<Transaction> transactions;
    static int transactionCounter;
    double minBalance;

protected:
    double balance;

public:
    CurrentAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal)
        : BankUser(accNum, accHolderName, atmPin), balance(bal), minBalance(50000) {}

    virtual double checkBalance() const {
        return balance;
    }

    void deposit(double amount) {
        if (amount < 0) {
            throw DepositAmountNegative("Deposit amount should not be negative!");
        }
        balance += amount;
        recordTransaction(TransactionIDGenerator::generateTransactionID(),"Deposit", amount, "Deposited amount");
    }

    virtual void withdraw(double amount) {
        if (amount <= balance && balance<50000) {
            balance -= amount;
            recordTransaction(TransactionIDGenerator::generateTransactionID(),"Withdrawal", amount, "Withdrawn amount");
        } else {
            throw InsufficientFundsException("Insufficient funds for withdrawal.");
        }
    }

    std::string getAccountInfo() const {
        return "Account Number: " + getAccountNumber() + "\nAccount Holder: " + getAccountHolderName() + "\nBalance: ₹" + std::to_string(balance);
    }

    void recordTransaction(std::string transactionId, const std::string& type, double amount, const std::string& description) {
        transactions.emplace_back(transactionId ,type, amount, description);
    }

    void displayTransactionHistory() const {
        for (const auto& transaction : transactions) {
            std::cout << transaction.getTransactionDetails() << std::endl;
        }
    }

    virtual ~CurrentAccount() {}
};

int CurrentAccount::transactionCounter = 0;

// SavingsAccount derived class
// SavingsAccount derived class
class SavingsAccount : public BankUser {
private:
    std::vector<Transaction> transactions;
    double interestRate;
    int withdrawalCount;
    int maxWithdrawalsPerMonth;
    double withdrawalFee;
    double minimumBalance;
    double balance;

public:
    SavingsAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal, double interest, int maxWithdrawPerMonth, double fee)
        : BankUser(accNum, accHolderName, atmPin), balance(bal), interestRate(interest), withdrawalCount(0), maxWithdrawalsPerMonth(maxWithdrawPerMonth), withdrawalFee(fee), minimumBalance(1000) {}

    void addInterest() {
        double interestAmount = balance * (interestRate / 100);
        balance += interestAmount;
        recordTransaction(TransactionIDGenerator::generateTransactionID(), getCurrentDate(), "Interest", interestAmount, "Interest added");
    }

    double checkBalance() const {
        return balance;
    }

    void deposit(double amount) {
        if (amount < 0) {
            throw DepositAmountNegative("Deposit amount should not be negative!");
        }
        balance += amount;
        recordTransaction(TransactionIDGenerator::generateTransactionID(), getCurrentDate(), "Deposit", amount, "Deposited amount");
    }

    void withdraw(double amount) {
        if (withdrawalCount >= maxWithdrawalsPerMonth) {
            balance -= withdrawalFee;
            recordTransaction(TransactionIDGenerator::generateTransactionID(), getCurrentDate(), "Fee", withdrawalFee, "Withdrawal limit exceeded fee");
            std::cout << "Withdrawal limit exceeded for the month. Fee applied: ₹" << withdrawalFee << std::endl;
        }
        if (amount <= balance - minimumBalance) {
            balance -= amount;
            withdrawalCount++;
            recordTransaction(TransactionIDGenerator::generateTransactionID(), getCurrentDate(), "Withdrawal", amount, "Withdrawn amount");
        } else {
            throw InsufficientFundsException("Insufficient funds for withdrawal or minimum balance requirement not met.");
        }
    }

    void recordTransaction(const std::string& transactionId, const std::string& date, const std::string& type, double amount, const std::string& description) {
        transactions.emplace_back(transactionId, date, type, amount, description);
    }

    void displayTransactionHistory() const {
        for (const auto& transaction : transactions) {
            std::cout << transaction.getTransactionDetails() << std::endl;
        }
    }
};

class ZeroBalanceSavingsAccount : public BankUser {
    private:
        double balance;
        int withdrawalCount;
        int maxWithdrawalsPerDay;
        double withdrawalFee;
        double minimumBalance;
        std::vector<Transaction> transactions;
    public:
        ZeroBalanceSavingsAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal, double interest, int maxWithdrawPerMonth, double fee)
            : BankUser(accNum, accHolderName, atmPin), withdrawalCount(0), maxWithdrawalsPerDay(50000), withdrawalFee(100), minimumBalance(0) {
            // Override minimum balance requirement for zero balance account
        }

        // No additional methods needed as per description
        void deposit(double amount) {
            if (amount < 0) {
                throw DepositAmountNegative("Deposit amount should not be negative!");
            }
            balance += amount;
            recordTransaction(TransactionIDGenerator::generateTransactionID(),"Deposit", amount, "Deposited amount");
        }

        void recordTransaction(const std::string& transactionId, const std::string& type, double amount, const std::string& description) {
            transactions.emplace_back(transactionId,type, amount, description);
        }

        void withdraw(double amount) {
            if (withdrawalCount >= maxWithdrawalsPerDay) {
                balance -= withdrawalFee;
                recordTransaction(TransactionIDGenerator::generateTransactionID(),"Fee", withdrawalFee, "Withdrawal limit exceeded fee");
                std::cout << "Withdrawal limit exceeded for the month. Fee applied: ₹" << withdrawalFee << std::endl;
            }
            if (amount <= balance - minimumBalance) {
                balance -= amount;
                withdrawalCount++;
                recordTransaction(TransactionIDGenerator::generateTransactionID(),"Withdrawal", amount, "Withdrawn amount");
            } else {
                throw InsufficientFundsException("Insufficient funds for withdrawal or minimum balance requirement not met.");
            }
        }

        void displayTransactionHistory() const {
            for (const auto& transaction : transactions) {
                std::cout << transaction.getTransactionDetails() << std::endl;
            }
        }
};

// Special savings accounts
class WomenSavingsAccount : public SavingsAccount {
public:
    WomenSavingsAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal, double interest, int maxWithdrawPerMonth, double fee)
        : SavingsAccount(accNum, accHolderName, atmPin, bal, interest, maxWithdrawPerMonth, fee) {}

    void healthInsuranceBenefit() {
        std::cout << "Health insurance benefit applied.\n";
    }

    void loanInterestDiscount() {
        std::cout << "Loan interest discount applied.\n";
    }
};

class KidsSavingsAccount : public SavingsAccount {
public:
    KidsSavingsAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal, double interest, int maxWithdrawPerMonth, double fee)
        : SavingsAccount(accNum, accHolderName, atmPin, bal, interest, maxWithdrawPerMonth, fee) {}

    void educationalBonus() {
        std::cout << "Educational bonus applied.\n";
    }

    void parentalControl() {
        std::cout << "Parental control activated.\n";
    }
};

class SeniorCitizensSavingsAccount : public SavingsAccount {
public:
    SeniorCitizensSavingsAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal, double interest, int maxWithdrawPerMonth, double fee)
        : SavingsAccount(accNum, accHolderName, atmPin, bal, interest, maxWithdrawPerMonth, fee) {}

    void higherInterestRate() {
        std::cout << "Higher interest rate applied.\n";
    }

    void medicalBenefit() {
        std::cout << "Medical benefit applied.\n";
    }
};



// OverdraftAccount derived class
class OverdraftAccount : public CurrentAccount {
private:
    double overdraftLimit;
    double overdraftFee;

public:
    OverdraftAccount(double m_overdraftLimit, double m_overdraftFee, std::string accNum, std::string atmPin, std::string accHolderName, double bal)
        : CurrentAccount(accNum, atmPin , accHolderName, bal), overdraftLimit(m_overdraftLimit), overdraftFee(m_overdraftFee) {}

    void withdraw(double amount) override {
        if (amount <= balance) {
            balance -= amount;
            recordTransaction(TransactionIDGenerator::generateTransactionID(),"Withdrawal", amount, "Withdrawn amount");
        } else if (amount <= balance + overdraftLimit) {
            double overdraftAmount = amount - balance;
            balance -= amount;
            balance -= overdraftFee; // Apply overdraft fee
            recordTransaction(TransactionIDGenerator::generateTransactionID(), "Overdraft", overdraftAmount, "Overdraft used");
            recordTransaction(TransactionIDGenerator::generateTransactionID(), "Overdraft Fee", overdraftFee, "Overdraft fee applied");
        } else {
            throw WithdrawalLimitExceedException("Withdrawal amount exceeds overdraft limit. Withdrawal denied.");
        }
    }
};

// Template class for managing accounts
class FixedDepositAccount : public BankUser {
private:
    std::vector<Transaction> transactions;
    double interestRate;
    time_t maturityDate;
    bool isMatured;
    double balance;


public:
    FixedDepositAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal, double interest, int durationInDays)
        : BankUser(accNum, accHolderName, atmPin), interestRate(interest), isMatured(false) {
        time_t now = time(0);
        maturityDate = now + durationInDays * 24 * 60 * 60;
    }

    void withdraw(double amount) {
        time_t now = time(0);
        if (now >= maturityDate) {
            isMatured = true;
        }
        if (isMatured) {
            double interestAmount = balance * (interestRate / 100);
            balance += interestAmount;
            recordTransaction(TransactionIDGenerator::generateTransactionID(),"Interest", interestAmount, "Interest added on maturity");
        }
        if (amount <= balance) {
            balance -= amount;
            recordTransaction(TransactionIDGenerator::generateTransactionID(),"Withdrawal", amount, "Withdrawn amount");
        } else {
            throw InsufficientFundsException("Insufficient funds for withdrawal.");
        }
    }

    void recordTransaction(const std::string transactionId, const std::string& type, double amount, const std::string& description) {
        transactions.emplace_back(transactionId, amount, description);
    }

    void displayTransactionHistory() const {
        for (const auto& transaction : transactions) {
            std::cout << transaction.getTransactionDetails() << std::endl;
        }
    }
};

// NRIAccount class inheriting BankUser
class NRIAccount : public BankUser {
private:
    double balance;

public:
    NRIAccount(std::string accNum, std::string accHolderName, std::string atmPin, double bal)
        : BankUser(accNum, accHolderName, atmPin), balance(bal) {}

    double checkBalance() const {
        return balance;
    }

    void deposit(double amount) {
        balance += amount;
        std::cout << "Deposit of $" << amount << " successful.\n";
    }

    void withdraw(double amount) {
        if (amount <= balance) {
            balance -= amount;
            std::cout << "Withdrawal of ₹" << amount << " successful.\n";
        } else {
            throw InsufficientFundsException("Insufficient funds for withdrawal.");
        }
    }
};

class NREAccount: public NRIAccount{
    public:
        double tax;
};

// AccountManager template class
template<typename T>
class AccountManager {
private:
    std::vector<T> accounts;

public:
    void addAccount(const T& account) {
        accounts.push_back(account);
    }

    bool removeAccount(const std::string& accNum) {
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if (it->getAccountNumber() == accNum) {
                accounts.erase(it);
                return true;
            }
        }
        return false;
    }

    void displayAllAccounts() const {
        for (const auto& account : accounts) {
            std::cout << account.getAccountInfo() << std::endl;
        }
    }

    T* findAccount(const std::string& accountNumber) {
        for (auto& account : accounts) {
            if (account.getAccountNumber() == accountNumber) {
                return &account;
            }
        }
        return nullptr;
    }
};

// Main function
int main() {
    try {

    } 
    catch (const InsufficientFundsException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (const WithdrawalLimitExceedException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
