 /*
    ** This program reads a parenthood.ppm image and identifies all the 'e' letters in it
    ** It uses tempmsf and a template to match and identify the letter 'e'. 
    */

#include <stdio.h>
#include <stdlib.h>


main()

{
FILE        *fpt,*fpt1,*fpt2,*fpt3;
unsigned char    *image,*template;
unsigned char    *found;
int             *tempmsf,*msf,*fliptemp;
char        header[320],header1[320],alphabet;
int        ROWS,COLS,BYTES,ROWS1,COLS1,BYTES1,posx,posy;
int        r,c,r1,c1;


    /* read the parenthood.ppm image */
if ((fpt=fopen("parenthood.ppm","rb")) == NULL)
  {
  printf("Unable to open parenthood.ppm for reading\n");
  exit(0);
  }
fscanf(fpt,"%s %d %d %d\n",header,&COLS,&ROWS,&BYTES);
if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
  printf("Not a greyscale 8-bit PPM image\n");
  exit(0);
  }
image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
header[0]=fgetc(fpt);    /* read white-space character that separates header */
fread(image,1,COLS*ROWS,fpt);
fclose(fpt);

//printf("ROWS =%d \n ", ROWS);


    /* read the template image, parenthood_e_template.ppm */
if ((fpt1=fopen("parenthood_e_template.ppm","rb")) == NULL)
  {
  printf("Unable to open parenthood_e_template.ppm for reading\n");
  exit(0);
  }
fscanf(fpt1,"%s %d %d %d\n",header1,&COLS1,&ROWS1,&BYTES1);
if (strcmp(header1,"P5") != 0  ||  BYTES1 != 255)
  {
  printf("Not a greyscale 8-bit PPM image\n");
  exit(0);
  }
ROWS1=ROWS1-1;
template=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
header1[0]=fgetc(fpt1);    /* read white-space character that separates header */
fread(template,1,COLS1*ROWS1,fpt1);
fclose(fpt1);
ROWS=567;
         /*allocate memory for tempmsf image calculated*/
tempmsf = (int *)calloc(ROWS1*COLS1,sizeof(int));

msf = (int *)calloc(ROWS*COLS,sizeof(int));
    /* allocate memory for detected version of image */
found=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

int r2,c2 ;
int sum = 0, avg =0;

//printf("\n Average = %d",avg);
//ROWS=567;
//printf(" ROWS = %d \tCOLS = %d \tROWS1 = %d \tCOLS1 = %d \t\n",ROWS,COLS,ROWS1,COLS1);
for ( r2=0; r2<ROWS1; r2++)
 {     for (c2=0; c2<COLS1; c2++)
       {    sum+=template[(r2)*COLS1+(c2)];
}
}

 //printf("\n Sum = %d, %d",sum,COLS1*ROWS1);      
avg=sum/(COLS1*ROWS1);
//printf("\n Average = %d\n ",avg);

int sum1=0,tempa=0;
for ( r2=0; r2<ROWS1; r2++)
{      for (c2=0; c2<COLS1; c2++)
              { 
                 tempmsf[r2*COLS1+c2]= template[(r2)*COLS1+(c2)]-avg;
               // printf("\t %d", tempmsf[r2*COLS1+c2]);
         tempa += tempmsf[r2*COLS1+c2];
}
      // printf("\n");
}

for ( r= 0; r<ROWS;r++)
    for(c=0;c<COLS;c++)
        msf[r*COLS+c]=0;


//printf(" tempa = %d", tempa);

for (r = 6 ; r< ROWS-7;r++)
{
 for (c=4;c<COLS-5;c++)
   {
sum1=0;  
     for (r2 = -6; r2< 7;r2++)
      {
 for (c2= -4; c2< 5; c2++)
  {
         sum1+= tempmsf[(r2+6)*COLS1+(c2+4)]*image[(r+r2)*COLS+(c+c2)];
 
}
}
msf[r*COLS+c]=sum1; 
}
}
 int max=0,min=0;

for (r=6; r<ROWS-7;r++)
     for(c=4;c<COLS-5;c++)
        if (msf[r*COLS+c]>max)
            max=msf[r*COLS+c];
        else if(msf[r*COLS+c]<min)
            min=msf[r*COLS+c];

int range =0;
range = max-min; 
//printf("\n range = %d",range);


int colpose[151];
int rowpose[151];
colpose[151]=0;
rowpose[151]=0;
int count=0,count1=0;
int colposr[1111];
int rowposr[1111];
colposr[1111]=0;
rowposr[1111]=0;


    /* read the ground truth files, parenthood_gt.txt */
char buffer[1300];
if ((fpt3=fopen("parenthood_gt.txt","r")) == NULL)
  {
  printf("Unable to open parenthood_gt.txt for reading\n");
  exit(0);
  }
     while(fgets(buffer,1300,fpt3)!=NULL)
{
fscanf(fpt3,"%s %d %d \n",&alphabet,&posx,&posy);
//printf(" \n %s  %d %d \n", &alphabet,posx,posy);
     
{ if (alphabet =='e')
         
    { colpose[count]=posx;
     rowpose[count]=posy;
    
    //printf(" found e bitch colpos = %d,\t rowpos = %d\t posx = %d \t posy =%d \n", colpose[count],rowpose[count],posx,posy);         
    count++;
      }
    else
    { 
    colposr[count1]=posx;
    rowposr[count1]=posy;
    //printf(" colpos = %d,\t rowpos = %d\t posx = %d \t posy =%d \n", colposr[count1],rowposr[count1],posx,posy);         
    count1++;
    }
}    
}
int i=0,j=0;
int flag =0,tp=0,fp=0;
int tresh=0;
for (tresh =0; tresh<255;tresh++)
for (r=6; r<ROWS-7;r++)
    {
        for (c=4;c<COLS-5;c++)
        {
        msf[r*COLS+c] = msf[r*COLS+c]-min;
        msf[r*COLS+c] = msf[r*COLS+c]*255;
        msf[r*COLS+c] = msf[r*COLS+c]/range;
        //printf("    %d",msf[r*COLS+c]);
        if(msf[r*COLS+c]>tresh)
            msf[r*COLS+c]=255;
        //else if (msf[r*COLS+c]>168 && (msf[r*COLS+c]<254))
            //msf[r*COLS+c]=255;
        
        else 
            msf[r*COLS+c]=0;

}
        //printf("\n");
}
 for (i=0;i<count+1;)
    {jump: i++;
        for (r2 = -6; r2< 7;r2++)
            {  for (c2= -4; c2< 5; c2++)
                { 
                    if (msf[(rowpose[i]+r2)*colpose[i]+(colpose[i]+c2)]==255)
                               {flag =1;
                            if (flag==1)
                                {flag=0;
                                     tp++;
                                        goto jump;
                }
            }
        }
    }


   }
flag =0;
 for (i=0;i<count1+1;)
     {jump1:i++;
        for (r2 = -6; r2< 7;r2++)
            {  for (c2= -4; c2< 5; c2++)
                {
                    if (msf[(rowposr[i]+r2)*colposr[i]+(colposr[i]+c2)]==255)
                               {flag =1;
                            if (flag==1)
                                {flag=0;
                                     fp++;
                                        goto jump1;
                }
            }
        }
    }

      }



printf(" True Positives = %d \t False Positive = %d\tTresh = %d \n ", tp,fp,tresh);
    

//printf(" \n max = %d \t min = %d \n", max,min);


    /* write out smoothed image to see result */
fpt2=fopen("msf.ppm","w");
fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS);
fwrite(msf,COLS*ROWS,1,fpt2);
fclose(fpt2);
}
