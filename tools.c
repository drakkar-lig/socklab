/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * tools.c: outils divers (saisie de parametres...)
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void rl_gets(str, len)
char *str;
int len;
{
    char *line_read;

    /* Get a line from the user.  */
    line_read = readline(" ");

    /* et on la renvoit. */
    if (line_read) {
        strlcpy(str, line_read, len);
    }
    /* et hop un peu de menage... */
    free(line_read);
}

/* Obtention de l'adresse d'un host
 *=======================================================================
 *
 */

int host_addr(host, addr, version)
char host[];
struct sockaddr *addr;
int version;

{
    struct addrinfo hints, *ai;
    int error;

    memset(&hints, 0, sizeof(hints));
    if (version == 4)
        hints.ai_family = AF_INET;      // socket IPV4
    else
        hints.ai_family = AF_INET6;     // socket IPV6

    /*  printf("host %s\n",host); */
    if (!strcmp(host, "*")) {
        /* sert pour la creation d'une socket passive  */
        hints.ai_flags = AI_PASSIVE;
        host = NULL;
    }
    /* port mis à "0" car node et service ne peuvent etre a NULL en meme temps */
    if ((error = getaddrinfo(host, "0", &hints, &ai)) != 0) {
        VIDEO_INV_ERR(fprintf(stderr, "%s\n", gai_strerror(error)));
        return (-1);
    }
    for (; ai != NULL; ai = ai->ai_next) {
        if (ai->ai_family == AF_INET) {
            *addr = *((struct sockaddr *)ai->ai_addr);
            freeaddrinfo(ai);
            return (0);
        }
        if (ai->ai_family == AF_INET6) {
            *((struct sockaddr_in6 *)addr) = *((struct sockaddr_in6 *)ai->ai_addr);
            /*attention sockaddr_in6 est plus grand que sockaddr */

            /*affichage adresse) */
            /* display_inet_addr((struct sockaddr *)addr); */
            freeaddrinfo(ai);
            return (0);
        }
    }

    VIDEO_INV_ERR(fprintf(stderr, "no AF_INET(6) address returned\n"));

    return (-1);

}

/* Obtention d'un numero de port d'un service
 *====================================================================

 */

int port_number(name, port)
char name[];
int *port;
{
    struct addrinfo hints, *ai;
    int error;
    int tmp_port;

    //struct servent *sp;

    if (sscanf(name, "%d", &tmp_port) == 1) {
        if (tmp_port >= 0 && tmp_port <= 65535) {
            *port = tmp_port;
	    return (0);
	} else {
	    return (-1);
	}
    } else {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;    // fonctionne pour IPV4 et IPV6
        if ((error = getaddrinfo(NULL, name, &hints, &ai)) != 0) {
            VIDEO_INV_ERR(fprintf(stderr, "%s\n", gai_strerror(error)));
            return (-1);
        }
        for (; ai != NULL; ai = ai->ai_next) {
            if (ai->ai_family == AF_INET) {
                *port = ntohs((((struct sockaddr_in *)ai->ai_addr)->sin_port));
                freeaddrinfo(ai);
                return (0);
            } else if (ai->ai_family == AF_INET6) {
                *port = ntohs((((struct sockaddr_in6 *)
                                ai->ai_addr)->sin6_port));
                freeaddrinfo(ai);
                return (0);

            }

        }

    }
    VIDEO_INV_ERR(fprintf(stderr, "no AF_INET port returned\n"));
    return (-1);

}

/* Saisie d'une adresse d'un host
 *=======================================================================
 * L'utilisateur peut donner un nom de machine ou une adresse brute (dot
 * notation). Si la machine est inconnue ou si le format d'adresse est
 * incorrect, la demande est reformulee. L'adresse est retournee par
 * parametre. Les adresses sont retournees en network byte order
 * (pas de hton_ necessaire par la suite !)
 */
void get_itf_host();

void get_host(name, addr, version)
char name[];
struct sockaddr *addr;
int version;

{
    get_itf_host(name, addr, version, "Host?");
}

void get_itf(name, addr, version)
char name[];
struct sockaddr *addr;
int version;

{
    get_itf_host(name, addr, version, "Interface address?");
}

void get_itf_host(name, addr, version, prompt)
char name[];
struct sockaddr *addr;
int version;

char prompt[];
{
    char str[MAX_HOSTNAME];

    strlcpy(str, name, sizeof(str));
    for (;;) {
        if (strcmp(str, "")) {
            if (!strcmp(str, ".")) {
                if (version == 4) {
                    if (host_addr("127.0.0.1", addr, version) == 0)
                        return;
                } else          //IPV6
                if (host_addr("::1", addr, version) == 0)
                    return;
            } else if (host_addr(str, addr, version) == 0)      /* c'est une machine quelconque  */
                return;

        }
        printf("%s : ", prompt);
/*		printf("Host (\"*\"=INADDR_ANY, \".\"=<local host>) ?: ");*/
        rl_gets(str, sizeof(str));
    }
}

/* Saisie d'une adresse de groupe IP sous forme numerique
 *=======================================================================
 * L'utilisateur doit donner une adresse brute (dot notation).
 * Si le format d'adresse est incorrect, la demande est reformulee.
 * L'adresse est retournee par parametre. 
 */

void get_group(name, addr)
char name[];
u_long *addr;
{
    char str[MAX_HOSTNAME];

    strlcpy(str, name, sizeof(str));
    for (;;) {
        *addr = inet_addr(str);
        if (*addr != INADDR_NONE)
            return;

        printf("Group address? : ");
        rl_gets(str, sizeof(str));
    }
}

/* Saisie d'un numero de port
 *=======================================================================
 */

void get_port(name, port)
char name[];
int *port;
{
    char str[MAX_PORTNAME];

    strlcpy(str, name, sizeof(str));
    for (;;) {
        if (strcmp(str, "") && port_number(str, port) == 0)
            return;

        printf("Port? : ");
        rl_gets(str, sizeof(str));
    }

}

/* Saisie d'un message a envoyer
 *=======================================================================
 * L'utilisateur peut entrer un message precis ou juste donner sa taille
 * le raccourci "#n" ou  n est la taille desiree. Un message de n caracte-
 * res '*' est alors cree.
 */

void get_msg(msg, size)
char **msg;
int *size;
{
    int nb;

    for (;;) {
        if (strcmp(*msg, "")) {
            if (sscanf(*msg, "#%d", &nb) == 1) {
                if (nb >= 0) {  /* attention : pas de limite de taille !!! */
                    free(*msg);
                    *msg = (char *)malloc((nb + 1) * sizeof(char));
                    memset(*msg, '*', nb);
                    (*msg)[nb] = (char)0;
                    *size = nb;
                    return;
                }
            } else {
                int len = strlen(*msg);
                *size = len;
                // replace trailing space with \n
                len--;
                if ((len >= 0) && ((*msg)[len] == ' ')) {
                    (*msg)[len] = '\n';
                }
                // and double trailing spaces with \r\n to easily send HTTP requests
                len--;
                if ((len >= 0) && ((*msg)[len] == ' ')) {
                    (*msg)[len] = '\r';
                    // plus all double spaces in the string with \r\n
                    for (; len >= 1; len--) {
                        if (((*msg)[len] == ' ') && ((*msg)[len - 1] == ' ')) {
                            (*msg)[len--] = '\n';
                            (*msg)[len]   = '\r';
                        }
                    }
                }
                return;
            }
        }
        printf("Message? : ");
        rl_gets(*msg, *size);
    }
}

/* Lecture d'un entier:
 *=========================================================================
 *
 *
 */

void get_nb(prompt, init, nb)
char prompt[];
char init[];
int *nb;
{
    int defval;
    char str[50];

    defval = *nb;
    if (!strcmp(init, "")) {
        strlcpy(str, "x", sizeof(str));
    } else {
        strlcpy(str, init, sizeof(str));
    }
    for (;;) {
        /* peut etre faut-il supprimer les eventuels blancs ? */
        if (!strcmp(str, ""))
            return;
        else if (sscanf(str, "%d", nb) == 1)
            return;

        printf("%s [%d] : ", prompt, defval);

        rl_gets(str, sizeof(str));
    }
}

/* Lecture d'un choix parmi n
 *===========================================================================
 *
 */

void get_choice(prompt, list, nb, init, choice)
char prompt[];
t_item list[];
int nb;
char init[];
int *choice;
{
    char str[50];
    int i;

    strlcpy(str, init, sizeof(str));
    for (;;) {
        if (strcmp(str, ""))
            for (i = 0; i < nb; i++)
                if (!strncmp(str, list[i].name, strlen(str))) {
                    *choice = list[i].val;
                    return;
                }

        printf("%s [", prompt);
        for (i = 0; i < nb - 1; i++)
            printf("%s/", list[i].name);
        printf("%s] : ", list[nb - 1].name);
        rl_gets(str, sizeof(str));
    }
}

/* Lecture d'une valeur booleenne (O/N)
 *========================================================================
 *
 */

void get_bool(prompt, var)
char prompt[];
int *var;
{
    char str[50];

    *var = (*var ? 1 : 0);
    for (;;) {
        printf("%s [%s] : ", prompt, (*var ? "Y/n" : "y/N"));
        rl_gets(str, sizeof(str));
        switch (my_toupper(str[0])) {
        case 0:
            return;
        case 'Y':
        case 'O':
            *var = 1;
            return;
        case 'N':
            *var = 0;
            return;
        }
    }
}

/* saisie du numero de socket a traiter
 *=======================================================================
 *
 */

void get_id_sock(init, so)
char init[];
int *so;
{
    char str[50];
    int id;

    if (!strcmp(init, "")) {
        strlcpy(str, "x", sizeof(str));
    } else if (!strcmp(init, ".")) {
        sprintf(str, "%d", id_sock);
    } else {
        strlcpy(str, init, sizeof(str));
    }
    for (;;) {
        if (!strcmp(str, ""))
            sprintf(str, "%d", id_sock);

        if (sscanf(str, "%d", &id) == 1) {
            for (*so = 0; *so < nbsock && sock[*so] != id; (*so)++) ;
            if (*so < nbsock) {
                dft_sock = *so;
                return;
            }
        }

        if (dft_sock != -1)
            printf("Id. socket (%d) ?: ", id_sock);
        else
            printf("Id socket ?: ");

        rl_gets(str, sizeof(str));
    }
}

/* demande de confirmation
 *=======================================================================
 *
 */

int ask(prompt)
char prompt[];
{
    int rep;

    rep = 0;
    get_bool(prompt, &rep);

    return (rep);
}

/* affichage de l'adresse d'une socket INTERNET:
 *=======================================================================
 * Utilisee uniquement pour debugger
 */

void display_inet_addr(addr)
struct sockaddr *addr;
{
    char ipstr[INET6_ADDRSTRLEN];
    char ipver;
    void *p;
    int port;

    if (addr->sa_family == AF_INET) {   // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;
        p = &(ipv4->sin_addr);
        port = ntohs(ipv4->sin_port);
        ipver = '4';
    } else {                    // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr;
        p = &(ipv6->sin6_addr);
        port = ntohs(ipv6->sin6_port);
        ipver = '6';
    }

    // Conversion de l'adresse IP en une chaîne de caractères
    inet_ntop(addr->sa_family, p, ipstr, sizeof ipstr);
    printf("IPv%c address: %s\n", ipver, ipstr);
    printf("Port:         %d\n", port);
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    if (getnameinfo(addr, sizeof(addr), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
        printf("Host Name=%s, Service Name =%s", hbuf, sbuf);

    printf("\n");
}

/* affichage d'un code d'erreur
 *=======================================================================
 * Appelee quand une primitive retourne une erreur (-1).
 * Le message est affiche en video inversee
 */

void ERREUR(msg)
char msg[];
{
    VIDEO_INV_ERR(perror(msg));
}

/* recuperation version IP d une socket
*=======================================================================
* retourne 4 ou 6  suivant l'option IPV6_BINDV6ONLY
* retourne -1 en cas d'erreur 
 */

int domainesock(socket)
int socket;
{
    int only = 0;
    socklen_t size;
    if (getsockopt(socket, IPPROTO_IPV6, IPV6_V6ONLY, &only, &size) < 0)
        return (4);             /* IPV4 a priori */

    if (only)                   /* IPV6 ONLY */
        return (6);
    return (-1);
}
