#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "ifaddrs.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

void
freeifaddrs(struct ifaddrs* ifp)
{
  if (ifp != NULL) {
    free(ifp->ifa_name);
    free(ifp->ifa_addr);
    free(ifp->ifa_netmask);
    free(ifp->ifa_dstaddr);
    freeifaddrs(ifp->ifa_next);
    free(ifp);
  }
}

static struct sockaddr*
sockaddr_dup(struct sockaddr* sa)
{
  struct sockaddr* ret;
  socklen_t socklen;
  socklen = sa->sa_len;
  ret = calloc(1, socklen);

  if (ret == NULL)
    return NULL;

  memcpy(ret, sa, socklen);

  return ret;
}

int
getifaddrs(struct ifaddrs** ifap)
{
  char buff[8192];
  int fd4, fd6, i;
  struct ifconf ifc;
  struct ifreq* ifr = NULL;
  struct ifaddrs *curif, *curif2;
  struct ifaddrs* lastif = NULL;

  *ifap = NULL;

  if ((fd4 = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    return -1;
  }

  if ((fd6 = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
    return -1;
  }

  ifc.ifc_len = sizeof(buff);
  ifc.ifc_buf = buff;

  if (ioctl(fd4, SIOCGIFCONF, &ifc) != 0) {
    close(fd4);
    close(fd6);
    return -1;
  }

  ifr = ifc.ifc_req;

  i = ifc.ifc_len;

  while (i > 0) {
    unsigned int inc;

    inc = ifr->ifr_addr.sa_len;

    if (ioctl(fd4, SIOCGIFADDR, ifr) != 0) {
      goto caseV6;
    }

    curif = calloc(1, sizeof(struct ifaddrs));
    curif->ifa_name = strdup(ifr->ifr_name);
    curif->ifa_addr = sockaddr_dup(&ifr->ifr_addr);
    curif->ifa_dstaddr = NULL;
    curif->ifa_data = NULL;
    curif->ifa_netmask = NULL;
    curif->ifa_next = NULL;

    if (ioctl(fd4, SIOCGIFNETMASK, ifr) != 0) {
      goto caseV6;
    }

    if (ioctl(fd4, SIOCGIFFLAGS, ifr) != 0) {
      goto caseV6;
    }

    if (lastif == NULL) {
      *ifap = curif;
    } else {
      lastif->ifa_next = curif;
    }

    curif->ifa_flags = ifr->ifr_flags;
    curif->ifa_netmask = sockaddr_dup(&ifr->ifr_addr);
    lastif = curif;

  caseV6:
    if (ioctl(fd6, SIOCGIFADDR6, ifr) != 0) {
      goto next;
    }

    curif2 = calloc(1, sizeof(struct ifaddrs));
    curif2->ifa_name = strdup(ifr->ifr_name);
    curif2->ifa_addr = sockaddr_dup(&ifr->ifr_addr);
    curif2->ifa_dstaddr = NULL;
    curif2->ifa_data = NULL;
    curif2->ifa_netmask = NULL;
    curif2->ifa_next = NULL;

    if (ioctl(fd6, SIOCGIFNETMASK6, ifr) != 0) {
      goto next;
    }

    if (ioctl(fd6, SIOCGIFFLAGS, ifr) != 0) {
      goto next;
    }

    if (lastif == NULL) {
      *ifap = curif2;
    } else {
      lastif->ifa_next = curif2;
    }

    curif2->ifa_flags = ifr->ifr_flags;
    curif2->ifa_netmask = sockaddr_dup(&ifr->ifr_addr);

    lastif = curif2;

  next:
    if (inc < sizeof(ifr->ifr_addr))
      inc = sizeof(ifr->ifr_addr);

    inc += IFNAMSIZ;

    ifr = (struct ifreq*)(((char*)ifr) + inc);
    i -= inc;
  }

  close(fd4);
  close(fd6);

  return 0;
}
