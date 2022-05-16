#ifndef SSL_HELPER_H
#define SSL_HELPER_H

#include <openssl/ssl.h>

struct contexts
{
  SSL_CTX* client_ctx;
  SSL_CTX* server_ctx;
};

struct contexts ssl_init();

int ssl_load_certificates_and_private_keys(SSL_CTX *ctx, char *certificate, char *priv_key);

#endif /* SSL_HELPER_H */

