#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<limits.h>
#include<unistd.h>
#include<sys/socket.h>
#define PCKT_LEN 10000000
struct ipheader {
 unsigned char      iph_hl:5, iph_ver:4;
 unsigned char      iph_tos;
 unsigned short int iph_len;
 unsigned short int iph_ident;
 unsigned char      iph_flag;
 unsigned short int iph_offset;
 unsigned char      iph_ttl;
 unsigned char      iph_protocol;
 unsigned short int iph_chksum;
 unsigned int       iph_sourceip;
 unsigned int       iph_destip;
};
char * dest_ip_str;
unsigned char buffer[PCKT_LEN];
unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--) sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}
void ipheader_init()
{
    struct ipheader *ip = (struct ipheader*) buffer;
    ip->iph_hl = 5; //number of 32 bit int
    ip->iph_ver = 4; //version IPv4, IPv6
    ip->iph_tos = 16; //Type of service
    ip->iph_len = sizeof(struct ipheader); //length of IP Header
    ip->iph_ident = htons(54321); //used for fragmentation
    ip->iph_ttl = 64; // hops made
    ip->iph_protocol = 6; //Demux Key, TCP=6, UDP=17

    //Get IP of source
    ip->iph_sourceip = inet_addr("10.20.30.40");
    // The destination IP address
    ip->iph_destip = inet_addr(dest_ip_str);
    ip->iph_chksum = 0;
    ip->iph_chksum = csum((unsigned short *)buffer, sizeof(struct ipheader));
}
void sender()
{
    printf("Adding IP Header .......\n");
    memset(buffer, 0, PCKT_LEN);
    FILE *in,*out;
    in = fopen("transport_out","r");
    out = fopen("network_out","wb");
    unsigned char * data = (unsigned char *)(sizeof(struct ipheader)+buffer);
    int total = fread(data,1,PCKT_LEN-sizeof(struct ipheader),in);
    ipheader_init();
    total += sizeof(struct ipheader);

    fwrite(buffer,1,total,out);
    fclose(in);
    fclose(out);
}
void receiver()
{
    printf("Decoding IP Header .........\n");
    memset(buffer, 0, PCKT_LEN);
    FILE *in, *out;
    out = fopen("transport_in","wb");
    in = fopen("network_in","r");
    unsigned char * data = (unsigned char *)(sizeof(struct ipheader)+buffer);
    int total = fread(buffer, 1, PCKT_LEN, in);
    total -= sizeof(struct ipheader);

    if(csum((unsigned short*)buffer, sizeof(struct ipheader))==0)
    {
        fwrite(data,1,total,out);
    }
    else
    {
        printf("IP Header Error - CheckSum...!!\nAborting.....!");
        exit(-1);
    }

    fclose(in);
    fclose(out);
}
int main(int argc, char * argv[])
{
    printf("In Network Layer ......\n");
    dest_ip_str = argv[2];
    if(argv[1][0]=='1') sender();
    else if(argv[1][0]=='2') receiver();
    else printf("Wrong Agruments.......!!\nAborting.......!!\n"),exit(-1);
    printf("Network Layer Work Done..........!!\n");
    return 0;
}
