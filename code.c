#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Transaction {
    char type[20];
    float amount;
    struct Transaction* next;
} Transaction;

typedef struct Account {
    char accountHolder[50];
    int accountNo;
    int pin;
    float balance;
    float cibilScore;
    Transaction* transactionStack;
    struct Account* next;
} Account;

typedef struct LoanApplication {
    char accountHolder[50];
    int accountNo;
    float cibilScore;
    float requestedAmount;
    int status; // 0 - Pending, 1 - Approved, 2 - Rejected
    struct LoanApplication* next;
} LoanApplication;

Account* customerAccounts = NULL;
LoanApplication* loanApplications = NULL;
// Function to print a formatted output box
void printBox(const char* message) {
	int i;
    int len = strlen(message);
    printf("\n+");
    for ( i = 0; i < len + 2; i++) {
        printf("-");
    }
    printf("+\n");
    printf("¦ %s ¦\n", message);
    printf("+");
    for ( i = 0; i < len + 2; i++) {
        printf("-");
    }
    printf("+\n");
}
// Function to create a new transaction node
Transaction* createTransaction(const char type[], float amount) {
    Transaction* newTransaction = (Transaction*)malloc(sizeof(Transaction));
    strcpy(newTransaction->type, type); // Copy the string constant to the character array
    newTransaction->amount = amount;
    newTransaction->next = NULL;
    return newTransaction;
}

// Function to push a transaction onto the stack for a specific account
void pushTransaction(Account* account, const char type[], float amount) {
    Transaction* newTransaction = createTransaction(type, amount);
    strcpy(newTransaction->type, type); // Copy the string constant to the character array
    newTransaction->next = account->transactionStack;
    account->transactionStack = newTransaction;
}

// Function to display transaction history for a specific account
void displayTransactionHistory(Account* account) {
    Transaction* current = account->transactionStack;

    if (current == NULL) {
        printf("Transaction history is empty for Account Number: %d\n", account->accountNo);
        return;
    }

    printf("\nTransaction History for Account Number: %d\n", account->accountNo);
    while (current != NULL) {
        printf("%s - Amount: %.2f\n", current->type, current->amount);
        current = current->next;
    }
}

// Function to create a new account
void createAccount(char accountHolder[], int pin,float cs) {
    Account* newAccount = (Account*)malloc(sizeof(Account));
    strcpy(newAccount->accountHolder, accountHolder);
    newAccount->accountNo = rand() % 9000 + 1000; // Generate a random 4-digit account number
    newAccount->pin = pin;
    newAccount->balance = 0.0;
    newAccount->cibilScore = cs; // Initialize CIBIL score to 0
    newAccount->transactionStack = NULL;
    newAccount->next = customerAccounts;
    customerAccounts = newAccount;

    printf("\nAccount created successfully\n");
    printf("Your account number is %d\n", newAccount->accountNo);
}

// Function to delete an account based on account number and PIN
void deleteAccount(int accountNo, int pin) {
    Account* current = customerAccounts;
    Account* previous = NULL;

    while (current != NULL) {
        if (current->accountNo == accountNo && current->pin == pin) {
            if (previous == NULL) {
                customerAccounts = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            printf("Account with Account Number %d has been deleted successfully.\n", accountNo);
            return;
        }

        previous = current;
        current = current->next;
    }

    printf("Account with Account Number %d and PIN %d not found.\n", accountNo, pin);
}

// Function to view customer account information based on account number and PIN
void checkAccountInfo(int accountNo, int pin) {
    Account* current = customerAccounts;

    while (current != NULL) {
        if (current->accountNo == accountNo && current->pin == pin) {
            printf("\nAccount Holder: %s\n", current->accountHolder);
            printf("Account Number: %d\n", current->accountNo);
            printf("Balance: %.2f\n", current->balance);
            printf("CIBIL Score: %.2f\n", current->cibilScore);
            return;
        }
        current = current->next;
    }

    printf("Account with Account Number %d and PIN %d not found.\n", accountNo, pin);
}

// Function to perform a withdrawal
void withdrawal(int accountNo, int pin, float amount) {
    Account* current = customerAccounts;

    while (current != NULL) {
        if (current->accountNo == accountNo && current->pin == pin) {
            if (current->balance >= amount) {
                current->balance -= amount;
                printf("\nWithdrawal of %.2f successful. New balance: %.2f\n", amount, current->balance);
                pushTransaction(current, "Withdrawal",amount);
            } else {
                printf("\nInsufficient balance. Current balance: %.2f\n", current->balance);
            }
            return;
        }
        current = current->next;
    }

    printf("Account with Account Number %d and PIN %d not found.\n", accountNo, pin);
}

// Function to perform a deposit
void deposit(int accountNo, int pin, float amount) {
    Account* current = customerAccounts;

    while (current != NULL) {
        if (current->accountNo == accountNo && current->pin == pin) {
            current->balance += amount;
            printf("\nDeposit of %.2f successful. New balance: %.2f\n", amount, current->balance);
            pushTransaction(current, "Deposit", amount);
            return;
        }
        current = current->next;
    }

    printf("Account with Account Number %d and PIN %d not found.\n", accountNo, pin);
}

// Function to perform a transfer between two accounts
void transfer(int senderAccountNo, int senderPin, int receiverAccountNo, float amount) {
    Account* sender = customerAccounts;
    Account* receiver = customerAccounts;

    while (sender != NULL) {
        if (sender->accountNo == senderAccountNo && sender->pin == senderPin) {
            break;
        }
        sender = sender->next;
    }

    while (receiver != NULL) {
        if (receiver->accountNo == receiverAccountNo) {
            break;
        }
        receiver = receiver->next;
    }

    if (sender == NULL) {
        printf("\nSender account with Account Number %d and PIN %d not found.\n", senderAccountNo, senderPin);
        return;
    }

    if (receiver == NULL) {
        printf("\nReceiver account with Account Number %d not found.\n", receiverAccountNo);
        return;
    }

    if (sender->balance >= amount) {
        sender->balance -= amount;
        receiver->balance += amount;
        printf("\nTransfer of %.2f from account %d to account %d successful.\n", amount, senderAccountNo, receiverAccountNo);
        printf("Sender's new balance: %.2f\n", sender->balance);
        char transactionInfo[50];
        sprintf(transactionInfo, "Transfer (To Account: %d)", receiverAccountNo);
        pushTransaction(sender, transactionInfo, -amount);
        sprintf(transactionInfo, "Transfer (From Account: %d)", senderAccountNo);
        pushTransaction(receiver, transactionInfo, amount);
    } else {
        printf("\nInsufficient balance in the sender's account. Current balance: %.2f\n", sender->balance);
    }
}

// Function to create a new loan application
void applyLoan(char accountHolder[], int accountNo, float requestedAmount) {
    LoanApplication* newApplication = (LoanApplication*)malloc(sizeof(LoanApplication));
    strcpy(newApplication->accountHolder, accountHolder);
    newApplication->accountNo = accountNo;
    newApplication->cibilScore = 0.0; // Initialize CIBIL score to 0
    newApplication->requestedAmount = requestedAmount;
    newApplication->status = 0; // Initialize status to Pending
    newApplication->next = loanApplications;
    loanApplications = newApplication;
    
    
    printf("\n");
    printf("+---------------------------------------+\n");
    printf("¦ Loan Application Submitted         ¦\n");
    printf("+---------------------------------------¦\n");
    printf("¦ Account Holder: %-20s   ¦\n", accountHolder);
    printf("¦ Account Number: %-4d             ¦\n", accountNo);
    printf("¦ Requested Amount: $%-14.2f ¦\n", requestedAmount);
    printf("+---------------------------------------+\n");

}

// Function to approve or reject a loan application
void processLoanApplication(int accountNo, int status) {
    LoanApplication* current = loanApplications;
        
    while (current != NULL) {
        if (current->accountNo == accountNo) {
            current->status = status;
            printf("\nLoan application for Account Number %d has been %s.\n", accountNo, (status == 1) ? "approved" : "rejected");
            return;
        }
        current = current->next;
    }

    printf("Loan application for Account Number %d not found.\n", accountNo);
}



// Function to calculate and display EMI for a loan application
void calculateEMI(int accountNo) {
    LoanApplication* current = loanApplications;

    while (current != NULL) {
        if (current->accountNo == accountNo && current->status == 1) {
            float loanAmount = current->requestedAmount;
            float interestRate = 10.0; // Assume a fixed interest rate of 10%
            int loanDuration = 12;    // Assume a loan duration of 12 months
            float emi = (loanAmount * interestRate / 100) / loanDuration;
            printf("\nEMI for the loan amount of %.2f for Account Number %d is %.2f per month.\n", loanAmount, accountNo, emi);
            return;
        }
        current = current->next;
    }

    printf("Loan application for Account Number %d not found or not approved.\n", accountNo);
}




void viewLoanStatusAndEMI(int accountNo) {
    LoanApplication* current = loanApplications;

    while (current != NULL) {
        if (current->accountNo == accountNo) {
            printBox("Loan Status and EMI Information");
            printf("Account Holder: %s\n", current->accountHolder);
            printf("Account Number: %d\n", current->accountNo);
            printf("Requested Amount: %.2f\n", current->requestedAmount);
              printf("\n");
              if(current->status == 1){
	
        printf("+---------------------------------------+\n");
        printf("¦ Loan Approved!                     ¦\n");
        printf("+---------------------------------------¦\n");
        printf("¦ Account Number: %-4d             ¦\n", accountNo);
        printf("+---------------------------------------+\n");
    }
    else if(current->status == 2)
    {
    	printf("+---------------------------------------+\n");
        printf("¦ Loan Rejected!                     ¦\n");
        printf("+---------------------------------------¦\n");
        printf("¦ Account Number: %-4d             ¦\n", accountNo);
        printf("+---------------------------------------+\n");
	}
	else
	{
		printf("+---------------------------------------+\n");
        printf("¦ Loan Pending!                         ¦\n");
        printf("+---------------------------------------¦\n");
        printf("¦ Account Number: %-4d             ¦\n", accountNo);
        printf("+---------------------------------------+\n");
	}
            printf("Status: %s\n", (current->status == 0) ? "Pending" : (current->status == 1) ? "Approved" : "Rejected");
            
            if (current->status == 1) {
                calculateEMI(accountNo);
            }
            return;
        }
        current = current->next;
    }

    printBox("Loan application not found or not approved.");
}

void enqueueLoanApplication(LoanApplication** head, LoanApplication* newApplication) {
    if (*head == NULL || newApplication->cibilScore >= (*head)->cibilScore) {
        newApplication->next = *head;
        *head = newApplication;
    } else {
        LoanApplication* current = *head;
        LoanApplication* previous = NULL;

        while (current != NULL && current->cibilScore > newApplication->cibilScore) {
            previous = current;
            current = current->next;
        }

        if (previous == NULL) {
            newApplication->next = *head;
            *head = newApplication;
        } else {
            previous->next = newApplication;
            newApplication->next = current;
        }
    }
}


// Manager interface functions

// Function to display all customer accounts
void displaySortedLoanApplications(LoanApplication* head) {
    LoanApplication* current = head;

    if (current == NULL) {
        printBox("No loan applications found.");
        return;
    }

    printf("\nLoan Applications (Sorted by CIBIL Score):\n");
    while (current != NULL) {
        printf("Account Holder: %s, Account Number: %d, Requested Amount: %.2f, CIBIL Score: %f, Status: %s\n",
            current->accountHolder, current->accountNo, current->requestedAmount, current->cibilScore,
            current->status == 0 ? "Pending" : (current->status == 1 ? "Approved" : "Rejected"));
        current = current->next;
    }
}
void showAllAccounts() {
    Account* current = customerAccounts;

    if (current == NULL) {
        printBox("No customer accounts found.");
        return;
    }

    printf("\n+-----------------------------------------------------------+\n");
    printf("¦ Account Holder                ¦ Account Number¦ Balance     ¦\n");
    printf("+-------------------------------+---------------+-------------¦\n");
    while (current != NULL) {
        printf("¦ %-30s ¦ %-13d ¦ %-9.2f ¦\n", current->accountHolder, current->accountNo, current->balance);
        current = current->next;
    }
    printf("+-----------------------------------------------------------+\n");
}

// Function to search for a customer account by account number
void searchAccountByAccountNo(int accountNo) {
    Account* current = customerAccounts;
    int found = 0;

    while (current != NULL) {
        if (current->accountNo == accountNo) {
            printf("\nAccount Holder: %s, Account Number: %d, Balance: %.2f, CIBIL Score: %.2f\n", current->accountHolder, current->accountNo, current->balance, current->cibilScore);
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Account with Account Number %d not found.\n", accountNo);
    }
}

// Function to display all loan applications in priority order (based on requested amount)
void displayLoanApplications() {
    LoanApplication* current = loanApplications;

    if (current == NULL) {
        printBox("No loan applications found.");
        return;
    }

    printf("\nLoan Applications:\n");
    while (current != NULL) {
        printf("Account Holder: %s, Account Number: %d, Requested Amount: %.2f, Status: %s\n",
            current->accountHolder, current->accountNo, current->requestedAmount, current->status == 0 ? "Pending" : (current->status == 1 ? "Approved" : "Rejected"));
        current = current->next;
    }
}

// Manager interface function
void managerInterface(void) {
    int ch = 1;
    int accountNo;

 printf("\n");
        printf("+------------------------------------------+\n");
        printf("¦ Manager  Interface                       ¦\n");
        printf("+------------------------------------------¦\n");
        printf("¦ 1.Show all accounts                      ¦\n");
        printf("¦ 2.Search account by Account Number       ¦\n");
        printf("¦ 3.Display loan applications              ¦\n");
        printf("¦ 4.Process loan application               ¦\n");
        printf("¦ 5.Calculate EMI                          ¦\n");
        printf("¦ 6.Back to main menu                      ¦\n");
        printf("+------------------------------------------+\n");
        printf("\n");

    while (ch != 6) {
        printf("Enter choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1:
                showAllAccounts();
                break;
            case 2:
                printf("\nEnter account number to search: ");
                scanf("%d", &accountNo);
                searchAccountByAccountNo(accountNo);
                break;
            case 3:
                displaySortedLoanApplications(loanApplications);
                break;
            case 4:
                printf("\nEnter account number to process loan application: ");
                scanf("%d", &accountNo);
                int status;
                printf("Enter status (1 for Approved, 2 for Rejected): ");
                scanf("%d", &status);
                processLoanApplication(accountNo, status);
                break;
            case 5:
                printf("\nEnter account number to calculate EMI: ");
                scanf("%d", &accountNo);
                calculateEMI(accountNo);
                break;
            case 6:
                break;
            default:
                printf("Enter valid choice\n");
        }
    }
}

// Customer interface function
// Customer interface function
void customerInterface(void) {
      char accountHolder[50];
    int pin;
    int accountNo;
    float requestedAmount;
    int ch = 1,cs=0;
    int senderAccountNo, senderPin, receiverAccountNo;
    float amount;

 printf("\n");
        printf("+---------------------------------------+\n");
        printf("¦ Customer Interface                    ¦\n");
        printf("+---------------------------------------¦\n");
        printf("¦ 1. Create Account                     ¦\n");
        printf("¦ 2. Delete Account                     ¦\n");
        printf("¦ 3. Check Account Information          ¦\n");
        printf("¦ 4. Deposit                            ¦\n");
        printf("¦ 5. Withdraw                           ¦\n");
        printf("¦ 6. Transfer Money                     ¦\n");
        printf("¦ 7.View transaction history            ¦\n");
        printf("¦ 8.Apply for Loan                      ¦\n");
        printf("¦ 9.view loan status                    ¦\n");
        printf("¦ 10.Back to main menu                  ¦\n");
        printf("+---------------------------------------+\n");
int cibil;

    while (ch != 10) {
        printf("Enter choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1:
                printf("\nEnter account holder name: ");
                scanf("%s", accountHolder);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                printf("Enter CIBIL SCORE: ");
                scanf("%d",&cs);
                createAccount(accountHolder, pin,cs); // Initialize CIBIL score to 0
                break;
            case 2:
                printf("\nEnter account number to be deleted: ");
                scanf("%d", &accountNo);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                deleteAccount(accountNo, pin);
                break;
            case 3:
                printf("\nEnter account number: ");
                scanf("%d", &accountNo);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                checkAccountInfo(accountNo, pin);
                break;
            case 4:
                printf("\nEnter account number: ");
                scanf("%d", &accountNo);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                printf("Enter amount to deposit: ");
                scanf("%f", &amount);
                deposit(accountNo, pin, amount);
                break;
            case 5:
                printf("\nEnter account number: ");
                scanf("%d", &accountNo);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                printf("Enter amount to withdraw: ");
                scanf("%f", &amount);
                withdrawal(accountNo, pin, amount);
                break;
            case 6:
                printf("\nEnter sender account number: ");
                scanf("%d", &senderAccountNo);
                printf("Enter sender PIN: ");
                scanf("%d", &senderPin);
                printf("Enter receiver account number: ");
                scanf("%d", &receiverAccountNo);
                printf("Enter transfer amount: ");
                scanf("%f", &amount);
                transfer(senderAccountNo, senderPin, receiverAccountNo, amount);
                break;
            case 8:
                 printf("\nEnter account holder name: ");
                scanf("%s", accountHolder);
                printf("Enter account number: ");
                scanf("%d", &accountNo);
                printf("Enter requested loan amount: ");
                scanf("%f", &requestedAmount);
                printf("Enter cibil score");
                scanf("%d",&cibil);
                LoanApplication* newApplication = (LoanApplication*)malloc(sizeof(LoanApplication));
                strcpy(newApplication->accountHolder, accountHolder);
                newApplication->accountNo = accountNo;
                newApplication->cibilScore = cibil; // Initialize CIBIL score to 0
                newApplication->requestedAmount = requestedAmount;
                newApplication->status = 0; // Initialize status to Pending
                newApplication->next = NULL;
                enqueueLoanApplication(&loanApplications, newApplication);
                
                
                
                printf("\nLoan Application Submitted Successfully!\n");
                break;
                case 9:
                	printf("\nEnter account number: ");
                scanf("%d", &accountNo);
                viewLoanStatusAndEMI(accountNo);
                	break;
                	case 10:
                		break;
            default:
                printf("Enter valid choice\n");
            case 7:
                printf("\nEnter account number: ");
                scanf("%d", &accountNo);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                Account* current = customerAccounts;
                while (current != NULL) {
                    if (current->accountNo == accountNo && current->pin == pin) {
                        displayTransactionHistory(current);
                        break;
                    }
                    current = current->next;
                }
                if (current == NULL) {
                    printf("Account with Account Number %d and PIN %d not found.\n", accountNo, pin);
                }
                break;
        }
    }
}

int main() {
    int ch = 1;

        
    printf("+---------------------------------------+\n");
    printf("¦ Welcome to Bank Of India              ¦\n");
    printf("+---------------------------------------+\n");

    while (ch != 3) {
    	printf("\n");
    printf("+---------------------------+\n");
    printf("¦ 1.Customer Interface      ¦\n");
    printf("¦ 2.Manager Interface       ¦\n");
    printf("¦ 3.Exit                    ¦\n");
    printf("+---------------------------+\n");
       
        printf("Enter choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1:
                customerInterface();
                break;
            case 2:
                managerInterface();
                break;
            case 3:
                 printBox("Exiting the Bank Management System. Goodbye!");
                break;
            default:
                printf("Enter valid choice\n");
                
        }
    }

    return 0;
}