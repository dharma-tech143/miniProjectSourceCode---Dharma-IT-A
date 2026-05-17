// Bank-account program reads a random-access file sequentially,
    // updates data already written to the file, creates new data to
    // be placed in the file, and deletes data previously in the file.
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <time.h>

    
    // clientData structure definition
    struct clientData
    {
        unsigned int acctNum; // account number
        char lastName[15];    // account last name
        char firstName[10];
        char gender[10];        // ADD
        char dob[12];           // ADD (DD/MM/YYYY)
        char nationality[20];  // ADD nationality of account holder
        char accountType[20];  // ADD (e.g. Savings, Checking)
        double balance;       // account balance
        int pin;               //pin number for authentication (ADD)
    };                        // end structure clientData

    struct adminData
    {
        char username[20];
        char password[20];
    };

    // prototypes
    unsigned int enterChoice(int role);
    void textFile(FILE *readPtr);
    void updateRecord(FILE *fPtr);
    void newRecord(FILE *fPtr);
    void deleteRecord(FILE *fPtr);
    void searchRecord(FILE *fPtr);
    void displayAll(FILE *fPtr);
    int isValidDOB(char *dob);
    void clearBuffer();
    void deposit(FILE *fPtr);
    void withdraw(FILE *fPtr);
    void initializeFile();
    void transfer(FILE *fPtr);
    void logTransaction(char *type, unsigned int acc1, unsigned int acc2, double amount, double balance);
    void viewTransactions();
    void viewTransactionsByAccount();
    void generateStatement(FILE *fPtr);
    void leaderboard(FILE *fPtr);
    void initializeAdmin();

    // ------------------ HELPER FUNCTIONS ------------------

    void clearBuffer()
    {
        while (getchar() != '\n');
    }

    int isValidDOB(char *dob)
    {
        if (strlen(dob) != 10)
            return 0;

        if (dob[2] != '/' || dob[5] != '/')
            return 0;

        for (int i = 0; i < 10; i++)
        {
            if (i == 2 || i == 5)
                continue;

            if (dob[i] < '0' || dob[i] > '9')
                return 0;
        }
        return 1;
    }

    int login()
    {
        char user[20], pass[20];
        struct adminData admin;

        FILE *fp = fopen("admin.dat", "rb");

        if (fp == NULL)
        {
            printf("Admin file not found!\n");
            return 0;
        }

        fread(&admin, sizeof(struct adminData), 1, fp);
        fclose(fp);

        printf("\n--- Admin Login ---\n");

        printf("Username: ");
        scanf("%19s", user);

        printf("Password: ");
        scanf("%19s", pass);

        if (strcmp(user, admin.username) == 0 &&
            strcmp(pass, admin.password) == 0)
        {
            printf("Login successful!\n");
            return 1;
        }
        else
        {
            printf("Invalid credentials!\n");
            return 0;
        }
    }
    
    int compareBalance(const void *a, const void *b)
    {
        struct clientData *c1 = (struct clientData *)a;
        struct clientData *c2 = (struct clientData *)b;

    // descending order
        if (c2->balance > c1->balance)
            return 1;
        else if (c2->balance < c1->balance)
            return -1;
        else
            return 0;
    }

    void initializeAdmin()
    {
        FILE *fp = fopen("admin.dat", "rb");~

        if (fp != NULL)
        {
            fclose(fp);
            return; // already exists
        }

        fp = fopen("admin.dat", "wb");

        struct adminData admin;

        printf("Set initial admin username: ");
        scanf("%19s", admin.username);

        printf("Set initial admin password: ");
        scanf("%19s", admin.password);

        fwrite(&admin, sizeof(struct adminData), 1, fp);

        fclose(fp);

        printf("Admin credentials saved successfully!\n");
    }

    int main(int argc, char *argv[])
    {
        FILE *cfPtr;         // credit.dat file pointer
        unsigned int choice; // user's choice

        int role;

        printf("\n=====================================================\n");
        printf("            BANK MANAGEMENT SYSTEM\n");
        printf("=====================================================\n");

        printf("\n-------------------- LOGIN ---------------------------\n");
        printf("1) ADMIN\n");
        printf("2) USER\n");
        printf("-----------------------------------------------------\n");

        printf("Select role: ");
        scanf("%d", &role);

        initializeAdmin();

        if (role == 1)
        {
            if (!login())
            {
                printf("Access denied!\n");
                return 0;
            }
        printf("Admin access granted.\n");
        }
        else if (role == 2)
        {
            printf("User access granted.\n");
        }
        else
        {
            printf("Invalid choice!\n");
            return 0;
        }
    
        // fopen opens the file; exits if file cannot be opened
        if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
        {
            initializeFile();
            cfPtr = fopen("credit.dat", "rb+");
        }

        // enable to enter choice
        while ((choice = enterChoice(role)) != 5)
        {
            switch (choice)
            {
            // ---------------- ADMIN ONLY ----------------
            case 1:
            case 2:
            case 3:
            case 4:
            case 7:
            case 14:
                if (role != 1)
                {
                    printf("Access denied! Admin only.\n");
                    break;
                }

                if (choice == 1) textFile(cfPtr);
                else if (choice == 2) updateRecord(cfPtr);
                else if (choice == 3) newRecord(cfPtr);
                else if (choice == 4) deleteRecord(cfPtr);
                else if (choice == 7) displayAll(cfPtr);
                else if (choice == 14) leaderboard(cfPtr);
                break;

            // ---------------- BOTH (ADMIN + USER) ----------------
            case 6:
                searchRecord(cfPtr);
                break;

            case 8:
                deposit(cfPtr);
                break;

            case 9:
                withdraw(cfPtr);
                break;

            case 10:
                transfer(cfPtr);
                break;

            case 11:
                viewTransactions();
                break;

            case 12:
                viewTransactionsByAccount();
                break;

            case 13:
                generateStatement(cfPtr);
                break;

            default:
                printf("Invalid choice!\n");
                break;
            } // end switch
        }     // end while

        fclose(cfPtr); // fclose closes the file
    } // end main

    // create formatted text file for printing
    void textFile(FILE *readPtr)
    {
        FILE *writePtr;
        struct clientData client;

        if ((writePtr = fopen("accounts.txt", "w")) == NULL)
        {
            puts("File could not be opened.");
            return;
        }

        rewind(readPtr);

        // TOP BORDER
        fprintf(writePtr, "============================================================================================================\n");

        // HEADER
        fprintf(writePtr, "| %-4s | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %-10s |\n",
            "Acct", "Last Name", "First Name", "Gender", "DOB", "Nationality", "Type", "Balance");

        // SEPARATOR
        fprintf(writePtr, "============================================================================================================\n");

        // DATA
        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "| %-4d | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %10.2f |\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.gender,
                    client.dob,
                    client.nationality,
                    client.accountType,
                    client.balance);
            }
        }

        // BOTTOM BORDER
        fprintf(writePtr, "============================================================================================================\n");

        fclose(writePtr);

        printf("accounts.txt created successfully!\n"); // DEBUG MESSAGE
    }

    // update balance in record
    void updateRecord(FILE *fPtr)
    {
        unsigned int account; // account number
        double transaction;   // transaction amount
        // create clientData with no information
        struct clientData client = {0, "", "", "", "", "", "", 0.0, 0};

        // obtain number of account to update
        printf("%s", "Enter account to update ( 1 - 100 ): ");
        scanf("%d", &account);
        if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

        // move file pointer to correct record in file
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        // read record from file
        fread(&client, sizeof(struct clientData), 1, fPtr);
        // display error if account does not exist
        if (client.acctNum == 0)
        {
            printf("Account #%d has no information.\n", account);
        }
        else
        { 
            //PIN CHECK
            int enteredPin;
            printf("Enter PIN: ");
            scanf("%d", &enteredPin);
            if (enteredPin != client.pin)
            {
                printf("Incorrect PIN!\n");
                return;
            }
            // update record
            printf("\n--- BEFORE UPDATE ---\n");
            printf("| %-4d | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %10.2f |\n",
            client.acctNum,
            client.lastName,
            client.firstName,
            client.gender,
            client.dob,
            client.nationality,
            client.accountType,
            client.balance);

            // request transaction amount from user
            printf("%s", "Enter charge ( + ) or payment ( - ): ");
            scanf("%lf", &transaction);
            if (client.balance + transaction < 0){
                printf("Insufficient balance!\n");
                return;
            }
            client.balance += transaction; // update record balance

            printf("\n--- AFTER UPDATE ---\n");
            printf("| %-4d | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %10.2f |\n",
            client.acctNum,
            client.lastName,
            client.firstName,
            client.gender,
            client.dob,
            client.nationality,
            client.accountType,
            client.balance);
            // move file pointer to correct record in file
            // move back by 1 record length
            fseek(fPtr, -(long int)sizeof(struct clientData), SEEK_CUR);
            // write updated record over old record in file
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
        } // end else
    } // end function updateRecord

    void deposit(FILE *fPtr)
    {
        unsigned int account;
        double amount;

        struct clientData client = {0, "", "", "", "", "", "", 0.0, 0};
        
        printf("Enter account number: ");
        scanf("%u", &account);
        
        if (account < 1 || account > 100)
        {
            printf("Invalid account number!\n");
            return;
        }

        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);
        
        if (client.acctNum == 0)
        {
            printf("Account not found!\n");
            return;
        }

        // PIN CHECK
        int enteredPin;
        printf("Enter PIN: ");
        scanf("%d", &enteredPin);

        if (enteredPin != client.pin)
        {
            printf("Incorrect PIN!\n");
            return;
        }

        printf("Enter amount to deposit: ");
        scanf("%lf", &amount);

        if (amount <= 0)
        {
            printf("Invalid amount!\n");
            return;
    }

    client.balance += amount;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Deposit successful! New balance: %.2f\n", client.balance);
    logTransaction("DEPOSIT", account, 0, amount, client.balance);
    }

    void withdraw(FILE *fPtr)
    {
        unsigned int account;
        double amount;

        struct clientData client = {0, "", "", "", "", "", "", 0.0, 0};

        printf("Enter account number: ");
        scanf("%u", &account);

        if (account < 1 || account > 100)
        {
            printf("Invalid account number!\n");
            return;
        }

        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);

        if (client.acctNum == 0)
        {
            printf("Account not found!\n");
            return;
        }

        // PIN CHECK
        int enteredPin;
        printf("Enter PIN: ");
        scanf("%d", &enteredPin);

        if (enteredPin != client.pin)
        {
            printf("Incorrect PIN!\n");
            return;
        }

        printf("Enter amount to withdraw: ");
        scanf("%lf", &amount);

        if (amount <= 0)
        {
            printf("Invalid amount!\n");
            return;
        }

        if (client.balance < amount)
        {
            printf("Insufficient balance!\n");
            return;
        }

        client.balance -= amount;

        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Withdrawal successful! Remaining balance: %.2f\n", client.balance);
        logTransaction("WITHDRAW", account, 0, amount, client.balance);
    }

    void transfer(FILE *fPtr)
    {
        unsigned int fromAcc, toAcc;
        double amount;
        
        struct clientData sender = {0}, receiver = {0};
        
        printf("Enter sender account number: ");
        scanf("%u", &fromAcc);

        printf("Enter receiver account number: ");
        scanf("%u", &toAcc);

        if (fromAcc < 1 || fromAcc > 100 || toAcc < 1 || toAcc > 100)
        {
            printf("Invalid account number!\n");
            return;
        }

        if (fromAcc == toAcc)
        {
            printf("Cannot transfer to the same account!\n");
            return;
        }

        // -------- READ SENDER --------
        fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&sender, sizeof(struct clientData), 1, fPtr);

        if (sender.acctNum == 0)
        {
            printf("Sender account not found!\n");
            return;
        }   

        // -------- PIN CHECK --------
        int enteredPin;
        printf("Enter sender PIN: ");
        scanf("%d", &enteredPin);

        if (enteredPin != sender.pin)
        {
            printf("Incorrect PIN!\n");
            return;
        }

        // -------- READ RECEIVER --------
        fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&receiver, sizeof(struct clientData), 1, fPtr);

        if (receiver.acctNum == 0)
        {
            printf("Receiver account not found!\n");
            return;
        }

        // -------- AMOUNT --------
        printf("Enter amount to transfer: ");
        scanf("%lf", &amount);

        if (amount <= 0)
        {
            printf("Invalid amount!\n");
            return;
        }

        if (sender.balance < amount)
        {
            printf("Insufficient balance!\n");
            return;
        }
        // -------- UPDATE BALANCES --------
        sender.balance -= amount;
        receiver.balance += amount;

        // -------- WRITE BACK SENDER --------
        fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&sender, sizeof(struct clientData), 1, fPtr);

        // -------- WRITE BACK RECEIVER --------
        fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&receiver, sizeof(struct clientData), 1, fPtr);

        printf("Transfer successful!\n");
        printf("Sender new balance: %.2f\n", sender.balance);

        logTransaction("TRANSFER", fromAcc, toAcc, amount, sender.balance);
    }

    // delete an existing record
    void deleteRecord(FILE *fPtr)
    {
        struct clientData client;                       // stores record read from file
        struct clientData blankClient = {0, "", "", "", "", "", "", 0.0, 0}; // blank client
        unsigned int accountNum;                        // account number

        // obtain number of account to delete
        printf("%s", "Enter account number to delete ( 1 - 100 ): ");
        scanf("%d", &accountNum);
        if (accountNum < 1 || accountNum > 100)
        {
        printf("Invalid account number!\n");
        return;
    }

        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // read record from file
        fread(&client, sizeof(struct clientData), 1, fPtr);
        // display error if record does not exist
        if (client.acctNum == 0)
        {
            printf("Account %d does not exist.\n", accountNum);
        } // end if
        else
        { 
            int enteredPin;
            printf("Enter PIN: ");
            scanf("%d", &enteredPin);
            
            if (enteredPin != client.pin)
            {
                printf("Incorrect PIN!\n");
                return;
            }
    
            // delete record
            // move file pointer to correct record in file
            fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
            // replace existing record with blank record
            fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        } // end else
    } // end function deleteRecord


    void searchRecord(FILE *fPtr)
    {
        unsigned int acc;
        struct clientData client = {0, "", "", "", "", "", "", 0.0, 0};

        printf("Enter account number to search (1-100): ");
        scanf("%u",&acc);

        if(acc < 1 || acc > 100)
        {
            printf("Invalid account number\n");
            return;
        }

        fseek(fPtr,(acc-1)*sizeof(struct clientData),SEEK_SET);
        fread(&client,sizeof(struct clientData),1,fPtr);

        if(client.acctNum == 0)
        {
            printf("Account not found\n");
        }
        else
        { //PIN CHECK
            int enteredPin;
            printf("Enter PIN: ");
            scanf("%d", &enteredPin);
            if (enteredPin != client.pin)
            {
                printf("Incorrect PIN!\n");
                return;
            }

            // ✅ Only show details if PIN is correct
            printf("\n============================================================================================================\n");
            printf("| %-4s | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %-10s |\n",
                "Acct", "Last Name", "First Name", "Gender", "DOB", "Nationality", "Type", "Balance");
            printf("============================================================================================================\n");

            printf("| %-4d | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %10.2f |\n",
            client.acctNum,
            client.lastName,
            client.firstName,
            client.gender,
            client.dob,
            client.nationality,
            client.accountType,
            client.balance);
            printf("============================================================================================================\n");
        }
    }


    // create and insert record
    void newRecord(FILE *fPtr)
    {
        // create clientData with default information
        struct clientData client = {0, "", "", "", "", "", "", 0.0, 0};
        unsigned int accountNum; // account number

        // obtain number of account to create
        printf("%s", "Enter new account number ( 1 - 100 ): ");
        scanf("%d", &accountNum);
        if (accountNum < 1 || accountNum > 100)
        {
        printf("Invalid account number!\n");
        return;
        }

        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // read record from file
        fread(&client, sizeof(struct clientData), 1, fPtr);
        // display error if account already exists
        if (client.acctNum != 0)
        {
            printf("Account #%d already contains information.\n", client.acctNum);
        } // end if
        else
        { // create record
            // user enters last name, first name and balance
            printf("Enter lastname firstname gender DOB nationality accountType balance\n");
            scanf("%14s %9s %9s %11s %19s %19s %lf",
            client.lastName,
            client.firstName,
            client.gender,
            client.dob,
            client.nationality,
            client.accountType,
            &client.balance);
            
            for (int i = 0; client.accountType[i]; i++)
            {
                client.accountType[i] = tolower(client.accountType[i]);
            }

            if (strcmp(client.accountType, "savings") != 0 &&
            strcmp(client.accountType, "current") != 0 &&
            strcmp(client.accountType, "student") != 0 &&
            strcmp(client.accountType, "senior") != 0)
            {
                printf("Invalid account type!\n");
            return;
            }

            if (!isValidDOB(client.dob)){
                printf("Invalid DOB format! Use DD/MM/YYYY\n");
                return;
            }

            if (client.balance < 0){
                printf("Balance cannot be negative!\n");
                return;
            }
            client.acctNum = accountNum;

            int pin1, pin2;
            printf("Set 4-digit PIN: ");
            scanf("%d", &pin1);
            
            if (pin1 < 1000 || pin1 > 9999)
            {
                printf("PIN must be 4 digits!\n");
                return;
            }
            printf("Confirm PIN: ");
            scanf("%d", &pin2);
            if (pin1 != pin2)
            {
                printf("PIN mismatch!\n");
                return;
            }
            client.pin = pin1;

            // move file pointer to correct record in file
            fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
            // insert record in file
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
        } // end else
    } // end function newRecord

    void displayAll(FILE *fPtr)
    {
        struct clientData client;
        rewind(fPtr);

        printf("\nExisting Accounts:\n");
        printf("\n============================================================================================================\n");
        printf("| %-4s | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %-10s |\n",
           "Acct", "Last Name", "First Name", "Gender", "DOB", "Nationality", "Type", "Balance");
        printf("============================================================================================================\n");

        while(fread(&client,sizeof(struct clientData),1,fPtr))
        {
            if(client.acctNum!=0)
                printf("| %-4d | %-14s | %-11s | %-6s | %-10s | %-13s | %-10s | %10.2f |\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.gender,
                   client.dob,
                   client.nationality,
                   client.accountType,
                   client.balance);
        }
        printf("============================================================================================================\n");
    }
    
    void leaderboard(FILE *fPtr)
    {
        struct clientData clients[100];
        int count = 0;

        rewind(fPtr);

        while (fread(&clients[count], sizeof(struct clientData), 1, fPtr) == 1)
        {
            if (clients[count].acctNum != 0)
            {
                count++;
            }
        }

        if (count == 0)
        {
            printf("No accounts found.\n");
            return;
        }

        //SORTING 
        qsort(clients, count, sizeof(struct clientData), compareBalance);

        printf("\n=========== LEADERBOARD (Top Balances) ===========\n");
        printf("===========================================================\n");
        printf("| %-4s | %-14s | %-11s | %-10s |\n",
           "Rank", "Last Name", "First Name", "Balance");
        printf("===========================================================\n");

        for (int i = 0; i < count; i++)
        {
            printf("| %-4d | %-14s | %-11s | %10.2f |\n",
               i + 1,
               clients[i].lastName,
               clients[i].firstName,
               clients[i].balance);
        }

        printf("===========================================================\n");
    }

    void initializeFile()
    {
        FILE *fp = fopen("credit.dat", "wb");
        if (fp == NULL)
        {
            printf("Error creating file.\n");
            return;
        }

            struct clientData blank = {0, "", "", "", "", "", "", 0.0, 0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, fp);
        }

        fclose(fp);
        printf("File initialized successfully.\n");
    }

    void logTransaction(char *type, unsigned int acc1, unsigned int acc2, double amount, double balance)
    {
        FILE *fp = fopen("transaction.txt", "a");

        if (fp == NULL)
            {
                printf("Error opening transaction file!\n");
                return;
        }

        // -------- GET CURRENT TIME --------
        time_t now;
        time(&now);
        struct tm *local = localtime(&now);

        char timeStr[50];
        strftime(timeStr, sizeof(timeStr), "%d/%m/%Y %H:%M:%S", local);

        // -------- WRITE TO FILE --------
        if (strcmp(type, "TRANSFER") == 0)
        {
            fprintf(fp, "[%s] TRANSFER: From Acc %u -> To Acc %u | Amount: %.2f\n",
                timeStr, acc1, acc2, amount);
        }
        else
        {
            fprintf(fp, "[%s] %s: Acc %u | Amount: %.2f | Balance: %.2f\n",
                timeStr, type, acc1, amount, balance);
        }

        fclose(fp);
    }

    void viewTransactions()
    {
        FILE *fp = fopen("transaction.txt", "r");

        if (fp == NULL)
        {
            printf("No transaction history found.\n");
            return;
        }

        char line[200];

        printf("\n=========== TRANSACTION HISTORY ===========\n");

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            printf("%s", line);   // already includes time
        }

        printf("=========== END OF HISTORY ===========\n");

        fclose(fp);
    }

    void viewTransactionsByAccount()
    {
        FILE *fp = fopen("transaction.txt", "r");

        if (fp == NULL)
        {
            printf("No transaction history found.\n");
            return;
        }

        unsigned int acc;
        char line[200];
        int found = 0;

        printf("Enter account number: ");
        scanf("%u", &acc);

        printf("\n===== Transactions for Account %u =====\n", acc);

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            char search[20];
            sprintf(search, "Acc %u", acc);

            char searchFrom[25];
            sprintf(searchFrom, "From Acc %u", acc);

            char searchTo[25];
            sprintf(searchTo, "To Acc %u", acc);

            if (strstr(line, search) || strstr(line, searchFrom) || strstr(line, searchTo))
            {
                printf("%s", line);   // includes timestamp
                found = 1;
            }
        }

        if (!found)
        {
            printf("No transactions found for this account.\n");
        }
    
        printf("===== END =====\n");

        fclose(fp);
    }

    void generateStatement(FILE *fPtr)
    {
        unsigned int acc;
        struct clientData client = {0};
        FILE *fp;

        printf("Enter account number: ");
        scanf("%u", &acc);

        if (acc < 1 || acc > 100)
        {
            printf("Invalid account number!\n");
            return;
        }

        // FETCH ACCOUNT
        fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);

        if (client.acctNum == 0)
        {
            printf("Account not found!\n");
            return;
        }

        // PIN CHECK
        int enteredPin;
        printf("Enter PIN: ");
        scanf("%d", &enteredPin);

        if (enteredPin != client.pin)
        {
            printf("Incorrect PIN!\n");
            return;
        }

        // CREATE FILE
        char filename[50];
        sprintf(filename, "statement_%u.txt", acc);

        fp = fopen(filename, "w");

        if (fp == NULL)
        {
            printf("Error creating file!\n");
            return;
        }

        fprintf(fp, "=============================================\n");
        fprintf(fp, "         BANK ACCOUNT STATEMENT              \n");
        fprintf(fp, "=============================================\n");

        fprintf(fp, "Account Number : %u\n", client.acctNum);
        fprintf(fp, "Name           : %s %s\n", client.firstName, client.lastName);
        fprintf(fp, "Gender         : %s\n", client.gender);
        fprintf(fp, "DOB            : %s\n", client.dob);
        fprintf(fp, "Nationality    : %s\n", client.nationality);
        fprintf(fp, "Account Type   : %s\n", client.accountType);
        fprintf(fp, "Current Balance: %.2f\n", client.balance);

        fprintf(fp, "\n------------ TRANSACTIONS ------------\n");

        FILE *tf = fopen("transaction.txt", "r");

        if (tf != NULL)
        {
            char line[200];
            int found = 0;

            while (fgets(line, sizeof(line), tf))
            {
                char s1[20], s2[25], s3[25];

                sprintf(s1, "Acc %u", acc);
                sprintf(s2, "From Acc %u", acc);
                sprintf(s3, "To Acc %u", acc);

                if (strstr(line, s1) || strstr(line, s2) || strstr(line, s3))
                {
                    fprintf(fp, "%s", line);
                    found = 1;
                }
            }
        

        if (!found)
            fprintf(fp, "No transactions found.\n");

        fclose(tf);
        }

        else
        {
            fprintf(fp, "Transaction file not found.\n");
        }

        fprintf(fp, "=============================================\n");

        fclose(fp);

        printf("Statement generated: %s\n", filename);
    }

    // enable to input menu choice
    unsigned int enterChoice(int role)
    {
        unsigned int menuChoice;

        if (role == 1) // ADMIN MENU
        {
            printf("\n--- ADMIN MENU ---\n");
            printf("1 - store accounts to text file\n");
            printf("2 - update account\n");
            printf("3 - add new account\n");
            printf("4 - delete account\n");
            printf("5 - end program\n");
            printf("6 - search account\n");
            printf("7 - display all accounts\n");
            printf("8 - deposit\n");
            printf("9 - withdraw\n");
            printf("10 - transfer\n");
            printf("11 - view all transactions\n");
            printf("12 - view transactions by account\n");
            printf("13 - generate statement\n");
            printf("14 - leaderboard\n");
        }
        else // USER MENU
        {
            printf("\n--- USER MENU ---\n");
            printf("5 - end program\n");
            printf("6 - search your account\n");
            printf("8 - deposit\n");
            printf("9 - withdraw\n");
            printf("10 - transfer\n");
            printf("11 - view transactions\n");
            printf("12 - view your transactions\n");
            printf("13 - generate statement\n");
        }

        printf("Enter your choice: ");
        scanf("%u", &menuChoice);

        return menuChoice;
    } // end function enterChoice