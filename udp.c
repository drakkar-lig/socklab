/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * udp.c: commandes en mode UDP
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"

char *versionUdp =
    "udp.c : $Revision: 386 $ du $Date: 2011-04-21 09:31:07 +0200 (Thu, 21 Apr 2011) $ par $Author: rousseau $\n";

/* Creation d'une socket udp en ipv4
 *=======================================================================
 *
 */

int UDP_socket(argc, argv)
int argc;
char *argv[];
{
	static char *socket_argv[] = { "socket", "udp", 0 };
	static char *bind_argv[] = { "bind", ".", "*", "0", 0 };
	static char *close_argv[] = { "close", ".", 0 };
	int so;
	int port;

	so = socket_call(2, socket_argv);
	if (so == -1)
		return (-1);

	port = bind_call(4, bind_argv);
	if (port == -1) {
		close_call(2, close_argv);
		return (-1);
	}

	printf("Socket UDP creee: Id=%d, port=%d\n", sock[so], port);
	return (0);
}

/* Creation d'une socket udp en ipv4
 *=======================================================================
 *
 */

int UDP_socket6(argc, argv)
int argc;
char *argv[];
{
    static char *socket_argv[] = { "socket6", "udp", 0 };
    static char *bind_argv[] = { "bind", ".", "*", "0", 0 };
    static char *close_argv[] = { "close", ".", 0 };
    int so;
    int port;
    
    so = socket6_call(2, socket_argv);
    if (so == -1)
        return (-1);
    
    port = bind_call(4, bind_argv);
    if (port == -1) {
        close_call(2, close_argv);
        return (-1);
    }
    
    printf("Socket (IPV6) UDP creee: Id=%d, port=%d\n", sock[so], port);
    return (0);
}


/* reception d'un message
 *=======================================================================
 *
 */

int UDP_recvfrom(argc, argv)
int argc;
char *argv[];
{
	argc = remove_flags(argc, argv);

	return (recvfrom_call(argc, argv));
}

/* envoi d'un message
 *=======================================================================
 *
 */

int UDP_sendto(argc, argv)
int argc;
char *argv[];
{
	argc = remove_flags(argc, argv);

	return (sendto_call(argc, argv));
}
