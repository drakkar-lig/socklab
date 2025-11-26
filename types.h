
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
    int (*fonc) (int, char **);             /* fonction correspondante */
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
