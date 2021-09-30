


#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>


typedef struct
{
    char *server_port_char;
    char *backend_address;
    char *backend_port_char;
    char *certificate_path;
    char *private_key_path;
} connection_configs_t;


connection_configs_t get_configs_from_file(char *filename);

char* get_string_option(lua_State *L, char *variable_name);



#endif /* CONFIGURE_H */
