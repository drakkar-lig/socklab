/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * options.h
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_OPTIONS_H_
#define	_OPTIONS_H_

int socket_options(int argc, char *argv[]);
int set_bool_opt(int so, int opt);
int set_int_opt(int so, int opt);
int set_async_opt(int so, int opt);
int set_nodelay_opt(int so, int opt);
int set_socket_opt(int so, int opt);

#endif                          /* _OPTIONS_H_ */
