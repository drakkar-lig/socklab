/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * options.c: controle des options des sockets creees (commande options)
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"

/* liste des options disponibles */

t_sockopt sockopt[] = {
    {"broadcast", SO_BROADCAST, BOOL_OPT}
    ,
    {"debug", SO_DEBUG, BOOL_OPT}
    ,
    {"dontroute", SO_DONTROUTE, BOOL_OPT}
    ,
    {"keepalive", SO_KEEPALIVE, BOOL_OPT}
    ,
    {"oobinline", SO_OOBINLINE, BOOL_OPT}
    ,
    {"reuseaddr", SO_REUSEADDR, BOOL_OPT}
    ,
    /*      { "useloopback", SO_USELOOPBACK, BOOL_OPT       }, */
    {"asynchrone", 0, ASYNC_OPT}
    ,
    {"nodelay", 0, NODELAY_OPT}
    ,
    {"rcvbuf", SO_RCVBUF, INT_OPT}
    ,
    {"sndbuf", SO_SNDBUF, INT_OPT}
    ,
    {"rcvtimeo", SO_RCVTIMEO, TIME_OPT}
    ,
    {"sndtimeo", SO_SNDTIMEO, TIME_OPT}
    ,
    /*{ "bsdcompat",    SO_BSDCOMPAT,   BOOL_OPT        }, */
    {"", 0, 0}
};

/* Modification des options d'une socket (getsockopt() / setsockopt() )
 *==========================================================================
 *
 */

int socket_options(argc, argv)
int argc;
char *argv[];
{
    int so;
    int opt;
    int nb;
    int i;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* Options ? */
    if (argc > 2) {
        for (i = 2; i < argc; i++) {
            nb = 0;
            for (opt = 0; strcmp(sockopt[opt].name, ""); opt++) {
                if (!strncmp(argv[i], sockopt[opt].name, strlen(argv[i]))) {
                    nb++;
                    set_socket_opt(so, opt);
                }
            }

            if (nb == 0)
                printf("*** unknown option %s\n", argv[i]);
        }

        return (0);
    } else {
        for (opt = 0; strcmp(sockopt[opt].name, ""); opt++)
            set_socket_opt(so, opt);

        return (0);
    }
}

/* Modification d'une option booleenne
 *==========================================================================
 */

int set_bool_opt(so, opt)
int so;
int opt;
{
    socklen_t optlen;
    int optval;
    char str[100];

    optlen = sizeof(int);
    if (getsockopt(sock[so], SOL_SOCKET, sockopt[opt].code, (char *)&optval, &optlen) < 0) {
        sprintf(str, "getsockopt()-%s", sockopt[opt].name);
        ERREUR(str);
        return (-1);
    }

    get_bool(sockopt[opt].name, &optval);

    optlen = sizeof(int);
    if (setsockopt(sock[so], SOL_SOCKET, sockopt[opt].code, (char *)&optval, optlen) < 0) {
        sprintf(str, "setsockopt()-%s", sockopt[opt].name);
        ERREUR(str);
        return (-1);
    }

    return (0);
}

/* Modification d'une option entiere
 *==========================================================================
 */

int set_int_opt(so, opt)
int so;
int opt;
{
    socklen_t optlen;
    int optval;
    char str[100];

    optlen = sizeof(int);
    if (getsockopt(sock[so], SOL_SOCKET, sockopt[opt].code, (char *)&optval, &optlen) < 0) {
        sprintf(str, "getsockopt()-%s", sockopt[opt].name);
        ERREUR(str);
        return (-1);
    }

    get_nb(sockopt[opt].name, "", &optval);

    optlen = sizeof(int);
    if (setsockopt(sock[so], SOL_SOCKET, sockopt[opt].code, (char *)&optval, optlen) < 0) {
        sprintf(str, "setsockopt()-%s", sockopt[opt].name);
        ERREUR(str);
        return (-1);
    }

    return (0);
}

/* Modification de l'option "asynchrone"
 *==========================================================================
 */

int set_async_opt(so, opt)
int so;
int opt;
{
    int flg;
    int optval;

    flg = fcntl(sock[so], F_GETFL, 0);
    if (flg == -1) {
        ERREUR("fcntl()-F_GETFL");
        return (-1);
    }

    optval = flg & FASYNC;
    flg &= ~FASYNC;
    get_bool(sockopt[opt].name, &optval);

    if (fcntl(sock[so], F_SETFL, flg | FASYNC * optval) == -1) {
        ERREUR("fcntl()-F_SETFL");
        return (-1);
    }

    return (0);
}

/* Modification de l'option "no delay"
 *==========================================================================
 */

int set_nodelay_opt(so, opt)
int so;
int opt;
{
    int flg;
    int optval;

    flg = fcntl(sock[so], F_GETFL, 0);
    if (flg == -1) {
        ERREUR("fcntl()-F_GETFL");
        return (-1);
    }

    optval = flg & FNDELAY;
    flg &= ~FNDELAY;
    get_bool(sockopt[opt].name, &optval);

    if (fcntl(sock[so], F_SETFL, flg | FNDELAY * optval) == -1) {
        ERREUR("fcntl()-F_SETFL");
        return (-1);
    }

    return (0);
}

/* Modification d'une option time
 *==========================================================================
 */

int set_time_opt(so, opt)
int so;
int opt;
{
    socklen_t optlen;
    struct timeval optval;
    char str[100];

    optlen = sizeof(optval);
    if (getsockopt(sock[so], SOL_SOCKET, sockopt[opt].code, (char *)&optval, &optlen) < 0) {
        sprintf(str, "getsockopt()-%s", sockopt[opt].name);
        ERREUR(str);
        return (-1);
    }

    get_nb(sockopt[opt].name, "", &(optval.tv_sec));
    optval.tv_usec = 0;

    optlen = sizeof(optval);
    if (setsockopt(sock[so], SOL_SOCKET, sockopt[opt].code, (char *)&optval, optlen) < 0) {
        sprintf(str, "setsockopt()-%s", sockopt[opt].name);
        ERREUR(str);
        return (-1);
    }

    return (0);
}

/* Modification d'une option d'une socket
 *==========================================================================
 */

int set_socket_opt(so, opt)
int so;
int opt;
{
    switch (sockopt[opt].type) {
    case BOOL_OPT:
        set_bool_opt(so, opt);
        break;

    case INT_OPT:
        set_int_opt(so, opt);
        break;

    case ASYNC_OPT:
        set_async_opt(so, opt);
        break;

    case NODELAY_OPT:
        set_nodelay_opt(so, opt);
        break;

    case TIME_OPT:
        set_time_opt(so, opt);
        break;
    }
    return 0;
}
