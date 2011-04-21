/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 * L.CROQ & L.SALI & P. SICARD
 *
 * socklab.c: module principal
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#include "socklab.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

char versionSock[] =
    "socklab.c $Revision: 386 $ du $Date: 2011-04-21 09:31:07 +0200 (Thu, 21 Apr 2011) $ par $Author: rousseau $";

int sock[MAXSOCK];		/* table des sockets gerees */
int nbsock;			/* nb de sockets gerees */
int dft_sock;			/* socket par defaut */
char cmd_prompt[50];		/* prompt de l'interpreteur de commandes */
int exec_mode;			/* mode d'exploitation */
t_cmd *cmds;			/* Commandes disponibles */
jmp_buf ihm_env;		/* pour retourner a l'ihm apres un ctrl-C */

int help_cmd();
int sock_status();
int terminaison();

/* Commandes par defaut */

t_cmd cmds_STD[] = {
	{"socket", 0, 's', socket_call, "[tcp|udp]"},
	{"bind", 0, 'b', bind_call, "[id] [host] [port]"},
	{"listen", 0, 'l', listen_call, "[id] [nb]"},
	{"accept", 0, 'a', accept_call, "[id] "},
	{"connect", 0, 'c', connect_call, "[id] [host] [port]"},
	{"close", 0, 'k', close_call, "[id] "},
	{"shutdown", 0, 'h', shutdown_call, "[id] [in|out|both]"},
	{"options", 0, 'o', socket_options, "[id] [nom]..."},
	{"read", 1, 'r', read_call, "[id] [nb]"},
	{"recv", 1, 'v', recv_call, "[id] [nb] [-oob] [-peek]"},
	{"recvfrom", 1, 'f', recvfrom_call, "[id] [nb] [-oob] [-peek]"},
	{"write", 2, 'w', write_call, "[id] [mesg]"},
	{"send", 2, 'd', send_call, "[id] [mesg] [-oob] [-dontroute] [-loop]"},
	{"sendto", 2, 't', sendto_call,
	 "[id] [host] [port] [mesg] [-oob] [-dontroute] [-loop]"},
	{"help", 3, '?', help_cmd, "[id] [cmd]..."},
	{"status", 3, '=', sock_status, ""},
	{"quit", 3, 'q', terminaison, ""},
	{"exit", 3, 'x', terminaison, ""},
	{"", -1, ' ', 0}
};

/* Commandes en mode udp */

/* cc96-2 */

t_cmd cmds_UDP[] = {
	{"socket", 0, 's', UDP_socket, ""},
	{"msocket", 0, 'c', msocket_call, ""},
	{"close", 0, 'k', close_call, "[id]"},
	{"options", 0, 'o', socket_options, "[id] [nom]..."},
	{"mjoin", 0, 'j', mjoin_call, "[id] [group] [local]"},
	{"mleave", 0, 'l', mleave_call, "[id] [group] [local]"},
	{"mbind", 0, 'b', mbind_call, "[id] [port]"},
	{"recvfrom", 1, 'f', recvfrom_call, "[id] [nb]"},
	{"mrecvfrom", 1, 'r', mrecvfrom_call, "[id] [nb]"},
	{"sendto", 2, 't', sendto_call, "[id] [host] [port] [mesg]"},
	{"msendto", 2, 'm', msendto_call, "[id] [group] [local]"},
	{"bsend", 2, 'a', bsend_call, "[id]"},
	{"help", 3, '?', help_cmd, "[id] [cmd]..."},
	{"status", 3, '=', sock_status, ""},
	{"quit", 3, 'q', terminaison, ""},
	{"exit", 3, 'x', terminaison, ""},
	{"", -1, ' ', 0, ""}
};

/* Commandes en mode tcp */

t_cmd cmds_TCP[] = {
	{"passive", 0, 'p', TCP_passive, ""},
	{"accept", 0, 'a', accept_call, "[id]"},
	{"connect", 0, 'c', TCP_connect, "[host] [port]"},
	{"close", 0, 'k', close_call, "[id]"},
	{"shutdown", 0, 'h', shutdown_call, "[id] [in|out|both]"},
	{"options", 0, 'o', socket_options, "[id] [nom]..."},
	{"read", 1, 'r', read_call, "[id] [nb]"},
	{"urecv", 1, 'v', TCP_urecv, "[id] [nb]"},
	{"write", 2, 'w', write_call, "[id] [mesg]"},
	{"usend", 2, 'd', TCP_usend, "[id] [mesg]"},
	{"help", 3, '?', help_cmd, "[id] [cmd]..."},
	{"status", 3, '=', sock_status, ""},
	{"quit", 3, 'q', terminaison, ""},
	{"exit", 3, 'x', terminaison, ""},
	{"", -1, ' ', 0, ""}
};

/* handler du signal SIGIO
 *=======================================================================
 * SIGIO est recu quand une socket vient de recevoir des donnees et quand
 * celle-ci marche en mode asynchrone.
 */

void SIGIO_handler(sig)
int sig;
{
	signal(SIGIO, SIGIO_handler);
	printf("\n\7*** une socket vient de recevoir des donnees"
	       " ou une demande de connexion\n");
}

/* handler du signal SIGPIPE
 *=======================================================================
 * SIGPIPE est recu quand un processus veut ecrire dans une socket qui a
 * ete fermee "de l'autre cote"
 *
 */

void SIGPIPE_handler(sig)
int sig;
{
	signal(SIGPIPE, SIGPIPE_handler);
	printf("\n\7*** le signal SIGPIPE a ete intercepte\n");
}

/* handler du signal SIGINT
 *=======================================================================
 * SIGINT est genere sur un ctrl-C de l'utilisateur.
 * Ici, on retourne a l'ihm
 */

void SIGINT_handler(sig)
int sig;
{
	signal(SIGINT, SIGINT_handler);
	longjmp(ihm_env, 1);
}

void SIGURG_handler(sig)
int sig;
{
	signal(SIGURG, SIGURG_handler);
	printf("Reception d'un signal SIGURG\n");
}

/* retour au shell
 *=======================================================================
 */

int terminaison()
{
	if (ask("Confirmez-vous le retour au shell") == 0)
		return (0);
	else
		exit(0);
	return (1);
}

/* Affichage de l'etat les sockets creees
 *=======================================================================
 *
 */

int sock_status()
{
	int type;
	socklen_t lentype;
	struct sockaddr_in sa;
	socklen_t lensa;
	struct hostent *hp;
	char str[100];
	int i;
	int c;
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	int maxfdpl;
	int the_ttl;
	struct timeval timeout;

	if (nbsock == 0) {
		printf("Aucune socket creee.\n");
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

	printf
	    (" Id  Proto   Adresse                    Connexion                  RWX ?\n");
	printf
	    (" -----------------------------------------------------------------------\n");

	for (i = 0; i < nbsock; i++) {
		printf("%c%-4d", (i == dft_sock ? '>' : ' '), sock[i]);

		lentype = sizeof(int);
		if (getsockopt(sock[i], SOL_SOCKET, SO_TYPE, (char *)&type,
			       &lentype) < 0) {
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
			if (getsockopt
			    (sock[i], IPPROTO_IP, IP_MULTICAST_TTL,
			     (char *)&the_ttl, &lentype) < 0)
				// Si le TTL a ete modifie, c'est une sock. multicast...
				// C'est le seul indice que j'ai trouve.
				printf
				    ("pb in getsockopt... sock multicast TTL ? \n");
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

		lensa = sizeof(struct sockaddr_in);
		if (getsockname(sock[i], (struct sockaddr *)&sa, &lensa) == -1) {
			ERREUR("getsockname()");
			return (-1);
		}

		if (sa.sin_port == 0)
			printf("%-25c  ", '-');
		else {
			if (sa.sin_addr.s_addr == INADDR_ANY)
				sprintf(str, "*(%d)", ntohs(sa.sin_port));
			else {
				hp = gethostbyaddr((char *)&sa.sin_addr,
						   sizeof(sa.sin_addr),
						   AF_INET);
				if (hp == NULL)
					sprintf(str, "%s(%d)",
						inet_ntoa(sa.sin_addr),
						ntohs(sa.sin_port));
				/*Modif P.Sicard ntoh et %s */

				else
					sprintf(str, "%s(%d)", hp->h_name,
						ntohs(sa.sin_port));
			}
			printf("%-25s  ", str);
		}

		lensa = sizeof(struct sockaddr_in);
		c = getpeername(sock[i], (struct sockaddr *)&sa, &lensa);
		if (c == 0) {
			hp = gethostbyaddr((char *)&sa.sin_addr,
					   sizeof(sa.sin_addr), AF_INET);
			if (hp == NULL)
				sprintf(str, "%s(%d)", inet_ntoa(sa.sin_addr),
					ntohs(sa.sin_port));
			else
				sprintf(str, "%s(%d)", hp->h_name,
					ntohs(sa.sin_port));
			printf("%-25s  ", str);
		} else if (errno == ENOTCONN)
			printf("%-25c  ", '-');
		else
			//modif Pascal il faut continuer pour les autres sockets, modif du message d'erreur
		{
			printf("%-25s  ", "-erreur deconnecte");
			//ERREUR ("getpeername()");
			//return (-1);
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

int help_cmd(argc, argv)
int argc;
char *argv[];
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
				printf(": commande incorrecte\n");
				break;

			case CMD_AMBIGUOUS:
				printf(": commande ambigue\n");
				break;

			case CMD_NULL:
				break;

			case CMD_BADQUOTE:
				printf(": guillemet non ferme\n");
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
	for (nb = 0, i = 0; i < 4 && cmds[nb].row >= 0; nb++)
		while (i < cmds[nb].row)
			ind[++i] = nb;

	printf("LISTE DES COMMANDES DISPONIBLES:\n");
	nb++;
	select_cmd = 0;

	while (nb > 0) {
		printf("    ");
		for (i = 0; i < 4; i++)
			if (cmds[ind[i]].row == i) {
				printf("%c %-16s", cmds[ind[i]].letter,
				       cmds[ind[i]].name);
				ind[i]++;
				nb--;
			} else if (i == 3 && !select_cmd) {
				printf("%-18s", "<id> select. sock.");
				select_cmd = 1;
				nb--;
			} else
				printf("%-18s", "");
		printf("\n");
	}
	return (1);
}

/* Lecture des eventuels flags sur une ligne de commande
 *============================================================================
 * Les flags sont supprimes de la ligne apres traitement.
 *
 */

int check_flags(argc, argv, flgs, nb_flgs)
int argc;
char *argv[];
t_flg flgs[];
int nb_flgs;
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
				if (!strncmp(*argv + 1, flgs[i].name,
					     strlen(*argv + 1))) {
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

int remove_flags(argc, argv)
int argc;
char *argv[];
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
 *
 */

int check_cmd(cmd, argc, argv)
char cmd[];			/* Ligne de commande */
int *argc;			/* Nb de mots dans la ligne */
char *argv[];			/* Commande + parametres */
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
			} else
			    if (!strncmp(argv[0], cmds[i].name,
					 strlen(argv[0])))
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

	switch (setjmp(ihm_env)) {
	case -1:
		ERREUR("setjmp()");
		break;
	case 0:
		signal(SIGINT, SIGINT_handler);
		break;
	case 1:
		printf("\n\n");
		break;
	}

	rl_bind_key('\t', rl_insert);

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
			printf("*** commande incorrecte\n\n");
			break;

		case CMD_AMBIGUOUS:
			printf("*** commande ambigue\n\n");
			break;

		case CMD_NULL:
			break;

		case CMD_BADQUOTE:
			printf("*** guillemet non ferme\n\n");
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
	printf("%s\n", versionSock);
	printf("%s\n", versionPrim);
	printf("%s\n", versionTcp);
	printf("%s\n", versionUdp);
}

/* MAIN()
 *===================================================================
 *
 */

int main(argc, argv)
int argc;
char *argv[];
{
	if (argc > 2)
		usage();

	exec_mode = DFT;
	cmds = cmds_STD;
	strlcpy(cmd_prompt, "[1msocklab>[0m ", sizeof(cmd_prompt));

	if (argc == 2) {
		if (!strcmp(argv[1], "tcp")) {
			cmds = cmds_TCP;
			strlcpy(cmd_prompt, "[1msocklab-TCP>[0m ", sizeof(cmd_prompt));
			exec_mode = TCP;
		} else if (!strcmp(argv[1], "udp")) {
			cmds = cmds_UDP;
			strlcpy(cmd_prompt, "[1msocklab-UDP>[0m ", sizeof(cmd_prompt));
			exec_mode = UDP;
		} else if (!strcmp(argv[1], "-v")) {
			print_version();
			exit(1);
		} else
			usage();
	}
	nbsock = 0;
	dft_sock = -1;
	signal(SIGIO, SIGIO_handler);
	signal(SIGPIPE, SIGPIPE_handler);

	printf("socklab - laboratoire d'etude des sockets INTERNET\n");
	printf
	    ("-------------------------------------------------------------------------------\n");
	ihm();

	return 0;
}
