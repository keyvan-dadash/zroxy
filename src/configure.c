


#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

#include "configure.h"
#include "logs.h"

connection_configs_t get_configs_from_file(char *filename)
{
    lua_State *L = lua_open();
    if (luaL_dofile(L, filename) != 0) {
        LOG_FATAL(-1, "Cannot load configs from given file(%s) error: %s", filename, lua_tostring(L, -1));
    }

    connection_configs_t configs;

    char *zroxy_port = strdup("zroxy_port");
    char *server_address = strdup("server_address");
    char *server_port = strdup("server_port");

    configs.server_port_char = get_string_option(L, zroxy_port);
    configs.backend_address = get_string_option(L, server_address);
    configs.backend_port_char = get_string_option(L, server_port);

    return configs;
}

char* get_string_option(lua_State *L, char *variable_name)
{
    lua_getglobal(L, variable_name);

    if (!lua_isstring(L, -1)) {
        LOG_FATAL(-1, "%s should be string", variable_name);
    }

    return (char *) lua_tostring(L, -1);
}

