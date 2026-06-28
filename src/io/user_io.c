#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "user_io.h"

void clearScreen(void){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen(void){
    printf("\nPress Entter to continue...");
    int c;
    while(( c == getchar()) != '\n' && c != EOF);
    getchar();
}

void getCurrentDate(char *buf){
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, 20, "%Y-%m-%d", tm_info); 
}

int load_users(User users[]){
    FILE *f = fopen(USERS_FILE, "r");
    if(!f) return 0;
    int n=0;
    while(n < MAX_USERS &&
          fscanf(f, "%d|%49[^|]|%49[^|]|%99[^|]|%19[^\n]\n",
                  &users[n].id,
                  users[n].username,
                  users[n].password,
                  users[n].name,
                  users[n].registered_date) == 5) {
        n++;
    }
    fclose(f);
    return n;
}

int load_borrows(Borrow borrows[]){
    FILE *f = fopen(BORROWS_FILE, "r");
    if(!f) return 0;
    int n=0;
    while (n < MAX_BORROWS &&
           fscanf(f, "%d|%d|%d|%19[^|]|%19[^|]|%9[^\n]\n",
                  &borrows[n].id,
                  &borrows[n].user_id,
                  &borrows[n].book_id,
                  borrows[n].borrow_date,
                  borrows[n].return_date,
                  borrows[n].status) == 6) {
        n++;
    }
    fclose(f);
    return n;
}

int load_books(Book books[]){
    FILE *f = fopen(BOOKS_FILE, "r");
    if(!f) return 0;
    int n=0;
    while(n< MAX_BOOKS &&
          fscanf(f, "%d|%99[^|]|%99[^|]|%49[^|]|%d\n",
                 &books[n].id, books[n].title, books[n].author, books[n].genre, &books[n].quantity) == 5){
        n++;
    }
    fclose(f);
    return n;
}

void save_borrow(Borrow *b){
    FILE *f = fopen(BORROWS_FILE, "a");
    if (!f) { printf("Error: cannot open %s\n", BORROWS_FILE); return;}
    fprintf(f, "%d|%d|%d|%s|%s|%s\n",
            b->id, b->user_id, b->book_id, b->borrow_date, b->return_date, b->status);
    fclose(f);
}

void update_book_quantity(int book_id, int delta){
    Book books[MAX_BOOKS];
    int n = load_books(books);
    for(int i=0;i<n;i++){
        if(books[i].id == book_id){
            books[i].quantity += delta;
            break;
        }
    }
    FILE *f = fopen(BOOKS_FILE, "w");
    if(!f) return;
    for(int i=0;i<n;i++){
        fprintf(f, "%d|%s|%s|%s|%d\n", 
                books[i].id, books[i].title, books[i].author, books[i].genre, books[i].quantity);
    }
    fclose(f);
}

void update_borrow_status(int borrows_id, const char *retun_date){
    Borrow borrows[MAX_BORROWS];
    int n = load_borrow(borrows);
    for(int i=0;i<n;i++){
        if(borrows[i].id == borrows_id){
            strcpy(borrows[i].return_date, retun_date);
            strcpy(borrows[i].status, "returned");
            break;
        }
    }
    FILE *f = fopen(BORROWS_FILE, "w");
    for(int i=0;i<n;i++){
        fprintf(f, "%d|%d|%d|%s|%s|%s\n", 
                borrows[i].id, borrows[i].user_id, borrows[i].book_id, borrows[i].borrow_date, borrows[i].return_date, borrows[i].status);
    }
    fclose(f);
}
