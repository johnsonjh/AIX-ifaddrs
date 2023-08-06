#ifndef AIX_IFADDRS_H
# define AIX_IFADDRS_H

# ifdef __cplusplus
extern "C"
{
# endif

# include <sys/socket.h>
# include <net/if.h>
# include <netinet/in6_var.h>

# undef ifa_dstaddr
# undef ifa_broadaddr

  struct ifaddrs
  {
    struct ifaddrs* ifa_next;     /* Next item in list */
    char* ifa_name;               /* Name of interface */
    unsigned int ifa_flags;       /* Flags from SIOCGIFFLAGS */
    struct sockaddr* ifa_addr;    /* Address of interface */
    struct sockaddr* ifa_netmask; /* Netmask of interface */
    union
    {
      struct sockaddr* ifu_broadaddr;
      /* Broadcast address of interface */
      struct sockaddr* ifu_dstaddr;
      /* Point-to-point destination address */
    } ifa_ifu;
# define ifa_broadaddr ifa_ifu.ifu_broadaddr
# define ifa_dstaddr ifa_ifu.ifu_dstaddr
    void* ifa_data; /* Address-specific data */
  };

# ifdef __cplusplus
  extern "C"
  {
# endif
    /* Create a linked list of `struct ifaddrs' structures, one for each
    network interface on the host machine.  If successful, store the
    list in *IFAP and return 0.  On errors, return -1 and set `errno'.

    The storage returned in *IFAP is allocated dynamically and can
    only be properly freed by passing it to `freeifaddrs'.  */
    extern int getifaddrs(struct ifaddrs**);

    /* Reclaim the storage allocated by a previous `getifaddrs' call.  */
    extern void freeifaddrs(struct ifaddrs*);
# ifdef __cplusplus
  }
# endif

# ifdef __cplusplus
}
# endif

#endif // AIX_IFADDRS_H
