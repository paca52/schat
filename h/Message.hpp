#define MAX_MESSAGE_LENGTH 1024
#define MAX_NAME_SIZE 32 
struct Message {
    char name[MAX_NAME_SIZE];
    char channel[MAX_NAME_SIZE];
    char message[MAX_MESSAGE_LENGTH];
};
