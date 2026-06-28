#ifndef COMMON_H
#define COMMON_H

/*  Limits  */
#define MAX_USERS 100
#define MAX_BOOKS 100
#define MAX_BORROWS 200

/*  File paths  */
#define USERS_FILE "../data/user.txt"
#define BOOKS_FILE "../data/book.txt"
#define BORROWS_FILE "../data/borrow.txt"

/*  Structs  */
typedef struct {
    int id;
    char username[50];
    char password[50];
    char name[100];
    char registered_date[20];
} User;

typedef struct {
    int id;
    char title[100];
    char author[100];
    char genre[50];
    int quantity;
} Book;

typedef struct {
    int id;
    int user_id;
    int book_id;
    char borrow_date[20];
    char return_date[20];
    char status[10];
} Borrow;

/*  Shared Helper  */
void clearScreen(void);
void pauseScreen(void);
void getCurrentDate(char *buf);

#endif;
