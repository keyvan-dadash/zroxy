


#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>



#include "utils/net/netutils.h"
#include "logging/logs.h"



void zxy_make_socket_nonblock(int sock_fd)
{
    int flags;
    flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags == -1) {
        LOG_ERROR("Failed to get flags of socket(%d)", sock_fd);
        return;
    }

    flags |= O_NONBLOCK;
    
    if (fcntl(sock_fd, F_SETFL, flags) == -1) {
        LOG_ERROR("Failed to set flag O_NONBLOCK on socket(%d)", sock_fd);
    }
}