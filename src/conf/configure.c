#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

#include "conf/configure.h"
#include "logging/logs.h"

zxy_server_configs_t zxy_get_server_configs_from_file(char *filename)
{
    lua_State *L = lua_open();
    if (luaL_dofile(L, filename) != 0) {
        LOG_FATAL(-1, "Cannot load configs from given file(%s) error: %s", filename, lua_tostring(L, -1));
    }

    zxy_server_configs_t  server_configs;

    char *zroxy_port = strdup("zroxy_port");

    server_configs.server_port = get_string_option(L, zroxy_port);

    free(zroxy_port);
    
    return server_configs;
}


zxy_proxy_configs_t zxy_get_proxy_configs_from_file(char *filename)
{
    lua_State *L = lua_open();
    if (luaL_dofile(L, filename) != 0) {
        LOG_FATAL(-1, "Cannot load configs from given file(%s) error: %s", filename, lua_tostring(L, -1));
    }

    zxy_proxy_configs_t proxy_configs;

    char *server_address = strdup("server_address");    
    char *server_port = strdup("server_port");

    proxy_configs.proxy_address = get_string_option(L, server_address);
    proxy_configs.proxy_port = get_string_option(L, server_port);

    free(server_address);
    free(server_port);
    
    return proxy_configs;    
}

zxy_certificates_configs_t zxy_get_certificates_configs_from_file(char *filename)
{
    lua_State *L = lua_open();
    if (luaL_dofile(L, filename) != 0) {
        LOG_FATAL(-1, "Cannot load configs from given file(%s) error: %s", filename, lua_tostring(L, -1));
    }

    zxy_certificates_configs_t certificates_configs;

    char *certificate_path = strdup("certificate_path");
    char *privkey_path = strdup("privkey_path");

    certificates_configs.certificate_path = get_string_option(L, certificate_path);
    certificates_configs.private_key_path = get_string_option(L, privkey_path);

    free(certificate_path);
    free(privkey_path);
    
    return certificates_configs;
}


char* get_string_option(lua_State *L, char *variable_name)
{
    lua_getglobal(L, variable_name);

    if (!lua_isstring(L, -1)) {
        LOG_FATAL(-1, "%s should be string", variable_name);
    }

    return (char *) lua_tostring(L, -1);
}

