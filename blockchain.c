/******************************************Dummy Code**************************/
#include<stdio.h>
#include<stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "sha256.h"
unsigned char hash1[32];
unsigned char hash[64];

/*************************** SHA 256 ********************************/

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

static const WORD k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/*********************** FUNCTION DEFINITIONS ***********************/
void sha256_transform(SHA256_CTX *ctx, const BYTE data[])
{
	WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
	for ( ; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

	for (i = 0; i < 64; ++i) {
		t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
		t2 = EP0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
	WORD i;

	for (i = 0; i < len; ++i) {
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64) {
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
	WORD i;

	i = ctx->datalen;

	// Pad whatever data is left in the buffer.
	if (ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = ctx->bitlen;
	ctx->data[62] = ctx->bitlen >> 8;
	ctx->data[61] = ctx->bitlen >> 16;
	ctx->data[60] = ctx->bitlen >> 24;
	ctx->data[59] = ctx->bitlen >> 32;
	ctx->data[58] = ctx->bitlen >> 40;
	ctx->data[57] = ctx->bitlen >> 48;
	ctx->data[56] = ctx->bitlen >> 56;
	sha256_transform(ctx, ctx->data);

	// Since this implementation uses little endian byte ordering and SHA uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
	}
    
}

/*void convert_hash(){
   FILE *fp;
   char ch;
   fp = fopen("f.txt","w+");
    if(fp==NULL){
        printf("File not found!");
    }
    printf("Hello1");
   for(int i=0;i<32;i++){
        //fputc(hash1[i],fp);
        fprintf(fp,"%0.8x",hash1[i]);
    }
   //fwrite(hash1 , 1 , 32 , fp );
    fclose(fp);
    printf("Hello2");
   fp = fopen("f.txt","w+");
    if(fp==NULL){
        printf("File not found!!");
    }
   for(int i=0;i<64;i++){
        ch = fgetc(fp);
        //printf("%s",ch);
        hash[i] = ch;
    }
    fclose(fp);
}*/

 void print_hash(unsigned char hash[]){
    // char hashh[32] = (char)hash; 
    printf("hash:  ");
    for(int i=0;i<32;i++){
       // printf("hash1");
        //printf("%d",i);
        printf("%0.2x",hash[i]);
        //printf("hash2");
    }
    printf("\n\n");
   // printf("%s",hashh[1]);
}


void findHash(char text[]){
    
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx,text,strlen(text));
    sha256_final(&ctx,hash1);
    printf("Hello");
   // convert_hash(hash1);
    print_hash(hash1);
}

//typedef int hash;
void trans();

/************************************ Blockchain Structure *************************/

struct Block_data{
   int blockId;
   char transactionId[32];
   int timeStamp;
};
struct Block
{
    /* data ***************************************************************/
    char block_hash[32];
    char prev_block_hash[32];
    struct Block_data data[5];
    //make an array of the Block_Data structure

};
struct node {
   struct Block b;
   //int key;
   struct node *next;
};



struct node* createNode(){
     //create a temporary pointer for link
   struct node *link = (struct node*) malloc(sizeof(struct node));
   return link;
}

struct node *head = NULL;
struct node *temp = NULL;
int currentCounter = 0;

int counter(){
   int x = (int) currentCounter%5;
   return x;
}
void addBlock(){
   if(head==NULL){
   //    printf("in  1 if\n");
      temp = createNode();
      temp->b.data[counter()].blockId = (currentCounter/5)+1;
   //   printf("%d",counter());
      //temp->b.data[counter()].transactionId = transaction;
      trans(temp->b.data[counter()].transactionId);  //hash value added to blockchain
      temp->b.data[counter()].timeStamp = (int)time(NULL);
   //   printf("%d",temp->b.data[counter()].transactionId);
      //currentCounter++;
      head = temp;
  //    printf("BlockID : %d\n",temp->b.data[counter()].blockId);
    //  printf("Transaction Id : %d\n",temp->b.data[counter()].transactionId);
      currentCounter++;
   }
   else{
      struct node *traverse; 
      traverse = head;
    //  printf("in 1 else\n");
      while(traverse->next!=NULL){
        // printf("in null while loop");
         traverse = traverse->next;
      }
    //  printf("outside while loop\n");
      if(counter()==0){
        //  printf("%d",counter());
         // printf("in if\n");
          struct node *t = createNode();
         traverse->next = t;
         traverse = traverse->next;
         traverse->b.data[counter()].blockId = (int)(currentCounter/5)+1;
         traverse->b.data[counter()].timeStamp = (int)time(NULL);
         //traverse->b.data[counter()].transactionId = transaction;
         trans(traverse->b.data[counter()].transactionId);
       //  printf("BlockID : %d\n",traverse->b.data[counter()].blockId);
        // printf("Transaction Id : %d\n",traverse->b.data[counter()].transactionId);
         currentCounter++;
         /* 
         traverse->b.data[counter()].blockId = (int)(currentCounter/5)+1;
         printf("%d ",traverse->b.data[counter()].transactionId);
         temp->b.data[counter()].transactionId = transaction;
         printf("%d ",temp->b.data[counter()].transactionId);
         currentCounter++;
         */
      }
      else{
         // printf("In else\n");
          traverse->b.data[counter()].blockId = (int)(currentCounter/5)+1;
          traverse->b.data[counter()].timeStamp = (int)time(NULL);
         //printf("%d ",traverse->b.data[counter()].transactionId);
          //traverse->b.data[counter()].transactionId = transaction;
          trans(traverse->b.data[counter()].transactionId);
       //  printf("BlockID : %d\n",traverse->b.data[counter()].blockId);
         //printf("Transaction Id : %d\n",traverse->b.data[counter()].transactionId);
         currentCounter++;
          
         /* 
         struct node *t = createNode();
         traverse->next = t;
         traverse->b.data[counter()].blockId = (int)(currentCounter/5)+1;
         traverse->b.data[counter()].transactionId = transaction;
         currentCounter++;
         */
      }
   }
}
void trans(char t[]){
   for(int i=0;i<32;i++){
      t[i] = hash1[i];
   }
}
void printChain(){
   struct node *ptr;
   if(head==NULL){
      printf("Chain is empty\n");
   }
   else{
        ptr = head;
       while(ptr!= NULL){
        for(int i=0;i<5;i++){
            printf("Time Stamp : %d\n",ptr->b.data[i].timeStamp);
            printf("BlockID : %d\n",ptr->b.data[i].blockId);
            printf("Transaction Id: ");
            print_hash(ptr->b.data[i].transactionId);
        }
        printf("\n\n");
        ptr = ptr->next;
       }
       
   }
       
   
}
void menu(){
   int m;
   char str[32];
   
   while(1){
      printf("1) Add data(transaction ID)\n");
      printf("2) Read the entire chain\n");
      printf("3) verify the chain\n");
      printf("4) search for hash value\n");
      scanf("%d",&m);
      switch (m)
      {
         case 1 /* constant-expression */:
            /* code */
            printf("Enter certificate id\n");
            scanf("%s",str);
            findHash(str);
            addBlock();
            break;
         case 2:
            printChain();
            printf("Stuck");
            break;
         default:
            exit(0);
      }
   }
}
int main(){

   menu();
}
