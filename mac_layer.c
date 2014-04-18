#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define PORTNO 5234
#define POLYNOMIAL 0x04c11db7L
#define PCKT_LEN 10000
unsigned char buffer[PCKT_LEN];
struct frame{
    unsigned char preamble[8],dest[6],src[6];
    unsigned short int len;
    unsigned char data[PCKT_LEN];
    unsigned int crc;
};
static unsigned int crc_table[256];
void gen_crc_table(void)
{
    register unsigned int i, j;
    register unsigned int crc_accum;
    for(i=0;i<256;i++)
    {
        crc_accum = (i<<24);
        for(j=0;j<8;j++)
        {
            if ( crc_accum & 0x80000000L ) crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
            else crc_accum = (crc_accum << 1);
        }
        crc_table[i] = crc_accum;
    }
}
unsigned int calc_crc(unsigned char *data,unsigned int len)
{
    register unsigned int i,j,crc_accum=0;
    for (j=0;j<len;j++)
    {
        i = ((unsigned int) (crc_accum >> 24) ^ data[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }
    return crc_accum;
}
void soc_sender(unsigned char *data,unsigned char *add,int nwords)
{
    struct sockaddr_in senaddr;
    int sockfd;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    memset(&senaddr,0,sizeof(struct sockaddr_in));
    senaddr.sin_family=AF_INET;
    senaddr.sin_port=htons(PORTNO);
    inet_pton(AF_INET,add,&senaddr.sin_addr);

    connect(sockfd,(struct sockaddr *)&senaddr,sizeof(struct sockaddr_in));

    send(sockfd,data,nwords,0);
    shutdown(sockfd,2);
    close(sockfd);
}
int soc_receiver(unsigned char *data,int nwords)
{
    struct sockaddr_in recaddr,senaddr;
    int recfd,senfd,clength,dlength;

    recfd=socket(AF_INET,SOCK_STREAM,0);

    memset(&recaddr,0,sizeof(struct sockaddr_in));
    recaddr.sin_family=AF_INET;
    recaddr.sin_port=htons(PORTNO);

    bind(recfd,(struct sockaddr *)&recaddr,sizeof(struct sockaddr_in));
    listen(recfd,1);

    clength=sizeof(struct sockaddr_in);
    senfd=accept(recfd,(struct sockaddr *)&senaddr,&clength);

    dlength=recv(senfd,data,nwords,0);

    shutdown(recfd,2);
    close(recfd);
    return dlength;
}
void frame_init(struct frame* fr,unsigned char message[], unsigned short int msglen)
{
    int i;
    for(i=0;i<7;i++) fr->preamble[i]=0xAA;
    fr->preamble[7]=0xAB;
    for(i=0;i<msglen;i++) fr->data[i]=buffer[i];
    fr->crc= calc_crc(fr->data,msglen);
    unsigned int tempcrc = fr->crc;
    fr->data[msglen]=tempcrc>>24;
    fr->data[msglen+1]=(tempcrc<<8)>>24;
    fr->data[msglen+2]=(tempcrc<<16)>>24;
    fr->data[msglen+3]=(tempcrc<<24)>>24;
    fr->data[msglen+4]='\0';

    fr->len = msglen+4;

    //Dummy Values for MAC Address
    fr->dest[0]=0x70;
    fr->dest[1]=0xf3;
    fr->dest[2]=0x95;
    fr->dest[3]=0x14;
    fr->dest[4]=0x56;
    fr->dest[5]=0xb7;

    fr->src[0]=0x80;
    fr->src[1]=0xfd;
    fr->src[2]=0x99;
    fr->src[3]=0x54;
    fr->src[4]=0x98;
    fr->src[5]=0xc6;
}
void sender(char * dest_ip_add)
{
    struct frame* send_fr = (struct frame*)malloc(sizeof(struct frame));
    FILE *in;
    in = fopen("network_out","rb");
    unsigned short int total = fread(buffer, 1, PCKT_LEN, in);
    frame_init(send_fr,buffer,total);
    fclose(in);
    //Socket Programming Here. send_fr is the frame to be sent
    soc_sender((unsigned char*)send_fr,dest_ip_add, sizeof(struct frame));
}
void receiver()
{
    struct frame* receive_fr = (struct frame*)malloc(sizeof(struct frame));
    //Socket Programming Here. Store the recived frame in receive_fr
    printf("Waiting for a message..........\n");
    int rr = soc_receiver((unsigned char*)receive_fr, sizeof(struct frame));
    printf("Ta Da.... Message Received.......!!\n");
    //End of Socket Programming Part
    unsigned int crc_rec = calc_crc(receive_fr->data,receive_fr->len);
    if(crc_rec==0)
    {
        printf("CRC Verification Done.......!!\n");
        FILE *out;
        out = fopen("network_in","wb");
        fwrite(receive_fr->data,1,receive_fr->len-4,out);
        fclose(out);
    }
    else
    {
        printf("Received Frame Error!! CRC Check Not Validated.....!!\nAborting!!\n");
        exit(-1);
    }
}
int main(int argc, char * argv[])
{
    int i;
    gen_crc_table();
    if(argc==2)
    {
        printf("Calling Receiver\n");
        receiver();
    }
    else if(argc==3)
    {
        printf("Calling Sender\n");
        sender(argv[2]);
    }
    else
    {
        printf("Wrong Arguments.....!!\nAborting....!!\n");
        exit(-1);
    }
    return 0;
}
