#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_NAME_LENGTH 30
#define MAX_PHONE_LENGTH 10
#define MAX_EMAIL_LENGTH 30
#define MAX_OBSERVATIONS_LENGTH 250
#define MAX_ID_LENGTH 9
#define FILE_NAME "Contacts.bin"

typedef struct
{
    char id[MAX_ID_LENGTH];
    char name[MAX_NAME_LENGTH + 1];
    char phone[MAX_PHONE_LENGTH + 1];
    char email[MAX_EMAIL_LENGTH + 1];
    char observations[MAX_OBSERVATIONS_LENGTH + 1];
} Contact;

typedef struct
{
    int firstBlockAddress;
    int fileSize;
    int numContacts;
    int numInsertedContacts;
    int numDeletedContacts;
} FileHeader;

FILE *contactsFile;
FileHeader fileHeader;

void createFile();
void searchContactByID();
void insertContact();
void deleteContactByID();
void displayFileCharacteristics();
void displayFileOrganization();
void updateNumContacts();
void createOrderedContactsFile();
void printContacts();
int compareContactsByID(const void *a, const void *b);
void printorderedContacts();

int main()
{
    int choice;

    createFile();

    while (1)
    {
        printf("Enter your choice:\n");
        printf("1. Search contact by ID\n");
        printf("2. Insert a new contact\n");
        printf("3. Delete a contact\n");
        printf("4. Display file characteristics\n");
        printf("5. Display file organization\n");
        printf("6  Print nonordered contacts\n");
        printf("7. Print ordered contacts\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        printf("--------------------------------------\n");
        switch (choice)
        {
        case 1:
            searchContactByID();
            break;
        case 2:
            insertContact();
            break;
        case 3:
            deleteContactByID();
            break;
        case 4:
            displayFileCharacteristics();
            break;
        case 5:
            displayFileOrganization();
            break;
        case 6:
            printContacts();
            break;
        case 7:
            printorderedContacts();
            break;
        case 0:
            updateNumContacts();
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
        }
        printf("--------------------------------------\n");
    }

    return 0;
}

void createFile()
{
    contactsFile = fopen(FILE_NAME, "wb");
    if (contactsFile == NULL)
    {
        printf("Error creating file.\n");
        return;
    }

    fileHeader.firstBlockAddress = sizeof(FileHeader);
    fileHeader.fileSize = sizeof(FileHeader);
    fileHeader.numContacts = 0;
    fileHeader.numInsertedContacts = 0;
    fileHeader.numDeletedContacts = 0;

    fwrite(&fileHeader, sizeof(FileHeader), 1, contactsFile);

    fclose(contactsFile);
}

void searchContactByID()
{
    char id[MAX_ID_LENGTH];
    printf("Enter the ID to search: ");
    scanf("%s", id);

    contactsFile = fopen(FILE_NAME, "rb");
    if (contactsFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fseek(contactsFile, sizeof(FileHeader), SEEK_SET);

    Contact contact;
    int found = 0;
    int blockAddress = sizeof(FileHeader);
    int displacement = 0;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (strcmp(contact.id, id) == 0)
        {
            found = 1;
            break;
        }
        displacement++;
        if (displacement == sizeof(Contact))
        {
            blockAddress += sizeof(Contact);
            displacement = 0;
        }
    }

    if (found)
    {
        printf("Contact found at block address %d, displacement %d.\n", blockAddress, displacement);
    }
    else
    {
        printf("Contact not found.\n");
    }

    fclose(contactsFile);
}

void insertContact()
{
    Contact newContact;

    printf("Enter the details of the new contact:\n");
    printf("ID: ");
    scanf("%s", newContact.id);
    printf("Name: ");
    scanf("%s", newContact.name);
    printf("Phone: ");
    scanf("%s", newContact.phone);
    printf("Email: ");
    scanf("%s", newContact.email);
    printf("Observations: ");
    scanf("%s", newContact.observations);

    contactsFile = fopen(FILE_NAME, "r+b");
    if (contactsFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fseek(contactsFile, 0, SEEK_END);
    fwrite(&newContact, sizeof(Contact), 1, contactsFile);
    fileHeader.fileSize += sizeof(Contact);
    fileHeader.numContacts++;
    fileHeader.numInsertedContacts++;

    fseek(contactsFile, 0, SEEK_SET);
    fwrite(&fileHeader, sizeof(FileHeader), 1, contactsFile);

    fclose(contactsFile);
}

void deleteContactByID()
{
    char id[MAX_ID_LENGTH];

    printf("Enter the ID to delete: ");
    scanf("%s", id);

    contactsFile = fopen(FILE_NAME, "r+b");
    if (contactsFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fseek(contactsFile, sizeof(FileHeader), SEEK_SET);

    Contact contact;
    int found = 0;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (strcmp(contact.id, id) == 0)
        {
            found = 1;
            break;
        }
    }

    if (found)
    {
        fseek(contactsFile, -sizeof(Contact), SEEK_CUR);
        contact.id[0] = '*';
        fwrite(&contact, sizeof(Contact), 1, contactsFile);
        printf("Contact with ID %s has been deleted.\n", id);
    }
    else
    {
        printf("Contact with ID %s not found.\n", id);
    }

    fclose(contactsFile);
}

void displayFileOrganization()
{
    contactsFile = fopen(FILE_NAME, "rb");
    if (contactsFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fread(&fileHeader, sizeof(FileHeader), 1, contactsFile);

    printf("File Organization:\n");
    printf("First Block Address: %d\n", fileHeader.firstBlockAddress);
    printf("File Size: %d\n", fileHeader.fileSize);

    fseek(contactsFile, sizeof(FileHeader), SEEK_SET);

    Contact contact;
    int blockAddress = sizeof(FileHeader);
    int displacement = 0;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (contact.id[0] != '*')
        {
            printf("Block Address: %d, Displacement: %d\n", blockAddress, displacement);
        }
        displacement++;
        if (displacement == sizeof(Contact))
        {
            blockAddress += sizeof(Contact);
            displacement = 0;
        }
    }

    fclose(contactsFile);
}

void displayFileCharacteristics()
{
    contactsFile = fopen(FILE_NAME, "rb");
    if (contactsFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fread(&fileHeader, sizeof(FileHeader), 1, contactsFile);

    printf("File Characteristics:\n");
    printf("First Block Address: %d\n", fileHeader.firstBlockAddress);
    printf("File Size: %d\n", fileHeader.fileSize);

    int numContacts = fileHeader.numContacts;
    int numDeletedContacts = fileHeader.numDeletedContacts;
    Contact contact;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (contact.id[0] == '*')
        {
            numDeletedContacts++;
            numContacts--;
        }
    }

    printf("Number of Contacts: %d\n", numContacts);
    printf("Number of Inserted Contacts: %d\n", fileHeader.numInsertedContacts);
    printf("Number of Deleted Contacts: %d\n", numDeletedContacts);

    fclose(contactsFile);
}

void updateNumContacts()
{
    FILE *contactsFile = fopen(FILE_NAME, "rb+");
    if (contactsFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fseek(contactsFile, sizeof(FileHeader), SEEK_SET);

    Contact contact;
    int numContacts = 0;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (contact.id[0] != '*')
        {
            numContacts++;
        }
    }

    fseek(contactsFile, offsetof(FileHeader, numContacts), SEEK_SET);
    fwrite(&numContacts, sizeof(int), 1, contactsFile);

    fclose(contactsFile);
}

void createOrderedContactsFile()
{
    FILE *contactsFile = fopen("Contacts.bin", "rb");
    if (contactsFile == NULL)
    {
        printf("Error opening 'Contacts.bin' file.\n");
        return;
    }

    FileHeader fileHeader;
    fread(&fileHeader, sizeof(FileHeader), 1, contactsFile);

    Contact *contacts = (Contact *)malloc(fileHeader.numContacts * sizeof(Contact));
    if (contacts == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(contactsFile);
        return;
    }

    int numNonDeletedContacts = 0;
    Contact contact;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (contact.id[0] != '*')
        {
            contacts[numNonDeletedContacts++] = contact;
        }
    }

    fclose(contactsFile);

    qsort(contacts, numNonDeletedContacts, sizeof(Contact), compareContactsByID);

    FILE *orderedContactsFile = fopen("Contacts_Ordonnés.bin", "wb");
    if (orderedContactsFile == NULL)
    {
        printf("Error creating 'Contacts_Ordonnés.bin' file.\n");
        free(contacts);
        return;
    }

    fwrite(&fileHeader, sizeof(FileHeader), 1, orderedContactsFile);

    fwrite(contacts, sizeof(Contact), numNonDeletedContacts, orderedContactsFile);

    fclose(orderedContactsFile);
    free(contacts);
}

int compareContactsByID(const void *a, const void *b)
{
    const Contact *contactA = (const Contact *)a;
    const Contact *contactB = (const Contact *)b;
    return strcmp(contactA->id, contactB->id);
}

void printorderedContacts()
{
    createOrderedContactsFile();
    FILE *orderedContactsFile = fopen("Contacts_Ordonnés.bin", "rb");
    if (orderedContactsFile == NULL)
    {
        printf("Error opening 'Contacts_Ordonnés.bin' file.\n");
        return;
    }

    FileHeader fileHeader;
    fread(&fileHeader, sizeof(FileHeader), 1, orderedContactsFile);

    Contact contact;

    while (fread(&contact, sizeof(Contact), 1, orderedContactsFile) == 1)
    {
        if (contact.id[0] != '*')
        {
            printf("ID: %s\n", contact.id);
            printf("Name: %s\n", contact.name);
            printf("Phone: %s\n", contact.phone);
            printf("Email: %s\n", contact.email);
            printf("Observations: %s\n", contact.observations);
            printf("--------------------------------------\n");
        }
    }

    fclose(orderedContactsFile);
}

void printContacts()
{
    FILE *contactsFile = fopen("Contacts.bin", "rb");
    if (contactsFile == NULL)
    {
        printf("Error opening 'Contacts.bin' file.\n");
        return;
    }

    FileHeader fileHeader;
    fread(&fileHeader, sizeof(FileHeader), 1, contactsFile);

    Contact contact;

    while (fread(&contact, sizeof(Contact), 1, contactsFile) == 1)
    {
        if (contact.id[0] != '*')
        {
            printf("ID: %s\n", contact.id);
            printf("Name: %s\n", contact.name);
            printf("Phone: %s\n", contact.phone);
            printf("Email: %s\n", contact.email);
            printf("Observations: %s\n", contact.observations);
            printf("--------------------------------------\n");
        }
    }

    fclose(contactsFile);
}