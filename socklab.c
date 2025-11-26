/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 * L.CROQ & L.SALI & P. SICARD
 *
 * socklab.c: module principal
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"
#include "version.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int sock[MAXSOCK];              /* table des sockets gerees */
int nbsock;                     /* nb de sockets gerees */
int dft_sock;                   /* socket par defaut */
char *cmd_prompt;               /* prompt de l'interpreteur de commandes */
int exec_mode;                  /* mode d'exploitation */
t_cmd *cmds;                    /* Commandes disponibles */
sigjmp_buf ihm_env;             /* pour retourner a l'ihm apres un ctrl-C */

/* Commandes par defaut */

t_cmd cmds_STD[] = {
    {"socket",     0, 's', socket_call,    "[tcp|udp]"},
    {"socket6",    0, 'n', socket6_call,   "[tcp|udp]"},
    {"bind",       0, 'b', bind_call,      "[id] [host] [port]"},
    {"listen",     0, 'l', listen_call,    "[id] [nb]"},
    {"accept",     0, 'a', accept_call,    "[id] "},
    {"connect",    0, 'c', connect_call,   "[id] [host] [port]"},
    {"close",      0, 'k', close_call,     "[id] "},
    {"shutdown",   0, 'h', shutdown_call,  "[id] [in|out|both]"},
    {"options",    0, 'o', socket_options, "[id] [name]..."},
    {"read",       1, 'r', read_call,      "[id] [nb]"},
    {"recv",       1, 'v', recv_call,      "[id] [nb] [-oob] [-peek]"},
    {"recvfrom",   1, 'f', recvfrom_call,  "[id] [nb] [-oob] [-peek]"},
    {"write",      2, 'w', write_call,     "[id] [mesg]"},
    {"send",       2, 'd', send_call,      "[id] [mesg] [-oob] [-dontroute] [-loop]"},
    {"sendto",     2, 't', sendto_call,    "[id] [host] [port] [mesg] [-oob] [-dontroute] [-loop]"},
    {"help",       3, '?', help_cmd,       "[cmd]..."},
    {"status",     3, '=', sock_status,    ""},
    {"quit",       3, 'q', terminaison,    ""},
    {"exit",       3, 'x', terminaison,    ""},
    {"",          -1, ' ', NULL,           ""}
};

/* Commandes en mode udp */

/* cc96-2 */

t_cmd cmds_UDP[] = {
    {"socket",     0, 's', UDP_socket,     ""},
    {"socket6",    0, 'n', UDP_socket6,    ""},
    {"msocket",    0, 'c', msocket_call,   ""},
    {"close",      0, 'k', close_call,     "[id]"},
    {"options",    0, 'o', socket_options, "[id] [name]..."},
    {"mjoin",      0, 'j', mjoin_call,     "[id] [group] [local]"},
    {"mleave",     0, 'l', mleave_call,    "[id] [group] [local]"},
    {"mbind",      0, 'b', mbind_call,     "[id] [port]"},
    {"recvfrom",   1, 'f', recvfrom_call,  "[id] [nb]"},
    {"mrecvfrom",  1, 'r', mrecvfrom_call, "[id] [nb]"},
    {"sendto",     2, 't', sendto_call,    "[id] [host] [port] [mesg]"},
    {"msendto",    2, 'm', msendto_call,   "[id] [group] [local]"},
    {"bsend",      2, 'a', bsend_call,     "[id]"},
    {"help",       3, '?', help_cmd,       "[cmd]..."},
    {"status",     3, '=', sock_status,    ""},
    {"quit",       3, 'q', terminaison,    ""},
    {"exit",       3, 'x', terminaison,    ""},
    {"",          -1, ' ', NULL,           ""}
};

/* Commandes en mode tcp */

t_cmd cmds_TCP[] = {
    {"passive",    0, 'p', TCP_passive,    ""},
    {"passive6",   0, 'n', TCP_passive6,   ""},
    {"accept",     0, 'a', accept_call,    "[id]"},
    {"connect",    0, 'c', TCP_connect,    "[host] [port]"},
    {"connect6",   0, 'e', TCP_connect6,   "[host] [port]"},
    {"close",      0, 'k', close_call,     "[id]"},
    {"shutdown",   0, 'h', shutdown_call,  "[id] [in|out|both]"},
    {"options",    0, 'o', socket_options, "[id] [name]..."},
    {"read",       1, 'r', read_call,      "[id] [nb]"},
    {"urecv",      1, 'v', TCP_urecv,      "[id] [nb]"},
    {"write",      2, 'w', write_call,     "[id] [mesg]"},
    {"usend",      2, 'd', TCP_usend,      "[id] [mesg]"},
    {"help",       3, '?', help_cmd,       "[cmd]..."},
    {"status",     3, '=', sock_status,    ""},
    {"quit",       3, 'q', terminaison,    ""},
    {"exit",       3, 'x', terminaison,    ""},
    {"",          -1, ' ', NULL,           ""}
};

/* handler du signal SIGIO
 *=======================================================================
 * SIGIO est recu quand une socket vient de recevoir des donnees et quand
 * celle-ci marche en mode asynchrone.
 */

void SIGIO_handler(int sig)
{
    printf("\n\7*** a socket just received data or a new connection attempt\n");
}

/* handler du signal SIGPIPE
 *=======================================================================
 * SIGPIPE est recu quand un processus veut ecrire dans une socket qui a
 * ete fermee "de l'autre cote"
 *
 */

void SIGPIPE_handler(int sig)
{
    printf("\n\7*** the SIGPIPE signal has just been caught\n");
}

/* handler du signal SIGINT
 *=======================================================================
 * SIGINT est genere sur un ctrl-C de l'utilisateur.
 * Ici, on retourne a l'ihm
 */

void SIGINT_handler(int sig)
{
    siglongjmp(ihm_env, 1);
}

void SIGURG_handler(int sig)
{
    printf("SIGURG signal received\n");
}

/* retour au shell
 *=======================================================================
 */

int terminaison(int argc, char *argv[])
{
    if (ask("Do you really want to exit socklab?") == 0)
        return (0);
    else
        exit(0);
    return (1);
}

/* Affichage de l'etat les sockets creees
 *=======================================================================
 *
 */

int sock_status(int argc, char *argv[])
{
    int type;
    socklen_t lentype;
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    socklen_t lensa;
    char str[100];  /* Needs to be larger than hbuf + some space */
    int i;
    int c;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    int maxfdpl;
    int the_ttl;
    struct timeval timeout;
    socklen_t len = 0;          /* input */
    char hbuf[64];
    char ipstr[INET6_ADDRSTRLEN];
    int ip, lgstradr = 0, lgstradr2 = 0;

    if (nbsock == 0) {
        printf("No socket has been created yet.\n");
        return (0);
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    maxfdpl = 0;
    for (i = 0; i < nbsock; i++) {
        FD_SET(sock[i], &readfds);
        FD_SET(sock[i], &writefds);
        FD_SET(sock[i], &exceptfds);
        if (sock[i] >= maxfdpl)
            maxfdpl = sock[i] + 1;
    }

    timeout.tv_usec = 0L;
    timeout.tv_sec = 0L;
    if (select(maxfdpl, &readfds, &writefds, &exceptfds, &timeout) == -1)
        ERREUR("select()");

    printf(" Id  Proto   Local address              Remote address             TYPE  RWX ?\n");
    printf(" ---------------------------------------------------------------------------\n");

    for (i = 0; i < nbsock; i++) {
        printf("%c%-4d", (i == dft_sock ? '>' : ' '), sock[i]);

        lentype = sizeof(int);
        if (getsockopt(sock[i], SOL_SOCKET, SO_TYPE, (char *)&type, &lentype)
            < 0) {
/*BUG get sockopt PS 2000 apparu avec nouvelle version de solaris */
            /*      ERREUR ("getsockopt()"); 
               return (-1); */
        }
        switch (type) {
        case SOCK_STREAM:
            printf("TCP     ");
            break;
        case SOCK_DGRAM:
            printf("UDP ");
            //modif martin est-ce une sock multicast ?

            ip = domainesock(sock[i]);
            if (ip == 4) {      /* AF_INET; */
                if (getsockopt(sock[i], IPPROTO_IP, IP_MULTICAST_TTL, (char *)&the_ttl, &lentype) < 0)
                    // Si le TTL a ete modifie, c'est une sock. multicast...
                    // C'est le seul indice que j'ai trouve.
                    printf("pb in getsockopt... sock ipv4 multicast TTL ? \n");
            } else              //IPV6
            {
                if (getsockopt(sock[i], IPPROTO_IPV6, IP_MULTICAST_TTL, (char *)&the_ttl, &lentype) < 0)
                    // Si le TTL a ete modifie, c'est une sock. multicast...
                    // C'est le seul indice que j'ai trouve.
                    printf("pb in getsockopt... sock ipv4 multicast TTL ? \n");
            }

            if (the_ttl == TTL_MCAST)
                printf("M   ");
            else if (the_ttl == 1)
                printf("U   ");
            else
                printf("%d  \n", type);
            break;
        default:
            printf("?       ");
            break;
        }

        lensa = sizeof(struct sockaddr_in6);
        if (getsockname(sock[i], (struct sockaddr *)&sa6, &lensa) == -1) {
            ERREUR("getsockname()");
            return (-1);
        } else if (lensa == sizeof(struct sockaddr_in6)) {      /* socket IPV6 */
            if (sa6.sin6_port == 0)     // socket non bindee
            {
                printf("%-25c  ", '-');
                lgstradr = 0;
            } else {
                /* if (sa6.sin6_addr == in6addr_any)
                   sprintf(str, "*(%d)", ntohs(sa6.sin6_port));
                   else { */
                if (getnameinfo((struct sockaddr *)&sa6, len, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD))
                    /* Resolution de nom impossible ou nom trop long */
                {
                    inet_ntop(sa6.sin6_family, &(sa6.sin6_addr), ipstr, sizeof ipstr);
                    sprintf(str, "%s(%d)", ipstr, ntohs(sa6.sin6_port));
                } else
                    sprintf(str, "%s(%d)", hbuf, ntohs(sa6.sin6_port));

                printf("%-25s  ", str);
                if ((lgstradr = strlen(str)) > 25)      // passage à la ligne pour adresse suivante
                    printf("\n");
            }

            lensa = sizeof(struct sockaddr_in6);
            c = getpeername(sock[i], (struct sockaddr *)&sa6, &lensa);
            if (c == 0) {
                if (getnameinfo((struct sockaddr *)&sa6, len, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD))
                    /* Resolution de nom impossible ou nom trop long */
                {
                    inet_ntop(sa6.sin6_family, &(sa6.sin6_addr), ipstr, sizeof ipstr);
                    sprintf(str, "%s(%d)", ipstr, ntohs(sa6.sin6_port));
                } else
                    sprintf(str, "%s(%d)", hbuf, ntohs(sa6.sin6_port));
                lgstradr2 = strlen(str);
                if (lgstradr < 25)      // passage a la ligne non fait
                {
                    if (lgstradr2 < 25) //affichage normal
                        printf("%-25s  ", str);
                    else        //il faut passer à la ligne pour la deuxieme adresse
                        printf("                    %-45s  ", str);
                } else          //passage a la ligne fait
                {
                    if (lgstradr2 < 25) //  on peut ecrire sur la colonne connexion
                        printf("                                        %-25s  ", str);
                    else
                        printf("                    %-45s  ", str);
                }
            } else if (errno == ENOTCONN)
                if (lgstradr < 25)
                    printf("%-25c  ", '-');
                else
                    printf("                                        %-25c  ", '-');
            else
                //modif Pascal il faut continuer pour les autres sockets, modif du message d'erreur
            {
                printf("%-25s  ", "-error: disconnected");
                //ERREUR ("getpeername()");
                //return (-1);
            }
            printf("%s  ", "ipv6");
        } else {                /*socket IPV4 */

            lensa = sizeof(struct sockaddr_in);
            if (getsockname(sock[i], (struct sockaddr *)&sa, &lensa)
                == -1) {
                ERREUR("getsockname()");
                return (-1);
            }
            if (sa.sin_port == 0) {

                printf("%-25c  ", '-');
                lgstradr = 0;
            } else {
                if (sa.sin_addr.s_addr == INADDR_ANY)
                    sprintf(str, "*(%d)", ntohs(sa.sin_port));
                else {
                    if (getnameinfo((struct sockaddr *)&sa, len, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD))
                        /* Resolution de nom impossible ou nom trop long */
                    {
                        inet_ntop(sa.sin_family, &(sa.sin_addr), ipstr, sizeof ipstr);
                        sprintf(str, "%s(%d)", ipstr, ntohs(sa.sin_port));
                    }

                    else
                        sprintf(str, "%s(%d)", hbuf, ntohs(sa.sin_port));
                }
                printf("%-25s  ", str);
                if ((lgstradr = strlen(str)) > 25)      // passage à la ligne pour adresse suivante
                    printf("\n");

            }

            lensa = sizeof(struct sockaddr_in);
            c = getpeername(sock[i], (struct sockaddr *)&sa, &lensa);
            if (c == 0) {
                if (getnameinfo((struct sockaddr *)&sa, len, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD))
                    /* Resolution de nom impossible ou nom trop long */
                {
                    inet_ntop(sa.sin_family, &(sa.sin_addr), ipstr, sizeof ipstr);
                    sprintf(str, "%s(%d)", ipstr, ntohs(sa.sin_port));
                } else
                    sprintf(str, "%s(%d)", hbuf, ntohs(sa.sin_port));
                lgstradr2 = strlen(str);
                if (lgstradr < 25)      // passage a la ligne non fait
                {
                    if (lgstradr2 < 25) //affichage normal
                        printf("%-25s  ", str);
                    else        //il faut passer à la ligne pour la deuxieme adresse
                        printf("                    %-45s  ", str);
                } else          //passage a la ligne fait
                {
                    if (lgstradr2 < 25) //  on peut ecrire sur la colonne connexion
                        printf("                                        %-25s  ", str);
                    else
                        printf("                    %-45s  ", str);
                }

            } else if (errno == ENOTCONN) {
                if (lgstradr < 25)
                    printf("%-25c  ", '-');
                else
                    printf("                                        %-25c  ", '-');
            }

            else
                //modif Pascal il faut continuer pour les autres sockets, modif du message d'erreur
            {
                printf("%-25s  ", "-error: disconnected");
                //ERREUR ("getpeername()");
                //return (-1);
            }
            printf("%s  ", "ipv4");
        }
        printf("%c", (FD_ISSET(sock[i], &readfds) ? 'R' : '.'));
        printf("%c", (FD_ISSET(sock[i], &writefds) ? 'W' : '.'));
        printf("%c", (FD_ISSET(sock[i], &exceptfds) ? 'X' : '.'));
        printf("\n");
    }
    return (1);
}

/* Affichage des commandes disponibles ou de la syntaxe des commandes
 *============================================================================
 *
 */

int help_cmd(int argc, char *argv[])
{
    int ind[4];
    int nb, i;
    int select_cmd;
    int cmd_argc;
    char *cmd_argv[20];

    if (argc > 1) {
        while (++argv, --argc) {
            i = check_cmd(argv[0], &cmd_argc, cmd_argv);
            if (i >= 0)
                printf("usage: %s", cmds[i].name);
            else
                printf("*** %s", argv[0]);

            switch (i) {
            case CMD_NOTFOUND:
                printf(": invalid command\n");
                break;

            case CMD_AMBIGUOUS:
                printf(": ambiguous command\n");
                break;

            case CMD_NULL:
                break;

            case CMD_BADQUOTE:
                printf(": invalid quoting\n");
                break;

            default:
                printf(" %s\n", cmds[i].usage);
                break;
            }
        }
        return (0);
    }

    /* affichage de toutes les commandes disponibles */
    ind[0] = 0;
    for (nb = 0, i = 0; i < 4 && cmds[nb].row >= 0; nb++) {
        while (i < cmds[nb].row) {
            ind[++i] = nb;
        }
    }
    printf("AVAILABLE COMMANDS:\n");
    nb++;
    select_cmd = 0;

    while (nb > 0) {
        printf("    ");
        for (i = 0; i < 4; i++) {
            if (cmds[ind[i]].row == i) {
                printf("%c %-16s", cmds[ind[i]].letter, cmds[ind[i]].name);
                ind[i]++;
                nb--;
            } else if (i == 3 && !select_cmd) {
                printf("%-18s", "<id> select. sock.");
                select_cmd = 1;
                nb--;
            } else {
                printf("%-18s", "");
            }
        }
        printf("\n");
    }
    return (1);
}

/* Lecture des eventuels flags sur une ligne de commande
 *============================================================================
 * Les flags sont supprimes de la ligne apres traitement.
 *
 */

int check_flags(int argc, char *argv[], t_flg flgs[], int nb_flgs)
{
    char **new_argv;
    int new_argc;
    int i;

    new_argv = argv;
    new_argc = 0;

    while (argc) {
        if (*argv[0] != '-') {
            *new_argv = *argv;
            new_argv++;
            new_argc++;
        } else {
            for (i = 0; i < nb_flgs; i++)
                if (!strncmp(*argv + 1, flgs[i].name, strlen(*argv + 1))) {
                    *(flgs[i].var) = 1;
                    break;
                }

            if (i == nb_flgs) {
                *new_argv = *argv;
                new_argv++;
                new_argc++;
            }
        }
        argc--;
        argv++;
    }

    return (new_argc);
}

/* Suppression des eventuels flags d'une ligne de commande
 *===========================================================================
 *
 */

int remove_flags(int argc, char *argv[])
{
    char **new_argv;
    int new_argc;
    new_argv = argv;
    new_argc = 0;

    while (argc) {
        if (*argv[0] != '-') {
            *new_argv = *argv;
            new_argv++;
            new_argc++;
        }
        argc--;
        argv++;
    }

    return (new_argc);
}

/* Recherche d'une commande dans la table
 *=================================================================
 * La fonction retourne l'indice de la commande ou un code d'erreur.
 * Le tableau argv[] est rempli en sorti si une commande a ete trouvee.
 * cmd : Ligne de commande
 * argc : Nb de mots dans la ligne
 * argv : Commande + parametres
 */

int check_cmd(char cmd[], int *argc, char *argv[])
{
    char *ptr;
    int i, nb, ind;
    int word;
    int quoting;

    /* decoupage de la commande en argc/argv */
    word = isspace(cmd[0]);
    quoting = 0;
    for (*argc = 0, ptr = cmd; *ptr != EOS; ptr++)
        if (isspace(*ptr)) {
            if (!quoting && word) {
                *ptr = EOS;
                word = 0;
            }
        } else {
            if (*ptr == '\"') {
                if (!quoting) {
                    if (word) {
                        *ptr = EOS;
                        word = 0;
                    }
                    argv[(*argc)++] = ptr + 1;
                    quoting = 1;
                } else {
                    *ptr = EOS;
                    quoting = 0;
                }
            } else if (!quoting && !word) {
                argv[(*argc)++] = ptr;
                word = 1;
            }
        }

    if (*argc == 0)
        return (CMD_NULL);

    if (quoting)
        return (CMD_BADQUOTE);

    /* passage du premier argument en minuscule */
    for (i = 0; argv[0][i] != EOS; i++)
        argv[0][i] = my_tolower(argv[0][i]);

    /* recherche de la commande */
    if (strlen(argv[0]) == 1) {
        for (i = nb = ind = 0; cmds[i].name[0] != 0 && nb == 0; i++)
            if (argv[0][0] == cmds[i].letter)
                ind = i, nb = 1;
    } else {
        for (i = nb = ind = 0; cmds[i].name[0] != 0 && nb < 2; i++)
            if (!strcmp(argv[0], cmds[i].name)) {
                ind = i, nb = 1;
                break;
            } else if (!strncmp(argv[0], cmds[i].name, strlen(argv[0])))
                ind = i, nb++;
    }

    /* resultat... */
    switch (nb) {
    case 0:
        return (CMD_NOTFOUND);
    case 1:
        return (ind);
    default:
        return (CMD_AMBIGUOUS);
    }
}

/* Support de la completion
 *=================================================================
 *
 */
char *socklab_command_generator(const char *text, int state)
{
    static int list_index, len;
    char letter;
    char *name;

    /* If this is a new word to complete, initialize now.  This includes
       saving the length of TEXT for efficiency, and initializing the index
       variable to 0. */
    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    /* Return the next name which partially matches from the command list. */
    for (; cmds[list_index].name[0] != 0; list_index++) {
        name = cmds[list_index].name;
        letter = cmds[list_index].letter;
        /* If the user entered a single letter, try to autocomplete according
           to the shortcut. */
        if (strncmp(name, text, len) == 0 || (len == 1 && letter == text[0])) {
            list_index++;
            return strdup(name);
        }
    }

    /* If no names matched, then return NULL. */
    return (char *)NULL;
}

char **socklab_completion(const char *text, int start, int end)
{
    char **matches = NULL;

    /* Don't let readline complete arguments as filenames. */
    rl_attempted_completion_over = 1;

    /* Only complete the first word of a line (command name) */
    if (start == 0)
        matches = rl_completion_matches(text, socklab_command_generator);

    return matches;
}

/* Interface Homme-Machine
 *=================================================================
 *
 */

void ihm()
{
    int choix;
    /*char        cmd[100]; */
    char *cmd;
    int cmd_argc;
    char *cmd_argv[MAX_CMD_ARGC];

    switch (sigsetjmp(ihm_env, 1)) {
    case -1:
        ERREUR("sigsetjmp()");
        break;
    case 0:
        break;
    case 1:
        printf("\n\n");
        break;
    }

    rl_attempted_completion_function = socklab_completion;

    for (;;) {
        cmd = readline(cmd_prompt);
        if (cmd == NULL) {
            printf("\n");
            return;
        }
        if (cmd && *cmd)
            add_history(cmd);

        choix = check_cmd(cmd, &cmd_argc, cmd_argv);
        switch (choix) {
        case CMD_NOTFOUND:
            printf("*** invalid command\n\n");
            break;

        case CMD_AMBIGUOUS:
            printf("*** ambiguous command\n\n");
            break;

        case CMD_NULL:
            break;

        case CMD_BADQUOTE:
            printf("*** invalid quoting\n\n");
            break;

        default:
            cmds[choix].fonc(cmd_argc, cmd_argv);
            printf("\n");
            break;
        }
        free(cmd);
    }
}

/* Usage
 *===========================================================================
 *
 */

void usage()
{
    printf("usage: socklab [tcp|udp|-v]\n");
    exit(1);
}

void print_version()
{
    printf("%s\n", socklab_version);
}

/* Replace \[ and \] by readline ignore delimiters (bash-like)
 * Not robust to malformed prompt strings
 */
char *make_prompt(char *str)
{
    char *prompt = malloc(strlen(str));
    char *p = prompt;
    char c;
    int skip = 0;
    if (prompt == NULL)
        err(EX_OSERR, "malloc prompt");
    while ((c = *str++)) {
        if (c == '\\') {
            c = *str++;
            switch (c) {
#ifdef OSX_READLINE
            case '[':
                skip = 1;
                break;
            case ']':
                skip = 0;
                break;
#else
            case '[':
                *p++ = RL_PROMPT_START_IGNORE;
                break;
            case ']':
                *p++ = RL_PROMPT_END_IGNORE;
                break;
#endif
            }
        } else {
            if (skip)
                continue;
            *p++ = c;
        }
    }
    *p = '\0';
    return prompt;
}

/* MAIN()
 *===================================================================
 *
 */

int main(int argc, char *argv[])
{
    struct sigaction sa;

    if (argc > 2)
        usage();

    exec_mode = DFT;
    cmds = cmds_STD;

    if (argc == 2) {
        if (!strcmp(argv[1], "tcp")) {
            cmds = cmds_TCP;
            exec_mode = TCP;
        } else if (!strcmp(argv[1], "udp")) {
            cmds = cmds_UDP;
            exec_mode = UDP;
        } else if (!strcmp(argv[1], "-v")) {
            print_version();
            exit(1);
        } else
            usage();
    }
#define START_BOLD "\\[\033[1m\\]"
#define END_BOLD   "\\[\033[0m\\]"
    switch (exec_mode) {
    case DFT:
        cmd_prompt = make_prompt(START_BOLD "socklab> " END_BOLD);
        break;
    case TCP:
        cmd_prompt = make_prompt(START_BOLD "socklab-TCP> " END_BOLD);
        break;
    case UDP:
        cmd_prompt = make_prompt(START_BOLD "socklab-UDP> " END_BOLD);
        break;
    }

    nbsock = 0;
    dft_sock = -1;

    /* Setup signal handlers */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = SIGINT_handler;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = SIGIO_handler;
    sigaction(SIGIO, &sa, NULL);
    sa.sa_handler = SIGPIPE_handler;
    sigaction(SIGPIPE, &sa, NULL);
    sa.sa_handler = SIGURG_handler;
    sigaction(SIGURG, &sa, NULL);

    printf("socklab - laboratoire d'etude des sockets INTERNET\n");
    printf("version %s\n", socklab_version);
    printf("-------------------------------------------------------------------------------\n");
    ihm();

    return 0;
}
