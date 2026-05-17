// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void createFileIfMissing(void);
void textFile(FILE *readPtr);
void listRecords(FILE *fPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void clearInput(void);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    createFileIfMissing();

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: Unable to open credit.dat for read/write.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    puts("\nWelcome to the simple bank account manager.");
    puts("Use the menu below to manage customer records.\n");

    while ((choice = enterChoice()) != 6)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            listRecords(cfPtr);
            break;
        default:
            puts("Please select a valid option from the menu.");
            break;
        }
    }

    fclose(cfPtr);
    puts("\nThank you for using the bank account manager. Goodbye!\n");
    return 0;
}

void createFileIfMissing(void)
{
    FILE *file;
    struct clientData blankClient = {0, "", "", 0.0};

    if ((file = fopen("credit.dat", "rb")) == NULL)
    {
        if ((file = fopen("credit.dat", "wb")) == NULL)
        {
            puts("Unable to create credit.dat. Please check permissions.");
            exit(EXIT_FAILURE);
        }

        for (unsigned int i = 0; i < 100; ++i)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, file);
        }

        fclose(file);
        puts("Created new credit.dat with 100 blank records.");
    }
    else
    {
        fclose(file);
    }
}

unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    do
    {
        puts("1 - Save all records to accounts.txt");
        puts("2 - Update an existing account");
        puts("3 - Create a new account");
        puts("4 - Delete an account");
        puts("5 - List all active accounts");
        puts("6 - Exit");
        printf("Enter your choice: ");

        if (scanf("%u", &menuChoice) != 1)
        {
            clearInput();
            puts("Invalid input. Please enter a number between 1 and 6.\n");
            menuChoice = 0;
        }
    } while (menuChoice < 1 || menuChoice > 6);

    clearInput();
    return menuChoice;
}

void clearInput(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        ;
    }
}

void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("Unable to open accounts.txt for writing.");
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }

    fclose(writePtr);
    puts("Saved active account data to accounts.txt.\n");
}

void listRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    int found = 0;

    rewind(fPtr);
    puts("\nActive accounts:");
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum, client.lastName, client.firstName, client.balance);
            found = 1;
        }
    }

    if (!found)
    {
        puts("No active accounts found.");
    }

    puts("");
}

void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    if (scanf("%u", &account) != 1 || account < 1 || account > 100)
    {
        clearInput();
        puts("Invalid account number. Please try again.\n");
        return;
    }

    clearInput();
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n\n", account);
        return;
    }

    printf("Current account: %-6u %-15s %-10s %10.2f\n",
           client.acctNum, client.lastName, client.firstName, client.balance);
    printf("Enter charge (+) or payment (-): ");

    if (scanf("%lf", &transaction) != 1)
    {
        clearInput();
        puts("Invalid amount. Transaction canceled.\n");
        return;
    }

    client.balance += transaction;
    printf("Updated balance for %s %s: %10.2f\n\n",
           client.firstName, client.lastName, client.balance);

    fseek(fPtr, -((long) sizeof(struct clientData)), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    clearInput();
}

void deleteRecord(FILE *fPtr)
{
    unsigned int accountNum;
    struct clientData client = {0, "", "", 0.0};
    struct clientData blankClient = {0, "", "", 0.0};

    printf("Enter account number to delete (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        clearInput();
        puts("Invalid account number. Please try again.\n");
        return;
    }

    clearInput();
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%u does not exist.\n\n", accountNum);
        return;
    }

    printf("Deleting account: %-6u %-15s %-10s %10.2f\n",
           client.acctNum, client.lastName, client.firstName, client.balance);
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    puts("Account deleted successfully.\n");
}

void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        clearInput();
        puts("Invalid account number. Please try again.\n");
        return;
    }

    clearInput();
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n\n", accountNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        clearInput();
        puts("Invalid input. Account creation canceled.\n");
        return;
    }

    client.acctNum = accountNum;
    fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    clearInput();
    puts("Account created successfully.\n");
}
