/**
 * @file configure.h
 */
#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

/**
 * zxy_server_configs_t is responsible for holding server configs.
 */
typedef struct
{
  /**
   * Server port
   */
  char *server_port;
} zxy_server_configs_t;

/**
 * zxy_proxy_configs_t is responsible for holding proxy configs.
 */
typedef struct
{
  /**
   * The address of proxy that zroxy going to connect.
   */
  char *proxy_address;

  /**
   * The port of proxy server that zroxy going to connect.
   */
  char *proxy_port;
} zxy_proxy_configs_t;

/**
 * zxy_certificates_configs_t is responsible for holding certificate configs
 */
typedef struct 
{
  /**
   * The path of certificate that zroxy going to use for ssl mode.
   */
  char *certificate_path;

  /**
   * The path of private key that zroxy going to use in order to decrypt ssl connections.
   */
  char *private_key_path;
} zxy_certificates_configs_t;

/**
 * Get server configs from path to server configs.
 *
 * \param filename The path to the server configs.
 * \return zxy_server_configs_t Extracted server configs from filename.
 */
zxy_server_configs_t zxy_get_server_configs_from_file(char *filename);

/**
 * Get proxy configs from path to proxy configs.
 *
 * \param filename The path to the proxy configs.
 * \return zxy_proxy_configs_t Extracted proxy configs from filename.
 */
zxy_proxy_configs_t zxy_get_proxy_configs_from_file(char *filename);

/**
 * Get certificates config from path to certificate configs.
 *
 * \param filename The path to the the certificates configs.
 * \return zxy_certificates_configs_t Extracted certificate configs from filename.
 */
zxy_certificates_configs_t zxy_get_certificates_configs_from_file(char *filename);

/**
 * Retrun string value from the lua state.
 *
 * \param L The state of lua file.
 * \param variable_name The name of the variable going to extract.
 */
char* get_string_option(lua_State *L, char *variable_name);

#endif /* CONFIGURE_H */

