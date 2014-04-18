#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define PORTNO 5580
#define LEN 10000
unsigned char buffer[LEN];
struct tcpheader {
 uint16_t srcport;
 uint16_t destport;
 unsigned int seqnum;
 unsigned int acknum;
 unsigned char offset:4,reserved:4;
 uint8_t flags;
 uint16_t win;
 uint16_t chksum;
 uint16_t urgptr;
 }*t_head;
unsigned char *buf,*dest_ip;

unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--) sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void TCP_header()
{
    t_head=(struct tcpheader *)malloc(sizeof(struct tcpheader));
    t_head->srcport=PORTNO;
    t_head->destport=PORTNO;
    t_head->offset=5;
    t_head->reserved=0;
    t_head->urgptr=0;
    t_head->seqnum=rand()%1024;
    t_head->acknum=0;
    t_head->win=2048;
    t_head->flags=2;
    t_head->chksum=0;
}

void TCP_header_init()
{
    struct tcpheader *tcp_head=(struct tcpheader *)buffer;
    tcp_head->srcport=t_head->srcport;
    tcp_head->destport=t_head->destport;
    tcp_head->offset=t_head->offset;
    tcp_head->reserved=0;
    tcp_head->urgptr=0;
    tcp_head->seqnum=t_head->seqnum;
    tcp_head->acknum=t_head->acknum;
    tcp_head->win=2048;
    tcp_head->flags=t_head->flags;
    tcp_head->chksum=0;
    tcp_head->chksum=csum((unsigned short *)buffer, sizeof(struct tcpheader));
}

int recvtimeout(int s, char *buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s+1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return recv(s, buf, len, 0);
}

void send_init(unsigned char *add)
{
    int s=sizeof(struct tcpheader);
    struct sockaddr_in senaddr,recaddr;
    struct tcpheader *res=malloc(sizeof(struct tcpheader));
    unsigned char *r=(unsigned char *)res;
    int socksd,sendsd,dl,length;
    TCP_header();
    buf=(unsigned char *)t_head;
    socksd=socket(AF_INET,SOCK_STREAM,0);
    memset(&senaddr,0,sizeof(struct sockaddr_in));
    senaddr.sin_family=AF_INET;
    senaddr.sin_port=htons(PORTNO);
    inet_pton(AF_INET,add,&senaddr.sin_addr);
    bind(socksd,(struct sockaddr *)&senaddr,sizeof(struct sockaddr_in));
    connect(socksd,(struct sockaddr *)&senaddr,sizeof(struct sockaddr_in));
    if(send(socksd,buf,s,0))
    printf("SYN sent\n");       //SYN
    bzero(r, sizeof(r));
    dl=recvtimeout(socksd,r,s,2);

    if(dl==-1)
    {
        printf("SYN + ACK error\n");
        exit(-1);
    }
    else if(dl==-2)
    {
        printf("SYN + ACK Timeout\n");
        exit(-1);
    }
    else if(res->acknum==t_head->seqnum+1 && res->flags==18)
    printf("SYN + ACK received\n");
    else
    {
        printf("Connection ERROR\n");
        exit(-1);
    }
    res->acknum=res->seqnum+1;
    res->flags-=2;
    if(send(socksd,r,s,0))
    printf("ACK sent\n");

    shutdown(socksd,2);
    close(socksd);
    t_head->seqnum+=1;
    t_head->flags=0;

    FILE *in,*out;
    in = fopen("application_out","r");
    if(!in)
    {
        printf("Input file error\n");
        exit(-1);
    }
    unsigned char * data = (unsigned char *)(sizeof(struct tcpheader)+buffer);
    int total = fread(data,1,LEN-s,in);
    TCP_header_init();

    total+=s;
    out = fopen("transport_out","wb");
    if(!out)
    {
        printf("Output file error\n");
        exit(-1);
    }
    fwrite(buffer,1,total,out);
    fclose(in);
    fclose(out);
}

void receive_init()
{

    int length=sizeof(struct sockaddr_in);
    struct tcpheader *rec=malloc(sizeof(struct tcpheader));
    unsigned char *r=(unsigned char *)rec;
    int nwords=sizeof(struct tcpheader);
    struct sockaddr_in recaddr,sendaddr;
    int recsd,sendsd,dl,seq;
    recsd=socket(AF_INET,SOCK_STREAM,0);
    memset(&recaddr,0,sizeof(struct sockaddr_in));
    recaddr.sin_family=AF_INET;
    recaddr.sin_port=htons(PORTNO);
    bind(recsd,(struct sockaddr *)&recaddr,sizeof(struct sockaddr_in));
    listen(recsd,1);

    sendsd=accept(recsd,(struct sockaddr *)&sendaddr,&length);
    dl=recvtimeout(sendsd,r,nwords,2);     //SYN
    if(dl==-1 || dl==0)
    {
        printf("SYN error\n");
        exit(-1);
    }
    else if(dl==-2)
    {
        printf("SYN Timeout\n");
        exit(-1);
    }
    else if(rec->flags==2)
    printf("SYN received\n");
    else
    {
        printf("Connection ERROR\n");
        exit(-1);
    }
    rec->acknum=rec->seqnum+1;
    rec->seqnum=rand()%1024;
    rec->flags=18;
    seq=rec->seqnum;
    if(send(sendsd,r,nwords,10))     //SYN + ACK
    printf("SYN + ACK sent\n");
    dl=recvtimeout(sendsd,r,nwords,2);     //ACK

    if(dl==-1 || dl==0)
    {
        printf("ACK error\n");
        exit(-1);
    }
    else if(dl==-2)
    {
        printf("ACK Timeout\n");
        exit(-1);
    }
    else if(rec->flags==16 && rec->acknum==seq+1)
    printf("ACK received\n3 way handshake established\n");
    else
    {
        printf("Connection ERROR\n");
        exit(-1);
    }
    shutdown(recsd,2);
    close(recsd);
}

void receiver()
{
    printf("Removing TCP header..\n");
    memset(buffer, 0, LEN);
    FILE *in, *out;
    out = fopen("application_in","wb");
    in = fopen("transport_in","r");
    unsigned char * data = (unsigned char *)(sizeof(struct tcpheader)+buffer);
    int total = fread(buffer, 1, LEN, in);
    total -= sizeof(struct tcpheader);

    if(csum((unsigned short*)buffer, sizeof(struct tcpheader))==0)
    {
        fwrite(data,1,total,out);
    }
    else
    {
        printf("TCP Header Error - CheckSum..!!\n");
        exit(-1);
    }

    fclose(in);
    fclose(out);
}

int main(int argc, char * argv[])
{
    srand(time(NULL));
    printf("Transport Layer ...\n");
    dest_ip = argv[2];
    if(argv[1][0]=='1') send_init(argv[2]);
    else if(argv[1][0]=='3') receive_init();
    else if(argv[1][0]=='2')receiver();
    else {printf("Wrong Agruments.....!!\nAborting....!!\n");exit(-1);}
    printf("Transport Layer exiting....!!\n");
    return 0;
}
