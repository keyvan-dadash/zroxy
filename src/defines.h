

#include <openssl/ssl.h>

#ifndef DEFINES_H
#define DEFINES_H

SSL_CTX *ctx;

#define READ_BUF_SIZE 4096

#define MAX_LISTEN_SIZE 10

#define MAX_EVENTS 10

#define SSL_CIPHER "AES128-GCM-SHA256"


#define READ_EVENT 43989759
#define WRITE_EVENT 43989760
#define CLOSE_EVENT 43989761


#define CLIENT_SOCK 101
#define BACKEND_SOCK 102
#define PROXY_TIMER_SOCK 103
#define FREED_SOCK 104

#endif /* DEFINES_H */