#ifndef USER_IO_H
#define USER_IO_H

#include "../common.h"

/*  User File I/O  */
int load_users(User users[]);
void save_user(User *u);

/*  Book file I/O (read-only for users)  */
int load_books(Book books[]);

/*  Borrow file I/O  */
int load_borrow(Borrow borrows[]);
void save_borrow(Borrow *b);
void update_book_quantity(int book_id, int delta);
void update_borrow_status(int borrow_id, const char *return_date);

#endif
