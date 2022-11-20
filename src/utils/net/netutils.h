/**
 * @file netutils.h
 */

#ifndef NET_UTILS_H
#define NET_UTILS_H

/**
 * Make a socket non-block.
 * \param sock_fd The FD of socket which is going to be non-block.
 */
void zxy_make_socket_nonblock(int sock_fd);

#endif /* NET_UTILS_H */

