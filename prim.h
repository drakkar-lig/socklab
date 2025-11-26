/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * prim.h
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_PRIM_H_
#define	_PRIM_H_

int socket_call(int argc, char *argv[]);
int socket6_call(int argc, char *argv[]);
int bind_call(int argc, char *argv[]);
int listen_call(int argc, char *argv[]);
int accept_call(int argc, char *argv[]);
int connect_call(int argc, char *argv[]);
int close_call(int argc, char *argv[]);
int shutdown_call(int argc, char *argv[]);
int write_call(int argc, char *argv[]);
int send_call(int argc, char *argv[]);
int sendto_call(int argc, char *argv[]);
int read_call(int argc, char *argv[]);
int recv_call(int argc, char *argv[]);
int recvfrom_call(int argc, char *argv[]);
int msocket_call(int argc, char *argv[]);
int mbind_call(int argc, char *argv[]);
int mjoin_call(int argc, char *argv[]);
int mleave_call(int argc, char *argv[]);
int msendto_call(int argc, char *argv[]);
int mrecvfrom_call(int argc, char *argv[]);
int bsend_call(int argc, char *argv[]);

#endif                          /* _PRIM_H_ */
