/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * prim.c: commandes associees a chaque primitive des sockets
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"
#include <stdlib.h>

/* Toutes les primitives n'ont que deux parametres: argc et argv qui ont le
 * meme sens que pour main(). Ici, ils s'appliquent aux arguments qui figuraient
 * sur la ligne de commandes saisie par l'utilisateur.
 */

/* Primitive socket()
 *=======================================================================
 *
 */

int socket_call(argc, argv)
int argc;
char *argv[];
{
    int s;
    int proto;
    static t_item list[] = { {"tcp", SOCK_STREAM}, {"udp", SOCK_DGRAM} };
    if (nbsock == MAXSOCK) {
        printf("Too much sockets already in use.\n");
        return (-1);
    }

    if (argc > 1)
        get_choice("Protocol?", list, 2, argv[1], &proto);
    else
        get_choice("Protocol?", list, 2, "", &proto);

    /* creation de la socket */
    s = socket(AF_INET, proto, 0);
    if (s < 0) {
        ERREUR("socket()");
        return (-1);
    }

    /* pour la reception de SIGIO */
    if (fcntl(s, F_SETOWN, getpid()) < 0) {
        ERREUR("fcntl()-SET_OWN");
        if (close(s) == -1)
            ERREUR("close()");

        return (-1);
    }

    if (DFT_MODE)
        printf("The socket identifier is %d\n", s);

    /* modification de la table */
    sock[nbsock] = s;
    dft_sock = nbsock++;
    return (dft_sock);
}

/* Primitive socket6() pour ipv6
 *=======================================================================
 *
 */
int socket6_call(argc, argv)
int argc;
char *argv[];
{
    int s;
    int proto;
    static t_item list[] = { {"tcp", SOCK_STREAM}, {"udp", SOCK_DGRAM} };
    if (nbsock == MAXSOCK) {
        printf("Too much sockets already in use.\n");
        return (-1);
    }

    if (argc > 1)
        get_choice("Protocol?", list, 2, argv[1], &proto);
    else
        get_choice("Protocol?", list, 2, "", &proto);

    /* creation de la socket */
    s = socket(AF_INET6, proto, 0);
    if (s < 0) {
        ERREUR("socket()");
        return (-1);
    }
    /* force IPV6, socket ne peut recevoir que des paquets IPV6 */
    int only = 1;

    if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &only, sizeof only) < 0) {
        ERREUR("setsockopt not IPv6 only");
        return (-1);
    }

    /* pour la reception de SIGIO */
    if (fcntl(s, F_SETOWN, getpid()) < 0) {
        ERREUR("fcntl()-SET_OWN");
        if (close(s) == -1)
            ERREUR("close()");

        return (-1);
    }

    if (DFT_MODE)
        printf("The IPv6 socket identifier is %d\n", s);

    /* modification de la table */
    sock[nbsock] = s;
    dft_sock = nbsock++;
    return (dft_sock);
}

/* Primitive bind()
 *=======================================================================
 *
 */

int bind_call(argc, argv)
int argc;
char *argv[];
{
    socklen_t lensa = sizeof(struct sockaddr_in);
    int port = -1;
    struct sockaddr_in6 addr;
    int so;
    int ip;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    ip = domainesock(sock[so]); /* domaine de la socket 4 ou 6 */

    /* host ? */
    if (argc > 2)
        get_host(argv[2], &addr, ip);
    else
        get_host("", &addr, ip);

    /* port ? */
    if (argc > 3)
        get_port(argv[3], &port);
    else
        get_port("", &port);

    if (ip == 4) {              /* socket ipv4 */
        ((struct sockaddr_in *)&addr)->sin_port = htons((u_short) port);
        lensa = sizeof(struct sockaddr_in);
        /* attachement de la socket */
        if (bind(sock[so], (struct sockaddr *)&addr, lensa) < 0) {
            ERREUR("bind()");
            return (-1);
        }

        if (port == 0) {        /* obtention du port choisi par le systeme */
            if (getsockname(sock[so], (struct sockaddr *)&addr, &lensa)
                == -1) {
                ERREUR("getsockname()");
                return (-1);
            }
            port = ntohs((u_short)
                         (((struct sockaddr_in *)&addr)->sin_port));

            if (DFT_MODE)
                printf("The socket was attributed port %d.\n", port);

        }
    } else {                    /*IPV6 */
        addr.sin6_port = htons((u_short) port);
        /*affichage adresse) */
        /*  display_inet_addr((struct sockaddr *)&addr); */
        lensa = sizeof(struct sockaddr_in6);
        /* attachement de la socket */
        if (bind(sock[so], (struct sockaddr *)&addr, lensa) < 0) {
            ERREUR("bind()");
            return (-1);
        }

        if (port == 0) {        /* obtention du port choisi par le systeme */
            if (getsockname(sock[so], (struct sockaddr *)&addr, &lensa)
                == -1) {
                ERREUR("getsockname()");
                return (-1);
            }

            port = ntohs(addr.sin6_port);       /* Modif P. Sicard */

            if (DFT_MODE)
                printf("The socket was attributed port %d.\n", port);
        }
    }
    return (port);
}

/* Primitive listen()
 *=======================================================================
 *
 */

int listen_call(argc, argv)
int argc;
char *argv[];
{
    int nb = -1;
    int so;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* backlog ? */
    nb = 5;
    if (argc > 2)
        get_nb("Nb of requests", argv[2], &nb);
    else
        get_nb("Nb of requests", "", &nb);

    /* mise en ecoute */
    if (listen(sock[so], nb) < 0) {
        ERREUR("listen()");
        return (-1);
    }

    return (0);
}

/* Primitive accept()
 *=======================================================================
 *
 */

int accept_call(argc, argv)
int argc;
char *argv[];
{
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    struct sockaddr *sa;
    int ip;

    socklen_t lensa;
    int newso;
    int so;
    char hbuf[NI_MAXHOST];
    char pbuf[NI_MAXSERV];

    if (nbsock == MAXSOCK) {
        printf("Too much sockets already in use.\n");
        return (-1);
    }

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    ip = domainesock(sock[so]); /*domaine de la socket 4 ou 6 */
    if (ip == 4) {              /* AF_INET; */
        sa = (struct sockaddr *)&sa4;
        lensa = sizeof(struct sockaddr_in);
    } else {
        sa = (struct sockaddr *)&sa6;
        lensa = sizeof(struct sockaddr_in6);
    }
    newso = accept(sock[so], sa, &lensa);
    if (newso < 0) {
        ERREUR("accept()");
        return (-1);
    }

    /* identification de l'appel entrant */
    if (getnameinfo(sa, lensa, hbuf, sizeof(hbuf), pbuf, sizeof(pbuf), NI_NUMERICSERV) != 0) {
        ERREUR("Cannot get remote IP address of incoming connection");
    } else {
        printf("A connection from %s (%s) was received.\n", hbuf, pbuf);
    }

    printf("Connection is established, with socket ID %d.\n", newso);

    /* modification de la table */
    sock[nbsock] = newso;
    dft_sock = nbsock++;
    return (dft_sock);
}

/* Primitive connect()
 *=======================================================================
 *
 */

int connect_call(argc, argv)
int argc;
char *argv[];
{
    int port = -1;
    struct sockaddr_in6 addr;   /* peut servir en ipv4 ou ipv6 */

    int so;
    int ip;
    int size;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    ip = domainesock(sock[so]); /* domaine de la socket 4 ou 6 */

    /* host ? */
    if (argc > 2)
        get_host(argv[2], &addr, ip);
    else
        get_host("", &addr, ip);

    /* port ? */
    if (argc > 3)
        get_port(argv[3], &port);
    else
        get_port("", &port);

    /* AF_INET; */
    if (ip == 4) {
        ((struct sockaddr_in *)&addr)->sin_port = htons((u_short) port);
        size = sizeof(struct sockaddr_in);
    } else {
        ((struct sockaddr_in6 *)&addr)->sin6_port = htons((u_short) port);
        size = sizeof(struct sockaddr_in6);
    }
    /* connexion */
    if (connect(sock[so], (struct sockaddr *)&addr, size) < 0) {
        ERREUR("connect()");
        return (-1);
    }

    printf("Connection established.\n");

    /* mise a jour de la table */
    return (0);
}

/* Primitive close()
 *=======================================================================
 *
 */

int close_call(argc, argv)
int argc;
char *argv[];
{
    int i;
    int so;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* fermeture de la socket */
    if (close(sock[so]) < 0)
        ERREUR("close()");

    /* mise a jour de la table */
    for (i = so + 1; i < nbsock; i++)
        sock[i - 1] = sock[i];

    nbsock--;
    if (so == dft_sock)
        dft_sock = -1;
    else if (dft_sock > so)
        dft_sock--;

    return (0);
}

/* Primitive shutdown()
 *=======================================================================
 *
 */

int shutdown_call(argc, argv)
int argc;
char *argv[];
{
    int sens = -1;
    static t_item list[] = { {"in", 0}, {"out", 1}, {"both", 2} };
    int so;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* sens ? */
    if (argc > 2)
        get_choice("Direction?", list, 3, argv[2], &sens);
    else
        get_choice("Direction?", list, 3, "", &sens);

    /* fermeture de la socket */
    if (shutdown(sock[so], sens) == -1) {
        ERREUR("shutdown()");
        return (-1);
    }

    return (0);
}

/* Primitive write()
 *=======================================================================
 *
 */

int write_call(argc, argv)
int argc;
char *argv[];
{
    int nb;
    char *msg;
    int so;

    msg = (char *)malloc(MAX_BUFFER * sizeof(char));

    /*id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* message ? */
    if (argc > 2) {
        strlcpy(msg, argv[2], MAX_BUFFER);
    } else {
        strlcpy(msg, "", MAX_BUFFER);
    }
    nb = MAX_BUFFER;
    get_msg(&msg, &nb);

    /* envoi du message */
    nb = write(sock[so], (void *)msg, (unsigned)strlen(msg));
    if (nb < 0) {
        ERREUR("write()");
        return (-1);
    }

    free(msg);
    printf("Sent %d bytes\n", nb);

    return (0);
}

/* Primitive send()
 *=======================================================================
 *
 */

int send_call(argc, argv)
int argc;
char *argv[];
{
    int nb;
    char *msg;
    int total;
    static int oob;
    static int dontroute;
    static int loop;
    static t_flg flgs[] = { {"oob", &oob},
    {"dontroute", &dontroute},
    {"loop", &loop}
    };
    int so;

    msg = (char *)malloc(MAX_BUFFER * sizeof(char));

    /* flags sur la ligne de commande */
    oob = dontroute = loop = 0;
    argc = check_flags(argc, argv, flgs, 3);

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* message ? */
    if (argc > 2) {
        strlcpy(msg, argv[2], MAX_BUFFER);
    } else {
        strlcpy(msg, "", MAX_BUFFER);
    }
    nb = MAX_BUFFER;
    get_msg(&msg, &nb);

    /* envoi du message */
    total = 0;
    do {
        nb = send(sock[so], msg, strlen(msg), MSG_OOB * oob | MSG_DONTROUTE * dontroute);
        if (nb >= 0)
            if (loop)
                printf("Sent %d bytes (total %d)\n", nb, total += nb);
            else
                printf("Sent %d bytes\n", nb);
        else {
            ERREUR("send()");
            return (-1);
        }
    }
    while (loop && nb > 0);
    free(msg);
    return (0);
}

/* Primitive sendto()
 *=======================================================================
 *
 * Meme chose que la fonction send_call ci-dessus mais en precisant l'hote...
 */

int sendto_call(argc, argv)
int argc;
char *argv[];
{
    int nb, ip, size;
    char *msg;
    int total = 0;
    int port = -1;
    struct sockaddr_in6 addr;
    static int oob;
    static int dontroute;
    static int loop;
    static t_flg flgs[] = { {"oob", &oob},
    {"dontroute", &dontroute},
    {"loop", &loop}
    };

    int so;

    msg = (char *)malloc(MAX_BUFFER * sizeof(char));

    /* flags sur la ligne de commande */
    oob = dontroute = loop = 0;
    argc = check_flags(argc, argv, flgs, 3);

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    ip = domainesock(sock[so]); /* domaine de la socket 4 ou 6 */

    /* host ? */
    if (argc > 2)
        get_host(argv[2], &addr, ip);
    else
        get_host("", &addr, ip);

    /* port ? */
    if (argc > 3)
        get_port(argv[3], &port);
    else
        get_port("", &port);

    /* message ? */
    if (argc > 4) {
        strlcpy(msg, argv[4], MAX_BUFFER);
    } else {
        strlcpy(msg, "", MAX_BUFFER);
    }
    nb = MAX_BUFFER;
    get_msg(&msg, &nb);

    /* AF_INET; */
    if (ip == 4) {
        ((struct sockaddr_in *)&addr)->sin_port = htons((u_short) port);
        size = sizeof(struct sockaddr_in);
    } else {
        ((struct sockaddr_in6 *)&addr)->sin6_port = htons((u_short) port);
        size = sizeof(struct sockaddr_in6);
    }
    /* envoi du message */
    do {
        nb = sendto(sock[so], msg, (unsigned int)strlen(msg), MSG_OOB * oob | MSG_DONTROUTE * dontroute, (struct sockaddr *)&addr, size);
        if (nb >= 0)
            if (loop)
                printf("Sent %d bytes (total %d)\n", nb, total += nb);
            else
                printf("Sent %d bytes\n", nb);
        else {
            ERREUR("sendto()");
            return (-1);
        }
    }
    while (loop && nb > 0);
    free(msg);
    return (0);
}

/* Primitive read()
 *=======================================================================
 *
 */

int read_call(argc, argv)
int argc;
char *argv[];
{
    int nb = -1;
    int lus;
    int so;
    char *msg;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* combien ? */
    nb = 100;
    if (argc > 2)
        get_nb("Nb of bytes to read", argv[2], &nb);
    else
        get_nb("Nb of bytes to read", "", &nb);

    msg = (char *)malloc((nb + 1) * sizeof(char));

    /* lecture des donnees */
    lus = read(sock[so], (void *)msg, (unsigned)nb);
    if (lus < 0) {
        ERREUR("read()");
        return (-1);
    }

    msg[lus] = (char)0;
    printf("Read %d bytes: message=<%s>\n", lus, msg);
    free(msg);
    return (0);
}

/* Primitive recv()
 *=======================================================================
 *
 */

int recv_call(argc, argv)
int argc;
char *argv[];
{
    int nb = -1;
    int lus;
    char msg[MAX_BUFFER];
    static int oob;
    static int peek;
    static t_flg flgs[] = { {"oob", &oob},
    {"peek", &peek}
    };
    int so;

    /* flags sur la ligne de commande */
    oob = peek = 0;
    argc = check_flags(argc, argv, flgs, 2);

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* combien ? */
    nb = 100;
    if (oob)
        nb = 1;                 // Un seul octet transmis en urgent...
    else {
        if (argc > 2)
            get_nb("Nb of bytes to read", argv[2], &nb);
        else
            get_nb("Nb of bytes to read", "", &nb);
    }
    /* lecture des donnees */
    lus = recv(sock[so], msg, nb, MSG_OOB * oob | MSG_PEEK * peek);
    if (lus < 0) {
        ERREUR("recv()");
        return (-1);
    }

    msg[lus] = (char)0;
    printf("Read %d bytes: message=<%s>\n", lus, msg);

    return (0);
}

/* Primitive recvfrom()
 *=======================================================================
 *
 */

int recvfrom_call(argc, argv)
int argc;
char *argv[];
{
    int nb;
    char *msg;
    int lus;
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    struct sockaddr *sa;

    socklen_t lensa;
    static int oob;
    static int peek;
    static t_flg flgs[] = { {"oob", &oob},
    {"peek", &peek}
    };
    int so;
    char hbuf[NI_MAXHOST];
    char pbuf[NI_MAXSERV];
    int ip;

    /* flags sur la ligne de commande */
    oob = peek = 0;
    argc = check_flags(argc, argv, flgs, 2);

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* combien ? */
    nb = 100;
    if (oob)
        nb = 1;                 // Un seul octet transmis en urgent...
    else {
        if (argc > 2)
            get_nb("Nb of bytes to read", argv[2], &nb);
        else
            get_nb("Nb of bytes to read", "", &nb);
    }
    msg = (char *)malloc((nb + 1) * sizeof(char));

    ip = domainesock(sock[so]); /* domaine de la socket 4 ou 6 */

    /* AF_INET; */
    if (ip == 4) {
        sa = (struct sockaddr *)&sa4;
	lensa = sizeof(struct sockaddr_in);
    } else {
        sa = (struct sockaddr *)&sa6;
	lensa = sizeof(struct sockaddr_in6);
    }
    /* lecture */
    lus = recvfrom(sock[so], msg, nb, MSG_OOB * oob | MSG_PEEK * peek, sa, &lensa);

    if (lus < 0) {
        ERREUR("recvfrom()");
        return (-1);
    }

    /* identification de l'origine des donnees */
    if (getnameinfo(sa, lensa, hbuf, sizeof(hbuf), pbuf, sizeof(pbuf), NI_NUMERICSERV) != 0) {
        ERREUR("Cannot get remote IP address of incoming connection");
    } else {
        printf("A message of length %d bytes was received from %s (%s).\n", lus, hbuf, pbuf);
    }
    msg[lus] = (char)0;
    printf("Message=<%s>\n", msg);
    free(msg);
    return (0);
}

/*_______________________________________________________________________________
 *_______________________________________________________________________________
 */

/* Primitive msocket()
 *=======================================================================
 * permet de creer une socket UDP SANS L'ATTACHER A UN PORT
 *
 */

int msocket_call(argc, argv)
int argc;
char *argv[];
{
    static char *socket_argv[] = { "socket", "udp", 0 };
    int so;

    so = socket_call(2, socket_argv);
    if (so == -1)
        return (-1);

    printf("UDP socket created: Id=%d\n", sock[so]);
    /*Modif P. Sicard modification du TTL car par defaut=1 */
    {
        u_char ttl = TTL_MCAST;
        if (setsockopt(sock[so], IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) == -1)
            ERREUR("Warning: default TTL is 1\n");
    }
    return (0);
}

/*------------------------------------------------------------------------*/

/* Primitive mbind()
 *=======================================================================
 *
 */

int mbind_call(argc, argv)
int argc;
char *argv[];
{
    int so;
    int port;
    struct sockaddr_in sin;

    /* "Bind" de la nouvelle socket sur un port */

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* port ? */
    if (argc > 3)
        get_port(argv[3], &port);
    else
        get_port("", &port);

    /* Initialisation de la structure necessaire au "bind" */

    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    /* Realisation du "bind" */

    if (bind(sock[so], (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        perror("bind");
        return (-1);
    }
    return (0);
}

/*------------------------------------------------------------------------*/

/* Primitive mjoin()
 *=======================================================================
 * Permet de rejoindre un groupe multicast
 *
 */

int mjoin_call(argc, argv)
int argc;
char *argv[];
{
    int so;
    struct ip_mreq imr;
    u_long addr = INADDR_NONE;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* Quel groupe joindre ? */
    if (argc > 2)
        get_group(argv[2], &addr);
    else
        get_group("", &addr);
    imr.imr_multiaddr.s_addr = addr;

    /* Sur quelle interface ? */
    if (argc > 3)
        get_itf(argv[3], &addr);
    else
        get_itf("", &addr);
    imr.imr_interface.s_addr = addr;

    /* Realisation du "setsockopt" */
    if (setsockopt(sock[so], IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&imr, sizeof(struct ip_mreq)) == -1)
        ERREUR("Unable to join group");
    else
        printf("Done joining group %s.\n", inet_ntoa(imr.imr_multiaddr));

    return (0);

}

/*------------------------------------------------------------------------*/

/* Primitive mleave()
 *=======================================================================
 * Permet de quitter un groupe multicast
 *
 */

int mleave_call(argc, argv)
int argc;
char *argv[];
{
    int so;
    struct ip_mreq imr;
    u_long addr = INADDR_NONE;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* Quel groupe quitter ? */
    if (argc > 2)
        get_group(argv[2], &addr);
    else
        get_group("", &addr);
    imr.imr_multiaddr.s_addr = addr;

    /* Sur quelle interface ? */
    if (argc > 3)
        get_itf(argv[3], &addr);
    else
        get_itf("", &addr);
    imr.imr_interface.s_addr = addr;

    /* Realisation du "setsockopt" */
    if (setsockopt(sock[so], IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&imr, sizeof(struct ip_mreq)) == -1)
        ERREUR("Unable to leave group");
    else
        printf("Done leaving group %s.\n", inet_ntoa(imr.imr_multiaddr));

    return (0);

}

/*------------------------------------------------------------------------*/

/* Primitive msendto()
 *=======================================================================
 *
 */

int msendto_call(argc, argv)
int argc;
char *argv[];
{
    int so;
    struct sockaddr_in sa;
    int port = 1111;
    int nb;
    char *msg;
    int total = 0;
    static int loop;
    struct in_addr ifaddr;
    u_long d_addr = INADDR_NONE;
    u_long i_addr = INADDR_NONE;

    msg = (char *)malloc(MAX_BUFFER * sizeof(char));

    /* Quelle socket utiliser ? */

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* A quel groupe envoyer le message ? */
    if (argc > 2)
        get_group(argv[2], &d_addr);
    else
        get_group("", &d_addr);

    /* Sur quelle interface ? */
    if (argc > 3)
        get_itf(argv[3], &i_addr);
    else
        get_itf("", &i_addr);

    /* Socket source deja bindee */
    /* modification des options de la socket source */
    ifaddr.s_addr = i_addr;

    if (setsockopt(sock[so], IPPROTO_IP, IP_MULTICAST_IF, (char *)&ifaddr.s_addr, sizeof(ifaddr.s_addr)) == -1) {
        perror("can't set multicast source interface");
        return (-1);
    }

    /* Numero de port destination ? */
    /* modif P. Sicard Port source et port destination pas forcement les memes */
    if (argc > 4)
        get_port(argv[4], &port);
    else
        get_port("", &port);

    /* Initialisation de l'adresse destination */
    memset((char *)&sa, 0, sizeof(struct sockaddr_in));

    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short) port);
    sa.sin_addr.s_addr = d_addr;

    /* message ? */
    if (argc > 4) {
        strlcpy(msg, argv[4], MAX_BUFFER);
    } else {
        strlcpy(msg, "", MAX_BUFFER);
    }
    nb = MAX_BUFFER;
    get_msg(&msg, &nb);

    /* envoi du message */
    do {
        nb = sendto(sock[so], msg, (unsigned int)strlen(msg), 0, (struct sockaddr *)&sa, sizeof(sa));
        if (nb >= 0)
            if (loop)
                printf("Sent %d bytes (total %d)\n", nb, total += nb);
            else
                printf("Sent %d bytes\n", nb);
        else {
            ERREUR("sendto()");
            return (-1);
        }
    }
    while (loop && nb > 0);
    free(msg);
    return (0);
}

/*------------------------------------------------------------------------*/

/* Primitive mrecvfrom()
 *=======================================================================
 *
 */

int mrecvfrom_call(argc, argv)
int argc;
char *argv[];
{
    int count = 1;
    struct timeval start, stop;
    struct sockaddr_in from;
    socklen_t fromlen = 0;
    int read = 0;
    int so;

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    if (argc > 2)
        get_nb("Nb of messages to receive", argv[2], &count);
    else
        get_nb("Nb of messages to receive", "", &count);

    gettimeofday(&start, NULL);

    while (count-- > 0) {
        char buf[BUFSIZ];
        printf("Waiting for incoming messages...\n");
        fromlen = sizeof(from);
        read = recvfrom(sock[so], buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen);
        if (read < 0) {
            perror("recvfrom");
            return (-1);
        }
        buf[read] = '\0';
        printf("Received a message of length %d bytes.\n", read);
        printf("Message received: %s\n", buf);
        strlcpy(buf, "", sizeof(buf));

        fflush(NULL);
    }

    gettimeofday(&stop, NULL);
    if (fromlen) {
        printf("Received from %s in %ld seconds\n", inet_ntoa(from.sin_addr), stop.tv_sec - start.tv_sec);
    }
    return (0);
}

/*------------------------------------------------------------------------*/

/* Primitive bsend()
 *=======================================================================
 *
 */

int bsend_call(argc, argv)
int argc;
char *argv[];
{
    int so;
    int enable = 1;
    char sockfd[8];
    char *sockarg[] = { "sendto from bsend", sockfd };

    /* Quelle socket utiliser ? */

    /* id socket ? */
    if (argc > 1)
        get_id_sock(argv[1], &so);
    else
        get_id_sock("", &so);

    /* Realisation du "setsockopt" */

    if (setsockopt(sock[so], SOL_SOCKET, SO_BROADCAST, (void *)&enable, sizeof(enable)) < 0) {
        ERREUR("setsockopt()-SO_BROADCAST");
        return (-1);
    }

    sprintf(sockfd, "%d", sock[so]);
    return sendto_call(2, sockarg);
}
