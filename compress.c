#include<stdio.h>
#include<string.h>
#include<malloc.h>
char pad;
unsigned char N;


#define ll long long
const ll INTERNAL= 0;
const ll LEAF= 1;

typedef struct charcode
{ char x;
  char code[16];
}charcode;

char ext[]=".c";
char dext[]=".un";

typedef struct node
{
	char x;
	int freq;
	char *code;
	int type;
	struct node *next;
	struct node *left;
	struct node *right;
}node;

node *HEAD,*ROOT;


node* newNode(char c)
{
	node *q;
	q=(node *)malloc(sizeof(node));
	q->x=c;
	q->type=LEAF;	//leafnode
	q->freq=1;
	q->next=NULL;
	q->left=NULL;
	q->right=NULL;
	return q;
}

char byte=0;
int cnt;


void writeBit(int b,FILE *f)
{	
	char temp;
	if(b==1)
	{	temp=1;
		temp=temp<<(7-cnt);	
		byte=byte | temp;
	}
	cnt++;
	
	if(cnt==8)	
	{
//		printf("[%s]",bitsInChar(byte));
		fwrite(&byte,sizeof(char),1,f);
		cnt=0; byte=0;	//reset buffer
		return;
	}
	return;
}




void writeHeader(FILE *f)
{
	charcode record;
	node *p;
	int temp=0,i=0;
	p=HEAD;
	while(p!=NULL)	
	{
		//printf("%c ", p->x);
		temp+=(strlen(p->code)) * (p->freq);		
		//printf("%ld ", strlen(p->code));
		//printf("%d ", p->freq);
		//printf("%d\n", temp);
		temp%=8;
		//printf("%d\n", temp);
		i++;
		p=p->next;
	}

	N=i;

	fwrite(&N,1,1,f);	//total different char
	printf("\nN=%u",i);

	p=HEAD;
	while(p!=NULL)
	{
		record.x=p->x;
		strcpy(record.code,p->code);
		fwrite(&record,sizeof(charcode),1,f);
		printf("\n%s\n",p->code);
		p=p->next;

	}
	pad=8-temp;	
	fwrite(&pad,sizeof(char),1,f);
	for(i=0;i<pad;i++)
			writeBit(0,f);

}


char *getCode(char ch)
{
node *p=HEAD;
	while(p!=NULL)
	{
	    if(p->x==ch)
		  return p->code;
	    p=p->next;
	}
	return NULL;
}

void writeCode(char ch,FILE *f)
{
	char *code;
	code=getCode(ch);
//printf("\n%s\n",code);
	while(*code!='\0')
	{
		if(*code=='1')
			writeBit(1,f);
		else
			writeBit(0,f);
	code++;
	}
	return;
}





void insert(node *p,node *h)
{ // insert p in list as per its freq.
	if(h->next==NULL)
	{  h->next=p; return;}
	while(h->next->freq < p->freq)
	{  h=h->next;
	  if(h->next==NULL)
	    { h->next=p; return; }
	}
	p->next=h->next;
	h->next=p;
}

void addSymbol(char c)
{// Insert symbols into linked list if its new, otherwise freq++
// Inserts a node in order of its freq
	node *p,*q,*h;
	int t;

	if(HEAD==NULL)
	{	HEAD=newNode(c);
		return;
	}
		p=HEAD; q=NULL;
	if(p->x==c) //item found in HEAD
	{
		p->freq+=1;
		if(p->next==NULL)
			return;
		if(p->freq > p->next->freq)//Change HEAD if frequency of first node becomes more than freq of second 
		{
			HEAD=p->next;
			p->next=NULL;
			insert(p,HEAD);// insert the first node back according to freq
		}
		return;
	}

	while(p->next!=NULL && p->x!=c)
	{
		q=p; p=p->next;
	}

	if(p->x==c)
	{
		p->freq+=1;
	        if(p->next==NULL)
			return;	
		if(p->freq > p->next->freq)
		{
			h=p->next;
			q->next=p->next;
			p->next=NULL;
			insert(p,HEAD);//insert the node back according to freq
		}
	}
	else 
	{	
		q=newNode(c);
		q->next=HEAD;  //first because freq is 1
		HEAD=q;
	}
}

void makeTree()
{
	node  *p,*q;
	p=HEAD;
	while(p!=NULL)
	{
		q=newNode('\0');
		q->type=INTERNAL;	//internal node
		q->left=p;		//join left subtree/node
		q->freq=p->freq;
		if(p->next!=NULL)
		{
			p=p->next;
			q->right=p;	//join right subtree /node
			q->freq+=p->freq;
		}
		p=p->next;	//consider next node frm list
		if(p==NULL)	//list ends
			break;
		//insert new subtree rooted at q into list starting from p
		if(q->freq <= p->freq)
		{//place it before p
			q->next=p;
			p=q;
		}
		else
			insert(q,p);	//find appropriate position
	}
	ROOT=q; 
}

node *s;

int flag=0;

void genCode(node *p, char* code)
{
	char *lcode,*rcode;
	
	if(p!=NULL)
	{
		if(p->type==LEAF)  
		{	if(flag==0) 
			{flag=1; HEAD=p;}
			else	
			{ s->next=p;}		
			p->next=NULL;
			s=p;
		}

		p->code=code;	
	//	printf("[%c|%d|%s|%d]",p->x,p->freq,p->code,p->type);
		lcode=(char *)malloc(strlen(code)+1);
		rcode=(char *)malloc(strlen(code)+1);
		sprintf(lcode,"%s0",code);
		sprintf(rcode,"%s1",code);
		genCode(p->left,lcode);		
		genCode(p->right,rcode);
	}
}



void printll()
{
node *p;
p=HEAD;

 while(p!=NULL)
 {
   printf("[%c|%d]=>",p->x,p->freq);
   p=p->next;
 }
}





int main(int argc, char *argv[])
{

	printf("Data Compression using Huffman\n\n");
	printf("COE4\n");
	printf("Arpit Sardana 101611009\n\n");

	FILE *fp,*fp2;
	char ch;
	ll t;
	HEAD=NULL;
	ROOT=NULL;
	if(argc==2)
	{

		argv[2]=(char *)malloc(strlen(argv[1])+strlen(ext)+1);
		strcpy(argv[2],argv[1]);
		strcat(argv[2],ext);
	}
	else
		return 0;

	fp=fopen(argv[1],"rb");
	if(fp==NULL)
	{
		printf("Input file empty.\n");
		return -1;
	}

	printf("\nInput file %s",argv[1]);
		while(fread(&ch,1,1,fp)!=0)
		{
			addSymbol(ch);
		}
	fclose(fp);
	
	makeTree();
	
	genCode(ROOT,"\0");	//preorder traversal

	fp=fopen(argv[1],"rb");

	fp2=fopen(argv[2],"wb");

	writeHeader(fp2);

	while(fread(&ch,1,1,fp)!=0)
	{	
		//printf("\n%c replaced with ",ch);
		writeCode(ch,fp2);	
	}
	fclose(fp);
	fclose(fp2);

	printf("\n File has been compressed\n");
	return 0;
}
