/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * tcp.h
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_TCP_H_
#define	_TCP_H_

int TCP_passive(int argc, char *argv[]);
int TCP_passive6(int argc, char *argv[]);
int TCP_connect(int argc, char *argv[]);
int TCP_connect6(int argc, char *argv[]);
int TCP_usend(int argc, char *argv[]);
int TCP_urecv(int argc, char *argv[]);

#endif
