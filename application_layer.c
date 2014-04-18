#define DESCRIPTOR 64
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
struct a_header{
uint8_t des;
uint16_t size;
};
void a_receiver(char *filename)
{
    unsigned char buf[2000];
    struct a_header *tmp=(struct a_header *)buf;
    unsigned char *data=(unsigned char *)(sizeof(struct a_header)+buf);

    FILE *in,*out;
    in=fopen("application_in","r");
    out=fopen(filename,"wb");

    if(in==NULL||out==NULL)
    {
        printf("File Opening error...........!!\nAborting....!!\n");
        exit(-1);
    }
    fread(buf,1,2000,in);
    fwrite(data,1,tmp->size,out);
    fclose(in);
    fclose(out);
}

void a_sender(char *filename)
{
    int total;
    unsigned char buf[2000];
    struct a_header *tmp=(struct a_header *)buf;
    char *data=(unsigned char *)(sizeof(struct a_header)+buf);

    FILE *in,*out;

    in=fopen(filename,"r");
    out=fopen("application_out","wb");

    if(in==NULL||out==NULL)
    {
        printf("File Opening error......!!\nAborting....!!\n");
        exit(-1);
    }

    tmp->des=(uint8_t)DESCRIPTOR;
    tmp->size=fread(data,1,2000,in);

    total=sizeof(struct a_header)+tmp->size;
    buf[total]=EOF;
    total++;

    fwrite(buf,1,total,out);

    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("2 Command Line Arguments Required.......!!\nAborting......!!\n");
        exit(-1);
    }
    else if(argv[1][0]=='1') a_sender(argv[2]);
    else if(argv[1][0]=='2') a_receiver(argv[2]);
    else
    {
        printf("Invalid Commandline Agruments.......!!\nAborting......!!\n");
        exit(-1);
    }
    return 0;
}
