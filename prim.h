/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * prim.h
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_PRIM_H_
#define	_PRIM_H_
char *versionPrim;

int socket_call();
int socket6_call();

int msocket_call();
int msetmulticast();
int bind_call();

int listen_call();
int accept_call();
int connect_call();
int close_call();
int shutdown_call();
int write_call();
int send_call();
int sendto_call();
int read_call();
int recv_call();
int recvfrom_call();
int msendto_call();
int bsend_call();
int mrecvfrom_call();
int mjoin_call();
int mleave_call();
int mbind_call();
void rtioctl();

#endif                          /* _PRIM_H_ */
