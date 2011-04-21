/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * tcp.c: commandes en mode TCP
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"

char *versionTcp =
    "tcp.c : $Revision: 386 $ du $Date: 2011-04-21 09:31:07 +0200 (Thu, 21 Apr 2011) $ par $Author: rousseau $";

/* Creation d'une socket passive
 *=======================================================================
 *
 */

int TCP_passive(argc, argv)
int argc;
char *argv[];
{
	static char *socket_argv[] = { "socket", "tcp", 0 };
	static char *bind_argv[] = { "bind", ".", "*", "0", 0 };
	static char *listen_argv[] = { "listen", ".", "5", 0 };
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

	if (listen_call(3, listen_argv) == -1) {
		close_call(2, close_argv);
		return (-1);
	}

	printf("Socket TCP creee: id=%d, port=%d\n", sock[so], port);
	return (0);
}

/* Creation d'une socket active + connexion
 *========================================================================
 *
 */

int TCP_connect(argc, argv)
int argc;
char *argv[];
{
	static char *socket_argv[] = { "socket", "tcp", 0 };
	static char *bind_argv[] = { "bind", ".", "*", "0", 0 };
	static char *close_argv[] = { "close", ".", 0 };
	static char c_host[20];
	static char c_port[20];
	static char *connect_argv[] = { "connnect", ".", c_host, c_port, 0 };
	int so;
	int port;
	u_long addr;
	struct in_addr in_addr;

	if (nbsock == MAXSOCK) {
		printf("La table interne des sockets est pleine.\n");
		return (-1);
	}

/* host ? */
	if (argc > 1)
		get_host(argv[1], &addr);
	else
		get_host("", &addr);

	//in_addr.s_addr = ntohl(addr);
	in_addr.s_addr = addr;
	strlcpy(c_host, (char *)inet_ntoa(in_addr), sizeof(c_host));

/* port ? */
	if (argc > 2)
		get_port(argv[2], &port);
	else
		get_port("", &port);

	sprintf(c_port, "%d", port);

/* creation de la socket */
	so = socket_call(2, socket_argv);
	if (so == -1)
		return (-1);

/* affectation d'une adresse */
	port = bind_call(4, bind_argv);
	if (port == -1) {
		close_call(2, close_argv);
		return (-1);
	}

	printf("Socket TCP creee: id=%d, port=%d\n", sock[so], port);

/* connexion */
	if (connect_call(4, connect_argv) == -1) {
		close_call(2, close_argv);
		return (-1);
	}

	return (0);
}

/* Envoi de donnees urgentes
 *=========================================================================
 *
 */

int TCP_usend(argc, argv)
int argc;
char *argv[];
{
	static char *send_argv[] = { "send", 0, 0, 0 };

	argc = remove_flags(argc, argv);
	send_argv[1] = "-oob";
	if (argc > 1)
		send_argv[2] = argv[1];
	if (argc > 2)
		send_argv[3] = argv[2];
	// et on utilise la fonction send_call (prim.c)...
	return (send_call(argc + 1, send_argv));
}

/* Reception de donnees urgentes
 *==========================================================================
 *
 */

int TCP_urecv(argc, argv)
int argc;
char *argv[];
{
	static char *recv_argv[] = { "recv", 0, 0, 0 };

	argc = remove_flags(argc, argv);
	recv_argv[1] = "-oob";
	if (argc > 1)
		recv_argv[2] = argv[1];
	if (argc > 2)
		recv_argv[3] = argv[2];

	return (recv_call(argc + 1, recv_argv));
}
