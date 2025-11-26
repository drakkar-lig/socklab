/*
 * SOCKLAB - "laboratoire" d'etude des sockets internet - ENSIMAG - Juillet 93
 *
 * tools.h
 *
 * sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt
 */

#ifndef	_TOOLS_H_
#define	_TOOLS_H_

int host_addr(char host[], struct sockaddr *addr, int version);
int port_number(char name[], int *port);
void get_host(char name[], struct sockaddr *addr, int version);
void get_itf(char name[], struct sockaddr *addr, int version);
void get_group(char name[], u_long *addr);
void get_port(char name[], int *port);
void get_msg(char **msg, int *size);
void get_nb(char prompt[], char init[], int *nb);
void get_choice(char prompt[], t_item list[], int nb, char init[], int *choice);
void get_bool(char prompt[], int *var);
void get_id_sock(char init[], int *so);
int ask(char prompt[]);
void display_inet_addr(struct sockaddr *addr);
void ERREUR(char msg[]);
int domainesock(int socket);

#endif                          /* _TOOLS_H_ */
