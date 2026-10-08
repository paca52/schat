#ifndef MESSAGE
#define MESSAGE

#define MAX_MESSAGE_LENGTH 1024
#define MAX_NAME_SIZE 32 
struct Message {
    char name[MAX_NAME_SIZE];
    char channel[MAX_NAME_SIZE];
    char text[MAX_MESSAGE_LENGTH];
};

#endif
