#include <stdio.h>
#include <stdlib.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include "defines.h"
#include "utils/ssl/ssl_helper.h"
#include "logging/logs.h"

SSL_CTX* ssl_init()
{
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    // const SSL_METHOD *method = TLS_server_method();
    // SSL_CTX *ctx = SSL_CTX_new(method);

    ctx = SSL_CTX_new(SSLv23_server_method());

    // if (!SSL_CTX_set_cipher_list(ctx, SSL_CIPHER)) {
    //     LOG_FATAL(-1, "Cannot set cipher %s on ctx\n", SSL_CIPHER);
    // }

    // if (!SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION)) {
    //     LOG_FATAL(-1, "cannot disable compression\n");
    // }

    return ctx;

}

int ssl_load_certificates_and_private_keys(SSL_CTX *ctx, char *certificate_location, char *priv_key_location)
{
    // if (SSL_CTX_load_verify_locations(ctx, certificate_location, 0) <= 0) {
    //     ERR_print_errors_fp(stderr);
    //     exit(-1);
    // }

    if (SSL_CTX_use_certificate_file(ctx, certificate_location, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, priv_key_location, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        LOG_FATAL(-1, "Private key and public key does not match in certificate\n");
    }

    SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);


    // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT |
    // SSL_VERIFY_CLIENT_ONCE, 0);

    return 0;
}

