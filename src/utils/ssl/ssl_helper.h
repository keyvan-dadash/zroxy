#ifndef SSL_HELPER_H
#define SSL_HELPER_H

#include <openssl/ssl.h>

SSL_CTX* ssl_init();

int ssl_load_certificates_and_private_keys(SSL_CTX *ctx, char *certificate, char *priv_key);

#endif /* SSL_HELPER_H */

