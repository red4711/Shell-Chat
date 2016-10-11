#include <stdint.h>

#define USERNAME_MAXLEN 256
#define MESSAGE_MAXLEN 1024

struct chatroom_data{
	uint8_t username_len;
	char username[USERNAME_MAXLEN];
	uint8_t message_len;
	char message[MESSAGE_MAXLEN];
};

char *serialize_data(struct chatroom_data , char *);
int getdatasize(struct chatroom_data );
struct chatroom_data *deserialize_data(struct chatroom_data*, const char *);
