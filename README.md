# 🏦 Bank Account Management System (C Project)

---

## 📖 Overview

A **Bank Account Management System** built in C using **file handling and structured data storage**. This project simulates real-world banking operations such as account creation, transactions, authentication, and statement generation using a **random-access file system (********`credit.dat`************)**.


---

## 📌 Features

### 🔐 Authentication

* Admin login system
* PIN-based verification for each account
* PIN is stored in the binary file (`admin.dat`)
* Prevents unauthorized access to sensitive operations

 

 
### 👤 Account Management

* Create new accounts with:
 

  * Name, Gender, DOB
  * Nationality
  * Account Type (Savings, Current, Student, Senior)
* Update account balance
* Delete existing accounts
* Search for specific accounts
* Display all accounts in formatted table

 
💾 Data stored in binary file (`credit.dat`) and exported to `accounts.txt`

 
### 💰 Transactions

* Deposit money
* Withdraw money
* Transfer funds between accounts
* Automatic balance validation (no negative balances)

### 📜 Transaction Logging

* Every transaction is recorded in `transaction.txt`
* Includes:

  * Timestamp
  * Transaction type
  * Account details
  * Amount and balance

### 📄 Statement Generation

* Generate individual account statements
* Stored as: `statement_<account_number>.txt`
* Includes:

  * Account details
  * Full transaction history

### 📊 File Handling

* Binary file: `credit.dat` (stores all accounts)
* Text file: `accounts.txt` (formatted for printing)
* Log file: `transaction.txt` (transaction history)

---

## 🗂️ Project Structure

```
📁 Project Folder
│
├── credit.dat            # Binary file (account database)
├── accounts.txt          # Formatted account list
├── transaction.txt       # Transaction history log
├── statement_*.txt       # Generated account statements
├── trans.c               # Main source code
```

---

## ⚙️ How It Works

### 🔄 File Initialization

* If `credit.dat` does not exist:

  * System initializes **100 empty records**
  * Each record represents one account slot

### 📍 Random Access Logic

* Each account is stored at a fixed position:

```
Position = (account_number - 1) * sizeof(struct clientData)
```

* Enables fast read/write operations

---

## 🧠 Data Structure

```c
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    char gender[10];
    char dob[12];
    char nationality[20];
    char accountType[20];
    double balance;
    int pin;
};
```

---

## 🖥️ Menu Options

```
1  - Export accounts to text file
2  - Update account
3  - Add new account
4  - Delete account
5  - Exit
6  - Search account
7  - Display all accounts
8  - Deposit money
9  - Withdraw money
10 - Transfer money
11 - View all transactions
12 - View transactions by account
13 - Generate account statement
14 - Leaderboard
```

---

## 🔒 Security Features

* Admin login required to access system  
  - Username: admin  
  - Password: 1234  

* PIN stored in binary file (`admin.dat`) (not encrypted)

* PIN verification required for:
  - Deposit
  - Withdraw
  - Transfer
  - Update
  - Delete
  - Statement generation
* Input validation:

  * Account number range (1–100)
  * DOB format (DD/MM/YYYY)
  * Valid account types
  * Non-negative balances

---

## 🧪 Sample Outputs

### 📄 Account Table (`accounts.txt`)

```
| Acct | Last Name     | First Name | Gender | DOB        | Nationality  | Type     | Balance    |
============================================================================================================
| 1    | Kumar         | Ravi       | Male   | 12/05/2000 | Indian       | savings  |   5000.00  |
```

### 📜 Transaction Log

```
[22/03/2026 14:32:10] DEPOSIT: Acc 1 | Amount: 1000.00 | Balance: 6000.00
[22/03/2026 15:10:45] TRANSFER: From Acc 1 -> To Acc 2 | Amount: 500.00
```

---

## 🚀 How to Compile & Run

### 🛠️ Compile

```bash
gcc trans.c -o trans
```

### ▶️ Run

```bash
./trans
```

---

## 💡 Unique Highlights

* 📌 Real-world banking simulation
* ⚡ Fast random-access file handling
* 🔍 Transaction filtering by account
* 📄 Auto-generated statements
* 🔐 Role-based access control (Admin + PIN verification)
* 🧾 Clean formatted outputs

---

## ⚠️ Limitations

* Maximum 100 accounts
* PIN stored without encryption (not secure for real-world use)
* CLI-based interface (no GUI)