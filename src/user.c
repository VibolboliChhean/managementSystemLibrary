#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "io/user_io.h"

int do_register(){
    User users[MAX_USERS];
    int n = load_users(users);

    User u;
    printf("\n===============\n");
    printf("     Register    \n");
    printf("===============\n");
    printf("Full Name : "); scanf(" %99[^\n]", u.name);
    printf("Username  : "); scanf(" %49s", u.username);

    for(int i=0;i< n;i++){
        if(strcmp(users[i].username, u.username) == 0) {
            printf("Username already exists!\n");
            return 0;
        }
    }

    printf("Password  : "); scanf(" %49s", u.password);

    int max_id = 0;
    for(int i=0;i<n; i++)
        if(users[i].id >max_id) max_id = users[i].id;
    u.id = max_id + 1;

    getCurrentDate(u.registered_date);
    save_user(&u);
    printf("Registration successful! You can now log in.\n");
    return 1;
}

User *do_login(User users[], int *n_users){
    char uname[50], pw[50];
    printf("\n=== Login ===\n");
    printf("Username : "); scanf(" %49s", uname);
    printf("Password : "); scanf(" %49s", pw);

    *n_users = load_users(users);
    for(int i=0;i< *n_users; i++){
        if(strcmp(users[i].username, uname) == 0 && strcmp(users[i].password, pw) == 0){
            printf("Welcome, %s!\n", users[i].name);
            return &users[i];
        }
    }
    printf("Invalid username or password.\n");
    return NULL;
}

void searchBook(void){
    Book books[MAX_BOOKS];
    int n= load_books(books);
    char keyword[100];

    printf("\n=== Search Books ===\n");
    printf("Enter keyword (title or author): "); scanf(" %99[^\n]", keyword);

    char kwLower[100];
    for(int i=0; keyword[i]; i++)
        kwLower[i] = (char)tolower((unsigned char)keyword[i]);
    kwLower[strlen(keyword)] = '\0';

    printf("\n%-5s %-35s %-25s %-15s %-5s\n", "ID", "Title", "author", "Genre", "Qty");
    printf("%-5s %-35s %-25s %-15s %-5s\n", "===", "===================================", "=========================", "===============", "===");

    int found=0;
    for(int i=0; i<n;i++){
        char titleLower[100], authorLower[100];
        for(int j=0;books[i].title[j]; j++)
            titleLower[j] = (char)tolower((unsigned char)books[i].title[j]);
        titleLower[strlen(books[i].title)] = '\0';
        for(int j=0; books[i].author[j];j++)
            authorLower[j] = (char)tolower((unsigned char)books[i].author[j]);
        authorLower[strlen(books[i].author)] = '\0';

        if(strstr(titleLower, kwLower) || strstr(authorLower, kwLower)){
            printf("%-5d %-35s %-25s %-15s %-5d\n", books[i].id, books[i].title, books[i].author, books[i].genre, books[i].quantity);
            found++;
        }
    }
    if(!found) printf("No books found matching \"%s\".\n", keyword);
}

void borrowBook(User *user){
    Book books[MAX_BOOKS];
    int nb = load_books(books);
    Borrow borrows[MAX_BORROWS];
    int nr = load_borrow(borrows);

    printf("\n=== Borrow a Book ===\n");
    printf("\n%-5s %-35s %-25s %-15s %-5s\n", "ID", "Title", "author", "Genre", "Qty");
    printf("%-5s %-35s %-25s %-15s %-5s\n", "===", "===================================", "=========================", "===============", "===");

    for(int i=0;i< nb;i++){
        if(books[i].quantity >0)
            printf("%-5d %-35s %-25s %-15s %-5d\n", books[i].id, books[i].title, books[i].author, books[i].genre, books[i].quantity);
    }

    int book_id;
    printf("\nEnter Book ID to borrow (0 to cancel): ");
    scanf("%d", &book_id);
    if(book_id == 0) return;

    int bidx = -1;
    for(int i=0; i<nb; i++){
        if(books[i].id == book_id){bidx = i; break; }
    }
    if(bidx == -1) {printf("Book not found\n"); return;}
    if(books[bidx].quantity <= 0){ printf("Book is not available.\n"); return;}

    for(int i=0; i<nr; i++){
        if(borrows[i].user_id == user->id && borrows[i].book_id == book_id && strcmp(borrows[i].status, "borrowed") == 0){
            printf("You already borrowed this book and have not returned it yet.\n");
            return;
        }
    }

    int maxId =0;
    for(int i=0; i<nr; i++){
        if(borrows[i].id > maxId) maxId = borrows[i].id;
    }
    Borrow b;
    b.id = maxId +1;
    b.user_id = user->id;
    b.book_id = book_id;
    getCurrentDate(b.borrow_date);
    strcpy(b.return_date, "-");
    strcpy(b.status, "borrowed");
    save_borrow(&b);
    update_book_quantity(book_id, -1);
    printf("Successfully borrowed \"%s\".\n", books[bidx].title);
}

void returnBook(User *user){
    Borrow borrows[MAX_BORROWS];
    int nr = load_borrow(borrows);
    Book books[MAX_BOOKS];
    int nb = load_books(books);

    printf("\n=== Borrow History ===\n");
    printf("%-5s %-35s %-13s %-13s %-10s\n", "ID", "Title", "Borrow Date", "Return Date", "Status");
    printf("%-5s %-35s %-13s %-13s %-10s\n", "===", "===================================", "=============", "=============", "==========");

    int found = 0;
    for(int i=0; i<nr; i++){
        if(borrows[i].user_id == user->id && strcmp(borrows[i].status, "Borrowed") == 0){
            char title[100] = "Unknown";
            for(int j=0; j<nb; j++){
                if(books[j].id == borrows[i].book_id){
                    strcpy(title, books[j].title); break;
                }
            }
            printf("%-10d %-35s %-15s\n", borrows[i].id, title, borrows[i].borrow_date);
            found++;
        }
    }
    if(!found) { printf("You have no books to return.\n"); return;}

    int borrow_id;
    printf("\nEnter Borrow ID to return (0 to cancel): ");
    scanf("%d", &borrow_id);
    if(borrow_id == 0) return;

    int bidx = -1;
    for(int i=0;i<nr; i++){
        if(borrows[i].id == borrow_id && borrows[i].user_id == user->id && strcmp(borrows[i].status, "borrowed") == 0){
            bidx = i; break;
        }
    }
    if(bidx == -1){ printf("Invalid Borrow ID.\n"); return;}

    char today[20];
    getCurrentDate(today);
    update_borrow_status(borrow_id, today);
    update_book_quantity(borrows[bidx].book_id, +1);
    printf("Book returned successfully.\n");
}

void borrowHistory(User *user){
    Borrow borrows[MAX_BORROWS];
    int nr = load_borrow(borrows);
    Book books[MAX_BOOKS];
    int nb = load_books(books);

    printf("\n=== Borrow History ===\n");
    printf("%-5s %-35s %-13s %-13s %-10s\n", "ID", "Title", "Borrow Date", "Return Date", "Status");
    printf("%-5s %-35s %-13s %-13s %-10s\n", "===", "===================================", "=============", "=============", "==========");

    int found = 0;
    for(int i=0; i<nr; i++){
        if(borrows[i].user_id == user->id){
            char title[100] = "Unknown";
            for(int j=0; j<nb; j++){
                if(books[j].id == borrows[i].book_id){
                    strcpy(title, books[j].title);  break;
                }
            }
            printf("%-5d %-35s %-13s %-13s %-10s\n", borrows[i].id, title, borrows[i].borrow_date, borrows[i].return_date, borrows[i].status);
            found++;
        }
    }
    if(!found) printf("No borrow history found.\n");
}

void user_menu(User *user){
    int choice;
    do{
        clearScreen();
        printf("=============================================\n");
        printf("    Library Management System - User Menu    \n");
        printf("    Logged in as:   %s\n", user->name);
        printf("=============================================\n");
        printf("|  [1]. Search Books                        |\n");
        printf("|  [2]. Borrow a Book                       |\n");
        printf("|  [3]. Return a Book                       |\n");
        printf("|  [4]. Borrow History                      |\n");
        printf("|  [0]. Logout                              |\n");
        printf("=============================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice){
            case 1: searchBook(); pauseScreen(); break;
            case 2: borrowBook(user); pauseScreen(); break;
            case 3: returnBook(user); pauseScreen(); break;
            case 4: borrowHistory(user); pauseScreen(); break;
            case 0: printf("Logged out.\n"); break;
            default: printf("Invalid choice.\n"); pauseScreen();
        }
    }while(choice!=0);
}

int main(){
    int choice;
    do{
        clearScreen();
        printf("============================================\n");
        printf("|        Library Management System         |\n");
        printf("============================================\n");
        printf("|   [1].  Register                         |\n");
        printf("|   [2].  Login                            |\n");
        printf("|   [0].  Exit                             |\n");
        printf("============================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            do_register();
            pauseScreen();
        } else if (choice == 2) {
            User users[MAX_USERS];
            int n;
            User *logged = do_login(users, &n);
            if (logged) {
                pauseScreen();
                user_menu(logged);
            } else {
                pauseScreen();
            }
        }
    }while(choice != 0);

    printf("Good bye!\n");
    return 0;
}
