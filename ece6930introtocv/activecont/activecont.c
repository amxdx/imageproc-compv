main()

{
FILE        *fpt,*fpt1,*fpt2;
unsigned char    *image,*imggrad,*imgconst;
float        *imagex,*imagey,*imagegrad;
char        header[320],header1[320],alphabet;
int        ROWS,COLS,BYTES,ROWS1,COLS1,BYTES1,posx,posy;
int        r,c,r2,c2,r1,c1;

float         energy[750]={0};
float        distbox[500]={0} ;
float         distsqrbox[500]={0} ;
float         devibox[500]={0} ;
int         distemp;
float         maxtemp1 , mintemp1,maxtemp2,mintemp2,maxenergy,minenergy ;
float         range1,range2;

    /* read the parenthood.ppm image */
if ((fpt=fopen("hawk.ppm","rb")) == NULL)
  {
  printf("Unable to open hawk.ppm for reading\n");
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

int poscols[300];
int posrows[300];


imgconst=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
    imgconst[r*COLS+c]=image[r*COLS+c];


int mama=1;
    /* read the ground truth files, parenthood_gt.txt */
char buffer[300];
if ((fpt1=fopen("hawk_init.txt","r")) == NULL)
  {
  printf("Unable to open hawk_init.txt for reading\n");
  exit(0);
  }
     while(fgets(buffer,300,fpt1)!=NULL)
{

fscanf(fpt1,"%d %d",&posx,&posy);
poscols[mama]=posx;
posrows[mama]=posy;
//printf(" %d %d \n",poscols[mama],posrows[mama]); 
mama++;
}
fclose(fpt1);

poscols[0] = 250;
posrows[0] = 78;

int i;
for(i=0;i<mama;i++)
for(r1=-3;r1<4;r1++)
for(c1=-3;c1<4;c1++)
if (r1==0)
imgconst[(posrows[i]+r1)*COLS+(poscols[i]+c1)]=0;
else if(c1==0)
imgconst[(posrows[i]+r1)*COLS+(poscols[i]+c1)]=0;

float temp =0;
float dist[500]={0};
float distsqr[500]={0};
float devi[500]={0};
for (i=0;i<mama;i++)
    {
    if(i<mama-1)
    {
        dist[i] = sqrt((posrows[i]-posrows[i+1])*(posrows[i]-posrows[i+1]) + (poscols[i]-poscols[i+1])*(poscols[i]-poscols[i+1]));
        distsqr[i]= dist[i]*dist[i];
        temp+= dist[i];
    }
    else 
    {
        dist[i] = sqrt((posrows[i]-posrows[0])*(posrows[i]-posrows[0]) + (poscols[i]-poscols[0])*(poscols[i]-poscols[0]));
        distsqr[i]= dist[i]*dist[i];
        temp+= dist[i];
    }
    }


float avg =0;
avg = temp/mama ;

for (i=0;i<mama;i++)
    devi[i] = (avg - dist[i])*(avg - dist[i]);
    

float         max ,min;
float         range =0;


//weight distance square rescaling 
max=0;
min=avg*avg;
for (i=0; i<mama;i++)
    if (distsqr[i]>max)
        max=distsqr[i];
    else if(distsqr[i]<min)
        min=distsqr[i];

range = max-min;

for (i=0; i<mama;i++)
{
        distsqr[i] = distsqr[i]-min;
        distsqr[i] = distsqr[i]/range;
        printf("%f",distsqr[i]);
}


// initialzing sobel kernels 

int sobelx[8] ={0};
int sobely[8] ={0};

sobelx[0]= 1;
sobelx[1]= 2;
sobelx[2]= 1;
sobelx[6]= -1;
sobelx[7]= -2;
sobelx[8]= -1;

sobely[2]= 1;
sobely[5]= 2;
sobely[8]= 1;
sobely[0]= -1;
sobely[3]= -2;
sobely[6]= -1;

imagex=(float *)calloc(ROWS*COLS,sizeof(float));
imagey=(float *)calloc(ROWS*COLS,sizeof(float));
float sum=0;

for (r=1; r<ROWS-1; r++)
  for (c=1; c<COLS-1; c++)
    {
    sum =0;
    for (r2=-1; r2<2; r2++)
    for (c2=-1; c2<2; c2++)
    {
        sum+=image[(r+r2)*COLS+(c+c2)]*sobelx[(1+r2)*3+(1+c2)];
    imagex[r*COLS+c]=sum;
        }
    }

for (r=1; r<ROWS-1; r++)
  for (c=1; c<COLS-1; c++)
    {
    sum =0;
    for (r2=-1; r2<2; r2++)
    for (c2=-1; c2<2; c2++)
    {
        sum+=image[(r+r2)*COLS+(c+c2)]*sobely[(1+r2)*3+(1+c2)];
    imagey[r*COLS+c]=sum;
        }
    }
imagegrad=(float *)calloc(ROWS*COLS,sizeof(float));

for (r=1; r<ROWS-1; r++)
  for (c=1; c<COLS-1; c++)
     imagegrad[r*COLS+c]= sqrt((imagex[r*COLS+c]*imagex[r*COLS+c]) + (imagey[r*COLS+c]*imagey[r*COLS+c]));


max=0;
min=255;

for (r=0; r<ROWS;r++)
     for(c=0;c<COLS;c++)
    {
        if (imagegrad[r*COLS+c]>max)
            max=imagegrad[r*COLS+c];
        if(imagegrad[r*COLS+c]<min)
            min=imagegrad[r*COLS+c];
    }

imggrad=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

for (r=0; r<ROWS;r++)
{
for (c=0;c<COLS;c++)
{
        imagegrad[r*COLS+c] = imagegrad[r*COLS+c]-min;
        imagegrad[r*COLS+c] = imagegrad[r*COLS+c]*255;
        imagegrad[r*COLS+c] = imagegrad[r*COLS+c]/range;
        imggrad[r*COLS+c] = imagegrad[r*COLS+c];


}
}

// making an attempt at the main sector 

//printf("\n %d %d %d",poscols[40],posrows[40],mama);
int looper =0;
for(looper =0; looper<30;looper++)
{
for (i=0;i<mama;i++)
{
//printf("%f",avg);
maxtemp1 =0;
mintemp1 =1000000;
maxtemp2 =0;
mintemp2 =1000000;
distemp =0;
range1 =0;
range2 =0;

    for(r2=-3;r2<4;r2++)
        for(c2=-3;c2<4;c2++)
        {
        if (i==mama-1)
        {
        distbox[distemp] = sqrt(((posrows[i]+r2)-posrows[0])*((posrows[i]+r2)-posrows[0]) + ((poscols[i]+c2)-poscols[0])*((poscols[i]+c2)-poscols[0]));        
        }
        else
        {
        distbox[distemp] = sqrt((posrows[i]+r2)-posrows[i+1])*((posrows[i]+r2)-posrows[i+1]) + ((poscols[i]+c2)-poscols[i+1])*((poscols[i]+c2)-poscols[i+1]);        

        }

        //printf(" %d %d %d %d %f \n",distemp,i,poscols[distemp]+c2,posrows[distemp]+r2, distbox[distemp]);


        distsqrbox[distemp] = distbox[distemp]*distbox[distemp];
        devibox[distemp] = (avg - distbox[distemp])*(avg - distbox[distemp]);
        distemp++;

        }
distemp=0;
    for(r2=-3;r2<4;r2++)
        for(c2=-3;c2<4;c2++)
        {
        if (distsqrbox[distemp]>maxtemp1)
        {
        maxtemp1=distsqrbox[distemp];
        }
        if(distsqrbox[distemp]<mintemp1)
        {
        mintemp1=distsqrbox[distemp];
        }
        
        if (devibox[distemp]>maxtemp2)
        {
        maxtemp2 = devibox[distemp];
        }
        if(devibox[distemp]<mintemp2)
        {
        mintemp2= devibox[distemp];
        }

        distemp++;
        }

//printf("maxtemp1 = %f mintemp2 = %f maxtemp1 = %f mintemp2 = %f \n",maxtemp1,mintemp1,maxtemp2,mintemp2);
distemp=0;
range1 = maxtemp1-mintemp1;
range2 = maxtemp2-mintemp2;

//printf("range 1 = %f range 2 = %f \n",range1,range2);
    for(r2=-3;r2<4;r2++)
        for(c2=-3;c2<4;c2++)
        {
        distsqr[distemp] = distsqr[distemp] -mintemp1;
        distsqr[distemp] = distsqr[distemp]/range1;
        devibox[distemp] = devibox[distemp] -mintemp2;
        devibox[distemp] = devibox[distemp]/range2;
        distemp++;
        }
distemp=0;
    for(r2=-3;r2<4;r2++)
        for(c2=-3;c2<4;c2++)
        {
        energy[distemp] = 7*distsqrbox[distemp]+48*devibox[distemp]+ 17*imagegrad[(posrows[distemp]+r2)*COLS+ (poscols[distemp]+c2)];
        distemp++;
                    //printf("%f\n",energy[distemp]);
        }
energy[50]=3;
minenergy=3;
distemp=0;
    for(r2=-3;r2<4;r2++)
        for(c2=-3;c2<4;c2++)
        {
        if (energy[distemp]<minenergy)
            {
            minenergy=energy[distemp];
            //printf("%f",minenergy);
            }            
            distemp++;
        }
distemp=0;
    for(r2=-3;r2<4;r2++)
        for(c2=-3;c2<4;c2++)
        {
            if(energy[distemp]==minenergy)
                {
                //printf("%d %d \n updated \t",poscols[i],posrows[i]);
                poscols[i]= poscols[i]+c2;
                posrows[i]= posrows[i]+r2;
                //printf("%d %d ",poscols[i],posrows[i]);
                }
            distemp++;
        }

}
}
/*
for(i=0;i<mama;i++)
for(r1=-3;r1<4;r1++)
for(c1=-3;c1<4;c1++)
if (r1==0)
imgconst[(posrows[i]+r1)*COLS+(poscols[i]+c1)]=0;
else if(c1==0)
imgconst[(posrows[i]+r1)*COLS+(poscols[i]+c1)]=0;*/







fpt2=fopen("new_hawk.ppm","w");
fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS);
fwrite(imgconst,COLS*ROWS,1,fpt2);
fclose(fpt2);
}








