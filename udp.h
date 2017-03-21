/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * udp.h
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_UDP_H_
#define _UDP_H_
char *versionUdp;

int UDP_socket();
int UDP_socket6();
int UDP_recvfrom();
int UDP_sendto();

#endif
