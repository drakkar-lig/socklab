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

#include "types.h"
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

void SIGIO_handler(int sig);
void SIGPIPE_handler(int sig);
void SIGINT_handler(int sig);
void SIGURG_handler(int sig);
int terminaison(int argc, char *argv[]);
int sock_status(int argc, char *argv[]);
int help_cmd(int argc, char *argv[]);
int check_flags(int argc, char *argv[], t_flg flgs[], int nb_flgs);
int remove_flags(int argc, char *argv[]);
int check_cmd(char cmd[], int *argc, char *argv[]);
void ihm();
void usage();
void print_version();
#endif                          /* _SOCKLAB_H_ */
