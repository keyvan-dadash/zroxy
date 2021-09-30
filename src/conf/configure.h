


#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>


typedef struct
{
    char *server_port_char;
} zxy_server_configs_t;


typedef struct
{
    char *proxy_address;
    char *proxy_port;
} zxy_proxy_configs_t;

typedef struct 
{
    char *certificate_path;
    char *private_key_path;
} zxy_certificates_configs_t;


zxy_server_configs_t zxy_get_server_configs_from_file(char *filename);

zxy_proxy_configs_t zxy_get_proxy_configs_from_file(char *filename);

zxy_certificates_configs_t zxy_get_certificates_configs_from_file(char *filename);

char* get_string_option(lua_State *L, char *variable_name);



#endif /* CONFIGURE_H */
