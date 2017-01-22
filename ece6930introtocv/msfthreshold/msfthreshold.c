#include <stdio.h> 
#include <stdlib.h> 
main() 
{ 
FILE *fpt,*fpt1,*fpt2,*fpt3,*fpt4; 
unsigned char *image,*msfimg; 
unsigned char *found,*found1; 
char header[320],header1[320],alphabet; 
int ROWS,COLS,BYTES,ROWS1,COLS1,BYTES1; 
int r,c,r1,c1; 
/* read the parenthood.ppm image */ 
if ((fpt=fopen("parenthood.ppm","rb")) == NULL) 
{ 
printf("Unable to open parenthood.ppm for reading\n"); 
exit(0); 
} 
fscanf(fpt,"%s %d %d %d\n",header,&COLS,&ROWS,&BYTES); 
if (strcmp(header,"P5") != 0 || BYTES != 255) 
{ 
printf("Not a greyscale 8-bit PPM image\n"); 
exit(0); 
} 
image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char)); 
header[0]=fgetc(fpt); /* read white-space character that separates header */ 
fread(image,1,COLS*ROWS,fpt); 
fclose(fpt); 
/* read the msf image, msf.ppm */ 
if ((fpt1=fopen("msf.ppm","rb")) == NULL) 
{ 
printf("Unable to open msf.ppm for reading\n"); 
exit(0); 
} 
fscanf(fpt1,"%s %d %d %d\n",header1,&COLS1,&ROWS1,&BYTES1); 
if (strcmp(header1,"P5") != 0 || BYTES1 != 255) 
{ 
printf("Not a greyscale 8-bit PPM image\n"); 
exit(0); 
} 
msfimg=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char)); 
header1[0]=fgetc(fpt1); /* read white-space character that separates header */ 
fread(msfimg,1,COLS1*ROWS1,fpt1); 
fclose(fpt1); 
/* allocate memory for detected version of image and for thinned skeleton image */ 
found=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char)); 
found1=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char)); 
for ( r=0; r<ROWS; r++) 
{ for (c=0; c<COLS; c++) 
{ 
found[r*COLS+c]=255; 
} 
} 
int thresh = 230; // threshold 
for (r=7;r<ROWS-8;r++) 
for(c=4;c<COLS-5;c++) 
if (msfimg[r*COLS+c] >thresh) 
{ 
for (r1=-7;r1<8;r1++) 
{ 
for(c1=-4;c1<5;c1++) 
{ 
found[(r+r1)*COLS+(c1+c)] = image[(r+r1)*COLS+(c1+c)]; 
} 
} 
} 
for ( r=0; r<ROWS; r++) 
{ for (c=0; c<COLS; c++) 
{ 
if(found[r*COLS+c]>128) 
found[r*COLS+c]=255; 
else 
found[r*COLS+c]=0; 
found1[r*COLS+c]=found[r*COLS+c]; 
} 
} 
int count; //edge neighbours 
int ene; // edge to non edge transitions 
int delposx[10000], delposy[10000]; 
int bhme=1; 
//thinning algorithm 
int n89, n81, n82,n83,n84,n85,n86,n87,n88 ; // initialzing n8 neighbours 
while(bhme) 
{ 
bhme =0; 
for ( r=1; r<ROWS-1; r++) 
{ for (c=1; c<COLS-1; c++) 
{ 
if(found[r*COLS+c]==255) 
continue; 
{ 
count =0; 
ene = 0; 
n89 = found[r*COLS+c]; 
n81 = found[(r-1)*COLS+(c)]; 
n82 = found[(r-1)*COLS+(c+1)]; 
n83 = found[(r)*COLS+(c+1)]; 
n84 = found[(r+1)*COLS+(c+1)]; 
n85 = found[(r+1)*COLS+(c+1)]; 
n86 = found[(r+1)*COLS+(c-1)]; 
n87 = found[(r)*COLS+(c-1)]; 
n88 = found[(r-1)*COLS+(c-1)]; 
ene = (n81==0 && n82==255)+(n82==0 && n83==255)+(n83==0 && n84==255)+(n84==0 && n85==255)+(n85==0 && n86==255)+(n86==0 && n87==255)+(n87==0 && n88==255)+(n88==0 && n81==255); // calculating the no of edge to non edge transitions 
for(r1 = -1;r1<2;r1++) 
{ 
for(c1 = -1;c1<2;c1++) 
{ 
if(found[(r+r1)*COLS+(c+c1)]==0) 
{ 
count = count +1 ; // calculating the number of edge neighbours 
} 
} 
} 
if((n81==255 || n83==255 || (n85==255 && n87==255)) && ene == 1 && count>=3 && count<=7) // marking pixels for erasing. 
{ 
delposx[bhme] = r; 
delposy[bhme]= c; 
bhme++; 
} 
} 
} 
} 
printf(" bhme = %d \n",bhme); 
for ( r=0; r<=bhme; r++) 
found[(delposx[r])*COLS+(delposy[r])]=255; // erasing pixels for thinning the image to one pixel wide. 
printf(" COLS = %d ROWS = %d \n ", COLS, ROWS); 
} // looping till the characters are 1 pixel wide. 
int posx1, posy1 ; 
int rowpos[1300], colpos[1300]; 
int count1=0; 
char alp[5000]; 
/* read the ground truth files, parenthood_gt.txt */ 
char buffer[1300]; 
if ((fpt4=fopen("parenthood_gt.txt","r")) == NULL) 
{ 
printf("Unable to open parenthood_gt.txt for reading\n"); 
exit(0); 
} 
while(fgets(buffer,1300,fpt4)!=NULL) 
{ 
fscanf(fpt4,"%s %d %d \n",&alphabet,&posx1,&posy1); 
//printf(" \n %s %d %d \n", &alphabet,posx1,posy1); 
colpos[count1]=posx1; 
rowpos[count1]=posy1; 
alp[count1] =alphabet; 
//printf(" \n %s %d %d \n", &alp[count1],posx1,posy1); 
count1++; 
} 
int ep ,bp ,tn ,fp ,tp ,fn,n1; 
for ( n1 =0; n1<=count1; n1++) 
{ ep =0; bp =0; 
if (alp[n1] =='e') // finding the position of e from ground truth and checking the 9x15 area 
{ 
for (r1 = -7 ; r1<8 ;r1++) 
for(c1=-4;c1<5;c1++) 
{ ep =0; 
bp=0; 
n89 = found[(rowpos[n1]+r1)*COLS+(colpos[n1]+c1)]; 
n81 = found[(rowpos[n1]+r1-1)*COLS+(colpos[n1]+c1)]; 
n82 = found[(rowpos[n1]+r1-1)*COLS+(colpos[n1]+c1+1)]; 
n83 = found[(rowpos[n1]+r1)*COLS+(colpos[n1]+c1+1)]; 
n84 = found[(rowpos[n1]+r1+1)*COLS+(colpos[n1]+c1+1)]; 
n85 = found[(rowpos[n1]+r1+1)*COLS+(colpos[n1]+c1+1)]; 
n86 = found[(rowpos[n1]+r1+1)*COLS+(colpos[n1]+c1-1)]; 
n87 = found[(rowpos[n1]+r1)*COLS+(colpos[n1]+c1-1)]; 
n88 = found[(rowpos[n1]+r1-1)*COLS+(colpos[n1]+c1-1)]; 
ene =0; 
ene = (n81==0 && n82==255)+(n82==0 && n83==255)+(n83==0 && n84==255)+(n84==0 && n85==255)+(n85==0 && n86==255)+(n86==0 && n87==255)+(n87==0 && n88==255)+(n88==0 && n81==255); 
//printf(" %d \n ",ene); 
// searching for branch points and end points 
if (ene ==1 ) 
ep++; 
else if (ene>2) 
bp++; 
} 
// calculating the number of true positives and false negatives 
if (ep ==1 && bp==1) 
tp++; 
else 
fn++; 
} 
else 
{ 
// for every other alphabet , finding the position and checking the 9x15 area. 
for (r1 = -7 ; r1<8 ;r1++) 
for(c1=-4;c1<5;c1++) 
{ 
ep=0; 
bp=0; 
n89 = found[(rowpos[n1]+r1)*COLS+(colpos[n1]+c1)]; 
n81 = found[(rowpos[n1]+r1-1)*COLS+(colpos[n1]+c1)]; 
n82 = found[(rowpos[n1]+r1-1)*COLS+(colpos[n1]+c1+1)]; 
n83 = found[(rowpos[n1]+r1)*COLS+(colpos[n1]+c1+1)]; 
n84 = found[(rowpos[n1]+r1+1)*COLS+(colpos[n1]+c1+1)]; 
n85 = found[(rowpos[n1]+r1+1)*COLS+(colpos[n1]+c1+1)]; 
n86 = found[(rowpos[n1]+r1+1)*COLS+(colpos[n1]+c1-1)]; 
n87 = found[(rowpos[n1]+r1)*COLS+(colpos[n1]+c1-1)]; 
n88 = found[(rowpos[n1]+r1-1)*COLS+(colpos[n1]+c1-1)]; 
ene =0; 
ene = (n81==0 && n82==255)+(n82==0 && n83==255)+(n83==0 && n84==255)+(n84==0 && n85==255)+(n85==0 && n86==255)+(n86==0 && n87==255)+(n87==0 && n88==255)+(n88==0 && n81==255); 
//printf(" %d \n ",ene); 
// searching for branch points and end points 
if (ene ==1 ) 
ep++; 
else if (ene>2) 
bp++; 
} 
// calculating the number of true negatives and false positives. 
if (ep ==0 && bp==0) 
tn++; 
else 
fp++; 
} 
} 
printf(" TP = %d\t FP = %d\n",tp,fp); // Printing the true positives and false negatives. 
fclose(fpt4); 
ROWS=567; 
/* write out smoothed image to see result */ 
fpt2=fopen("found.ppm","w"); 
fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS); 
fwrite(found,COLS*ROWS,1,fpt2); 
fclose(fpt2); 
fpt3=fopen("found1.ppm","w"); 
fprintf(fpt3,"P5 %d %d 255\n",COLS,ROWS); 
fwrite(found1,COLS*ROWS,1,fpt2); 
fclose(fpt3); 
}
