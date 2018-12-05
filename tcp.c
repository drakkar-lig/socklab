/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * tcp.c: commandes en mode TCP
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"

/* Creation d'une socket passive en ipv4
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

    printf("TCP socket created: id=%d, port=%d\n", sock[so], port);
    return (0);
}

/* Creation d'une socket passive en ipv6
 *=======================================================================
 *
 */

int TCP_passive6(argc, argv)
int argc;
char *argv[];
{
    static char *socket_argv[] = { "socket6", "tcp", 0 };
    static char *bind_argv[] = { "bind", ".", "*", "0", 0 };
    static char *listen_argv[] = { "listen", ".", "5", 0 };
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

    if (listen_call(3, listen_argv) == -1) {
        close_call(2, close_argv);
        return (-1);
    }

    printf("TCP socket created for IPv6: id=%d, port=%d\n", sock[so], port);
    return (0);
}

/* Creation d'une socket ipv4 active + connexion
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
    static char c_host[NI_MAXHOST];
    static char c_port[NI_MAXSERV];
    static char *connect_argv[] = { "connnect", ".", c_host, c_port, 0 };
    int so;
    int port;
    struct sockaddr_in addr;

    if (nbsock == MAXSOCK) {
        printf("Too many sockets already in use.\n");
        return (-1);
    }
    /* host ? */
    if (argc > 1)
        get_host(argv[1], (struct sockaddr *)&addr, 4);
    else
        get_host("", (struct sockaddr *)&addr, 4);

    inet_ntop(addr.sin_family, &(addr.sin_addr), c_host, sizeof(c_host));

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

    printf("TCP socket created: id=%d, port=%d\n", sock[so], port);

    /* connexion */
    if (connect_call(4, connect_argv) == -1) {
        close_call(2, close_argv);
        return (-1);
    }

    return (0);
}

/* Creation d'une socket ipv6 active + connexion
 *========================================================================
 *
 */

int TCP_connect6(argc, argv)
int argc;
char *argv[];
{
    static char *socket_argv[] = { "socket6", "tcp", 0 };
    static char *bind_argv[] = { "bind", ".", "*", "0", 0 };
    static char *close_argv[] = { "close", ".", 0 };
    static char c_host[NI_MAXHOST];
    static char c_port[NI_MAXSERV];
    static char *connect_argv[] = { "connect", ".", c_host, c_port, 0 };
    int so;
    int port;
    struct sockaddr_in6 addr;

    if (nbsock == MAXSOCK) {
        printf("Too many sockets already in use.\n");
        return (-1);
    }
    /* host ? */
    if (argc > 1)
        get_host(argv[1], (struct sockaddr *)&addr, 6);
    else
        get_host("", (struct sockaddr *)&addr, 6);

    inet_ntop(addr.sin6_family, &(addr.sin6_addr), c_host, sizeof(c_host));

    /* port ? */
    if (argc > 2)
        get_port(argv[2], &port);
    else
        get_port("", &port);

    sprintf(c_port, "%d", port);

    /* creation de la socket */
    so = socket6_call(2, socket_argv);
    if (so == -1)
        return (-1);

    /* affectation d'une adresse */
    port = bind_call(4, bind_argv);
    if (port == -1) {
        close_call(2, close_argv);
        return (-1);
    }

    printf("TCP socket created for IPv6: id=%d, port=%d\n", sock[so], port);

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
