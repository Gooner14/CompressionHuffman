#include<stdio.h>
#include<string.h>
#include<malloc.h>
char pad;
unsigned char N;


typedef struct charcode
{ 
	char x;
  	char code[16];
}charcode;

char ext[]=".c";
char dext[]=".un";

charcode *charcodelist;
int n;



int match(char a[],char b[],int limit)
{
	b[strlen(a)]='\0';
	b[limit]='\0';
	return strcmp(a,b);
}

char *int2string(int n)
{
	int i,k,an,j;
	char *temp=(char *)malloc(16*sizeof(char));
	j=0;

	for(i=15;i>=0;i--)
	{
		an=1<<i;
		k=n & an;
		if(k==0) temp[j++]='0'; else temp[j++]='1';
	}
	temp[j]='\0';
	return temp;
}

int k=0;
int buffer=0;
char *decodeBuffer(char b)
{
	int i=0,j=0,t;
	char *decoded=(char *)malloc(16*sizeof(char));

	t=(int)b;
	t=t & 0x00FF;		//mask 
	t=t<<8-k;		 
	buffer=buffer | t;	
	k=k+8;			

	if(pad!=0)	
	{
		buffer=buffer<<pad;
	 	k=8-pad;	
	 	pad=0;
	 }


	while(i<n)
	{
		if(!match(charcodelist[i].code, int2string(buffer),k))
		{	
			decoded[j++]=charcodelist[i].x;	//match found inserted decoded
			t=strlen(charcodelist[i].code);	//matched bits
			buffer=buffer<<t;		//throw out matched bits
			k=k-t;				
			i=0;				//match from initial record
			if(k==0) break;
			continue;
		}
		i++;
	}

	decoded[j]='\0';
	return decoded;

}


int main(int argc,char *argv[])
{
	FILE *fp,*outfile;
	char buffer;
	char *decoded;
	int i;

	if(argc==2)
	{
		argv[2]=(char *)malloc(sizeof(char)*(strlen(argv[1])+strlen(dext)+1));
		strcpy(argv[2],argv[1]);
		strcat(argv[2],dext);
		//argc++;
	}
	else	
		return 0;

	fp=fopen(argv[1],"rb");

	fread(&buffer,1,1,fp);
	n=buffer;		

	printf("\n %d total.",n);

	//allocate memory for mapping table
	charcodelist=(charcode *)malloc(sizeof(charcode)*n);


	fread(charcodelist,sizeof(charcode),n,fp);
		//Read mapping table
	

	for(i=0;i<n;i++)
		printf("[%c %s] ",charcodelist[i].x,charcodelist[i].code);
	

	fread(&buffer,sizeof(char),1,fp);
		pad=buffer;		//No. of bits to discard

	outfile=fopen(argv[2],"wb");

	while(fread(&buffer,sizeof(char),1,fp)!=0)	//Read 1 byte at a time
	{
		decoded=decodeBuffer(buffer);	//decoded is pointer to array of characters read from buffer byte
		i=0;
		while(decoded[i++]!='\0');	
		printf("%s",decoded);
		fwrite(decoded,sizeof(char),i-1,outfile);
	}
	fclose(fp);
	fclose(outfile);
	printf("\nfile %s decompressed successfully.\n",argv[2]); 
	return 0;
}