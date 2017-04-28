/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * socklab.h: fichier entete principal - configuration de socklab
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_SOCKLAB_H_
#define	_SOCKLAB_H_

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/param.h>
#include <errno.h>
#include <err.h>
#include <sysexits.h>
#include <sys/time.h>
#include <setjmp.h>
#include <memory.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "prim.h"
#include "tools.h"
#include "options.h"
#include "tcp.h"
#include "udp.h"

/* Quick and dirty hack : Attention, nous n'utilisons pas la valeur de retour de strlcpy !
 * peut-etre qu'un truc comme ca serait mieux
size_t strlcpy(char *dst, char *src, size_t len) {
	char *r = strncpy(dst, src, len);
	dst[len - 1] = '\0';
	return strlen(src);
}
*/
#ifdef __linux__
#define strlcpy(dst, src, len) strncpy(dst, src, len); dst[len-1] = '\0'
#endif

#ifndef INADDR_NONE
#define INADDR_NONE		0xffffffff
#endif

#ifndef EOS
#define EOS		'\0'
#endif

/* Macro d'affichage en vidéo inverse, Attention: non portable */
#define VIDEO_INV_ERR(code) 	fprintf(stderr, "\07\033[7m"); code; fprintf(stderr, "\033[0m")

/* Parametrage de l'application */

#define MAXSOCK		30      /* nb max de sockets gerees */
#define MAX_BUFFER	1000+1  /* Taille maximale des messages envoyes + 1 pour le '\0' de fin de chaine */

#define CMD_LENGTH	10      /* longueur max. d'une commande */
#define MAX_CMD_ARGC	20      /* nb max. de param dans une commande */
#define MAX_FLGNAME	10      /* longueur max. d'un flag */
#define MAX_ITEMNAME	10      /* longueur max. d'un choix d'une liste */
#define MAX_HOSTNAME	50      /* longueur max. d'un nom de machine attention avec IPV6 plus long */
#define MAX_PORTNAME	30      /* longueur max. d'un id. de port */
#define MAX_SONAME	30      /* longueur max. d'un nom d'option de sock. */
#define MAX_USAGELEN	70      /* longueur max. d'un message d'aide */

/* TTL utilise en multicast*/

#define TTL_MCAST        16

/* Etats possibles d'une socket: */

#define CREEE	  	0
#define NOMMEE		1
#define ECOUTE		2
#define CONNECTEE	3

/* Compte rendu d'une recherche de commande */

#define CMD_NOTFOUND	-1
#define CMD_AMBIGUOUS	-2
#define CMD_NULL	-3
#define CMD_BADQUOTE	-4

/* commandes de l'interpreteur */

typedef struct {
    char name[CMD_LENGTH];      /* nom */
    int row;                    /* colonne d'affichage (help) */
    char letter;                /* raccourci */
    int (*fonc) ();             /* fonction correspondante */
    char usage[MAX_USAGELEN + 1];       /* message d'aide */
} t_cmd;

/* flags... */

typedef struct {
    char name[MAX_FLGNAME + 1]; /* nom du drapeau */
    int *var;                   /* valeur (par adresse) */
} t_flg;

/* listes de choix... */

typedef struct {
    char name[MAX_ITEMNAME + 1];        /* nom du choix */
    int val;                    /* valeur de selection */
} t_item;

/* options des sockets: */

typedef struct {
    char name[MAX_SONAME + 1];  /* nom */
    int code;                   /* identificateur */
    int type;                   /* type (cf. plus loin) */
} t_sockopt;

/* valeurs possibles pour le type d'une option: */

#define BOOL_OPT	1
#define INT_OPT		2
#define ASYNC_OPT	3
#define NODELAY_OPT	4
#define TIME_OPT	5

/* Modes de fonctionnement possibles: */

#define DFT		0       /* defaut */
#define TCP		1       /* tcp */
#define UDP		2       /* udp */

/* Modes de selection des Id de sockets: */

#define IMPLICIT	0
#define EXPLICIT	1

/* #define divers */

#define DFT_MODE	(exec_mode==DFT)        /* mode par defaut ? */
#define id_sock		(sock[dft_sock])        /* id. socket par defaut */
#define my_toupper(c)	((!isalpha(c) || isupper(c)) ? (c) : toupper(c))
#define my_tolower(c)	((!isalpha(c) || islower(c)) ? (c) : tolower(c))

/* Variables globales de socklab */

extern int sock[];              /* table des sockets creees */
extern int nbsock;              /* nb de sockets creees */
extern int dft_sock;            /* socket par defaut */
extern int exec_mode;           /* mode d'exploitation */
extern int id_mode;             /* mode de selection des id. */

/* Fonctions du module socklab.c: */

void SIGIO_handler();
void SIGPIPE_handler();
void SIGINT_handler();
void SIGURG_handler();
int terminaison();
int sock_status();
int help_cmd();
int check_flags();
int remove_flags();
int check_cmd();
void ihm();
void usage();
void print_version();
#endif                          /* _SOCKLAB_H_ */
