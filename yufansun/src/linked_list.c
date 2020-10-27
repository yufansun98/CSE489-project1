#include <sys/queue.h> //only on BSD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
*   This a quick guide showing how to use the generic linked list
*   macros instead of making your own linked list. 
*   
*   The code below is based off of here: http://infnis.wikidot.com/list-from-sys-queue-h
*   The man page for this library is found here: https://linux.die.net/man/3/queue
*/

int main() {

    struct Authors {

        char * name;
        uint32_t id;

        // Similar to struct Authors * next_author;
        LIST_ENTRY(Authors) next_author;

        //maybe you'd want a nested list?
        struct Books {
            char * title;
            LIST_ENTRY(Books) next_book;
        }_warningsBeGone; //Not necessary. This eliminates a compiler warning to avoid confusion

        //book_list is the list of books for each instance of struct Author
        LIST_HEAD(__book_list, Books) book_list; 
    };

    /*  
       __authors_list is the HEADNAME, but you should use authors_list whenever you want to access/modify the list.
       In all of the code I've written, I've never needed to refer to the HEADNAME after declaring it in LIST_HEAD
    */
    LIST_HEAD(__authors_list, Authors) authors_list;
    LIST_INIT(&authors_list);

    //Shorthand notation for declaring and populating struct info
    struct Authors s1 = {"Orwell",      1};
    struct Authors s2 = {"Kafka",       2};
    struct Authors s3 = {"Dostoevsky",  3};
    struct Authors s4 = {"Faulkner",    4};

    /*  Insert structs into authors_list */
    LIST_INSERT_HEAD(&authors_list, &s1, next_author);
    LIST_INSERT_HEAD(&authors_list, &s2, next_author);
    LIST_INSERT_HEAD(&authors_list, &s3, next_author);
    LIST_INSERT_HEAD(&authors_list, &s4, next_author);

    /* Loop through authors_list */
    struct Authors * tmp;
    puts("\nLooping through authors_list");
    LIST_FOREACH(tmp, &authors_list, next_author) {
        printf("Name:%s\t\tID:%d\n", tmp->name, (int)tmp->id);
    }

    puts("/***********************************************************/\n");

    /* Loop through authors_list to initialize and populate the nested lists */
    LIST_FOREACH(tmp, &authors_list, next_author) {
        LIST_INIT(&tmp->book_list);

        switch (tmp->id) {
            case 1: {
                  struct Books b1 = {"1984"};
                  struct Books b2 = {"Animal Farm"};
                  struct Books b3 = {"Coming Up For Air"};

                  LIST_INSERT_HEAD(&tmp->book_list, &b1, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b2, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b3, next_book);
                  break;
            }
            case 2: {
                  struct Books b1 = {"The Trial"};
                  struct Books b2 = {"The Castle"};
                  struct Books b3 = {"The Metamorphosis"};

                  LIST_INSERT_HEAD(&tmp->book_list, &b1, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b2, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b3, next_book);
                  break;
            }
            case 3: {
                  struct Books b1 = {"Crime & Punishment"};
                  struct Books b2 = {"The Brothers Karamazov"};
                  struct Books b3 = {"The Idiot"};

                  LIST_INSERT_HEAD(&tmp->book_list, &b1, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b2, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b3, next_book);
                  break;
            }
            case 4: {
                  struct Books b1 = {"The Sound and the Fury"};
                  struct Books b2 = {"The Reivers"};
                  struct Books b3 = {"As I Lay Dying"};

                  LIST_INSERT_HEAD(&tmp->book_list, &b1, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b2, next_book);
                  LIST_INSERT_HEAD(&tmp->book_list, &b3, next_book);
                  break;
            }
        } //End of switch
    } // End of loop

    /* Loop through authors_list and print books*/
    struct Books * tmp_book;
    puts("Printing every author and their books\n");
    LIST_FOREACH(tmp, &authors_list, next_author) {
        printf("Name:%s\n", tmp->name);
        LIST_FOREACH(tmp_book, &tmp->book_list, next_book) {
            printf("%s\t", tmp_book->title);
        }
        printf("\n\n");
    }
    puts("/***********************************************************/\n");

    /* Loop through list and delete some elements from the Authors_list */
    puts("Remove all Authors with an id <= 2");
    LIST_FOREACH(tmp, &authors_list, next_author) {
        if (tmp->id <= 2)
            LIST_REMOVE(tmp, next_author);
        else  printf("Name:%s\t\tSome Unique Number:%d\n", tmp->name, (int)tmp->id);
    }
    puts("/***********************************************************/\n");
}