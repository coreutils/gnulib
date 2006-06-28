#include "config.h"
#include "getaddrinfo.h"
#include "inet_ntop.h"
#include <stdio.h>
#include <string.h>

int simple (char *host, char *service)
{
  char buf[BUFSIZ];
  struct addrinfo hints;
  struct addrinfo *ai0, *ai;
  int res;

  printf ("Finding %s service %s...\n", host, service);

  memset (&hints, 0, sizeof (hints));
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  res = getaddrinfo (host, service, 0, &ai0);

  printf ("res %d: %s\n", res, gai_strerror (res));

  if (res != 0)
    return 1;

  for (ai = ai0; ai; ai = ai->ai_next)
    {
      printf ("\tflags %x\n", ai->ai_flags);
      printf ("\tfamily %x\n", ai->ai_family);
      printf ("\tsocktype %x\n", ai->ai_socktype);
      printf ("\tprotocol %x\n", ai->ai_protocol);
      printf ("\taddrlen %d: ", ai->ai_addrlen);
      printf ("\tFound %s\n",
	      inet_ntop (ai->ai_family,
			 &((struct sockaddr_in *)
			  ai->ai_addr)->sin_addr,
			 buf, sizeof (buf) - 1));
      if (ai->ai_canonname)
	printf ("\tFound %s...\n", ai->ai_canonname);

      {
	char ipbuf[BUFSIZ];
	char portbuf[BUFSIZ];

	res = getnameinfo (ai->ai_addr, ai->ai_addrlen,
			   ipbuf, sizeof (ipbuf) - 1,
			   portbuf, sizeof (portbuf) - 1,
			   NI_NUMERICHOST|NI_NUMERICSERV);
	printf ("\t\tgetnameinfo %d: %s\n", res, gai_strerror (res));
	if (res == 0)
	  {
	    printf ("\t\tip %s\n", ipbuf);
	    printf ("\t\tport %s\n", portbuf);
	  }
      }

    }

  freeaddrinfo (ai0);

  return 0;
}

#define HOST1 "www.gnu.org"
#define SERV1 "http"
#define HOST2 "www.ibm.com"
#define SERV2 "https"
#define HOST3 "microsoft.com"
#define SERV3 "http"
#define HOST4 "google.org"
#define SERV4 "ldap"

int main (void)
{
#if _WIN32
  {
    WORD requested;
    WSADATA data;
    int err;

    requested = MAKEWORD (1, 1);
    err = WSAStartup (requested, &data);
    if (err != 0)
      return 1;

    if (data.wVersion < requested)
      {
	WSACleanup ();
	return 2;
      }
  }
#endif

  return simple (HOST1, SERV1)
    + simple (HOST2, SERV2)
    + simple (HOST3, SERV3)
    + simple (HOST4, SERV4);
}
