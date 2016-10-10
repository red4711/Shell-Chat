#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "chatroom_serialization.h"

#define USERNAME_MAXLEN 256
#define MESSAGE_MAXLEN 1024


char *serialize_data(struct chatroom_data data, char *buffer){
	int seeker = 0;
	memcpy(&buffer[seeker], &data.username_len, sizeof(data.username_len));
	seeker += sizeof(data.username_len);

	memcpy(&buffer[seeker], &data.username, data.username_len);
	seeker += data.username_len;

	memcpy(&buffer[seeker], &data.message_len, sizeof(data.message_len));
	seeker += sizeof(data.message_len);

	memcpy(&buffer[seeker], &data.message, data.message_len);
	seeker += data.message_len;

	return buffer;
}

int getdatasize(struct chatroom_data data){
	int size = 0;
	size += data.message_len;
	size += data.username_len;
	size += sizeof(data.message_len);
	size += sizeof(data.username_len);
	return size;
}

struct chatroom_data *deserialize_data(struct chatroom_data *data, const char *buffer){
	data->username_len = *buffer++;
	strncpy(data->username, buffer, data->username_len);
	data->username[data->username_len] = '\0';
	buffer += data->username_len;
	data->message_len = *buffer++;
	strncpy(data->message, buffer, data->message_len);
	data->message[data->message_len] = '\0';
	return data;
}
