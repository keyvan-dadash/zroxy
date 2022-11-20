#ifndef SSL_HELPER_H
#define SSL_HELPER_H

#include <openssl/ssl.h>

/**
 * Holds the contexts of the ssl that need in the zroxy.
 */
struct contexts
{
  /**
   * Client SSL context.
   */
  SSL_CTX* client_ctx;

  /**
   * Server SSL context.
   */
  SSL_CTX* server_ctx;
};

/**
 * Intialize the OPENSSL.
 * \return SSL Context.
 */
struct contexts ssl_init();

/**
 * Load certicate and private key, and also intialize SSL context.
 * \param ctx SSL Context.
 * \param certicate Path to the certicate file.
 * \param priv_key Path to the private key file.
 * \return Return 0 on the success.
 */
int ssl_load_certificates_and_private_keys(SSL_CTX *ctx, char *certificate, char *priv_key);

#endif /* SSL_HELPER_H */

