#include <stdio.h>
#include <math.h>

#define ROWS    128
#define COLS    128

/*
**    This routine converts the data in an Odetics range image into 3D
**    cartesian coordinate data.  The range image is 8-bit, and comes
**    already separated from the intensity image.
*/


main(int argc, char *argv[])

/*int    argc;
char    *argv[];*/

{
int    r,c;
double    cp[7];
double    xangle,yangle,dist;
double    ScanDirectionFlag,SlantCorrection;
unsigned char    RangeImage[128*128];
double        P[3][128*128];
int             ImageTypeFlag;
char    Filename[160],Outfile[160];
FILE    *fpt;

printf("Enter range image file name:");
scanf("%s",Filename);
if ((fpt=fopen(Filename,"r")) == NULL)
  {
  printf("Couldn't open %s\n",Filename);
  //exit(0);
  }
fread(RangeImage,1,128*128,fpt);
fclose(fpt);

int i ; 
for(i =0; i<ROWS*COLS; i++) 
{
if (RangeImage[i] > 128 ) 
RangeImage[i] = 255; 
}


printf("Up(-1), Down(1) or Neither(0)? ");
scanf("%d",&ImageTypeFlag);


cp[0]=1220.7;        /* horizontal mirror angular velocity in rpm */
cp[1]=32.0;        /* scan time per single pixel in microseconds */
cp[2]=(COLS/2)-0.5;        /* middle value of columns */
cp[3]=1220.7/192.0;    /* vertical mirror angular velocity in rpm */
cp[4]=6.14;        /* scan time (with retrace) per line in milliseconds */
cp[5]=(ROWS/2)-0.5;        /* middle value of rows */
cp[6]=10.0;        /* standoff distance in range units (3.66cm per r.u.) */

cp[0]=cp[0]*3.1415927/30.0;    /* convert rpm to rad/sec */
cp[3]=cp[3]*3.1415927/30.0;    /* convert rpm to rad/sec */
cp[0]=2.0*cp[0];        /* beam ang. vel. is twice mirror ang. vel. */
cp[3]=2.0*cp[3];        /* beam ang. vel. is twice mirror ang. vel. */
cp[1]/=1000000.0;        /* units are microseconds : 10^-6 */
cp[4]/=1000.0;            /* units are milliseconds : 10^-3 */

switch(ImageTypeFlag)
  {
  case 1:        /* Odetics image -- scan direction upward */
    ScanDirectionFlag=-1;
    break;
  case 0:        /* Odetics image -- scan direction downward */
    ScanDirectionFlag=1;
    break;
  default:        /* in case we want to do this on synthetic model */
    ScanDirectionFlag=0;
    break;
  }

    /* start with semi-spherical coordinates from laser-range-finder: */
    /*            (r,c,RangeImage[r*COLS+c])          */
    /* convert those to axis-independant spherical coordinates:      */
    /*            (xangle,yangle,dist)              */
    /* then convert the spherical coordinates to cartesian:           */
    /*            (P => X[] Y[] Z[])              */

if (ImageTypeFlag != 3)
  {
  for (r=0; r<ROWS; r++)
    {
    for (c=0; c<COLS; c++)
      {
      SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
      xangle=cp[0]*cp[1]*((double)c-cp[2]);
      yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+    /* Standard Transform Part */
    SlantCorrection*ScanDirectionFlag;    /*  + slant correction */
      dist=(double)RangeImage[r*COLS+c]+cp[6];
      P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
    +(tan(yangle)*tan(yangle))));
      P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
      P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
      }
    }
  }

sprintf(Outfile,"%s.coords",Filename);
fpt=fopen(Outfile,"w");
fwrite(P[0],8,128*128,fpt);
fwrite(P[1],8,128*128,fpt);
fwrite(P[2],8,128*128,fpt);
fclose(fpt);
}

Code to read the .coords file and segment the range image. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SQR(x) ((x)*(x))


#define MAX_QUEUE 10000    /* max perimeter size (pixels) of border wavefront */


double surfnorm[3][128*128];
void RegionGrow(unsigned char *image,    /* image data */
        unsigned char *labels,    /* segmentation labels */
        int ROWS,int COLS,    /* size of image */
        int r,int c,        /* pixel to paint from */
        int paint_over_label,    /* image label to paint over */
        int new_label,        /* image label for painting */
        int *indices,        /* output:  indices of pixels painted */
        int *count)        /* output:  count of pixels painted */
{
int    r2,c2;
int    queue[MAX_QUEUE],qh,qt;
int    average,total;    /* average and total intensity in growing region */
double thetanext=0;
double avg=0;
double x0,y0,z0,x1,y1,z1,arg;
double theta;
int threshold;
int dist=3;
int mama =0;
int intinc=0;
double surfnormxavg,surfnormyavg,surfnormzavg;
        x0 = surfnorm[0][r*COLS+c];
            y0 = surfnorm[1][r*COLS+c];
            z0 = surfnorm[2][r*COLS+c];
        x1=0;
        y1=0;
        z1=0;
            arg = (x0*x1 +y0*y1+z0*z1)/(sqrt((x0*x0)+(y0*y0)+(z0*z0)) * sqrt((x1*x1)+(y1*y1)+(z1*z1)));
            
        x1+=a0;y1+=y0;z1+=z0;
            theta= acos(arg)*(180/3.141592);
            avg+= theta;
        
*count=0;
if (labels[r*COLS+c] != paint_over_label)
  return;
labels[r*COLS+c]=new_label;
surfnormxavg=surfnorm[0][r*128+c];
surfnormyavg=surfnorm[1][r*128+c];
surfnormzavg=surfnorm[2][r*128+c];

if (indices != NULL)
  indices[0]=r*COLS+c;
queue[0]=r*COLS+c;
qh=1;    /* queue head */
qt=0;    /* queue tail */
(*count)=1;
while (qt != qh)
  {
  for (r2=-1; r2<=1; r2++)
    for (c2=-1; c2<=1; c2++)
      {
      if (r2 == 0  &&  c2 == 0)
        continue;
    int addrow=queue[qt]/128+r2;
    int addcol=queue[qt]%128+c2;
      if (addrow < 0  ||  addrow >= 128-dist  ||
      (addcol < 0  ||  addcol >= 128-dist)
        continue;
      
        /* test criteria to join region */
      if (abs(theta) > threshold)
        continue;
    mama++;
    surfnormxavg=surfnormxavg*(mama-1)+ surfnorm[0][addrow*128+addcol];
    surfnormxavg=surfnormxavg/mama;
    surfnormyavg=surfnormyavg*(mama-1)+ surfnorm[1][addrow*128+addcol];
    surfnormyavg=surfnormyavg/mama;
    surfnormzavg=surfnormzavg*(mama-1)+ surfnorm[2][addrow*128+addcol];
    surfnormzavg=surfnormzavg/mama;
      if (indices != NULL)
        indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      
      (*count)++;
      queue[qh]=addrow*128+addcol;
      qh=(qh+1)%MAX_QUEUE;
      if (qh == qt)
        {
        printf("Max queue size exceeded\n");
        exit(0);
        }
      }
  qt=(qt+1)%MAX_QUEUE;
  }
  intinc = intinc +30;
}


main()

{
FILE        *fpt,*fpt1,*fpt2,*fpt3,*fpt4;
unsigned char    *imagels,*labels;
unsigned char    *found,*found1;

char        headerls[320],header1[320],alphabet;
float        P1,P2,P3;
int        ROWS,COLS,BYTES;
ROWS = 128; COLS = 128;
int        r,c,r2,c2;
int        *indices,i;
int        RegionSize,*RegionPixels,TotalRegions;
double        avg,var;
//void        RegionGrow();


if ((fpt=fopen("range.coords","rb")) == NULL)
  {
  printf("Unable to open range.coords for reading\n");
  exit(0);
  }
fscanf(fpt,"%g %g %g\n",&P1,&P2,&P3);
double p[3][128*128];    /* read white-space character that separates header */
fread(p[0],8,128*128,fpt);
fread(p[1],8,128*128,fpt);
fread(p[2],8,128*128,fpt);
fclose(fpt);


    /* read the parenthood.ppm image */
if ((fpt1=fopen("chair-range.ppm","rb")) == NULL)
  {
  printf("Unable to open chair-range.ppm for reading\n");
  exit(0);
  }
fscanf(fpt1,"%s %d %d %d\n",headerls,&COLS,&ROWS,&BYTES);
if (strcmp(headerls,"P5") != 0  ||  BYTES != 255)
  {
  printf("Not a greyscale 8-bit PPM image\n");
  exit(0);
  }
imagels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
headerls[0]=fgetc(fpt1);    /* read white-space character that separates header */
fread(imagels,1,COLS*ROWS,fpt1);
fclose(fpt1);

int ils ; 

for(ils =0; ils<ROWS*COLS; ils++) 
{
if (imagels[ils] > 128 ) 
imagels[ils] = 255; 

}
double x0,y0,z0,x1,y1,z1,arg; 
double theta[25];

int dist = 3;
for (r = 0 ; r < 128-dist ; r++)
    for(c = 0 ; c < 128-dist ; c++) 
{
    x0 = p[0][(r+dist)*COLS+c] - p[0][(r)*COLS+c];
    y0 = p[1][(r+dist)*COLS+c] - p[1][(r)*COLS+c];
    z0 = p[2][(r+dist)*COLS+c] - p[2][(r)*COLS+c];
    x1 = p[0][r*COLS+(c+dist)] - p[0][(r)*COLS+c];
    y1 = p[1][r*COLS+(c+dist)] - p[1][(r)*COLS+c];
    z1 = p[2][r*COLS+(c+dist)] - p[2][(r)*COLS+c];
    surfnorm[0][r*COLS+c] = y0*z1 - z0*y1 ;
    surfnorm[1][r*COLS+c] = z0*x1 - x0*z1 ;
    surfnorm[2][r*COLS+c] = x0*y1 - y0*x1 ;


    /*arg = (x0*x1 +y0*y1+z0*z1)/(sqrt((x0*x0)+(y0*y0)+(z0*z0)) * sqrt((x1*x1)+(y1*y1)+(z1*z1)));
    theta = acos(arg);
    printf("%f \n",theta);*/
}

labels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
/* used to quickly erase small grown regions */
indices=(int *)calloc(ROWS*COLS,sizeof(int));

int seedflag =0;

TotalRegions=0;
for (r=2; r<ROWS-2-dist; r++)
  {
  for (c=2; c<COLS-2-dist; c++)
    {
seedflag =0;
   /* if (labels[r*COLS+c] != 0)
{      printf("Help\n");
      continue;}
    avg=var=0.0;*/
        /* compute average and variance in 5x5 window */
    for (r2=-2; r2<=2; r2++)
      for (c2=-2; c2<=2; c2++)
    {
        if(imagels[(r+r2)*COLS+(c+c2)]!=255 && labels[(r+r2)*COLS+(c+c2)]==0 )
        {seedflag =1;
             continue;}    
    }
    if (seedflag==1)    /* condition for seeding a new region is low var */
      {
      // printf("%d,%d avg=%lf var=%lf\n",r,c,avg,var);
      TotalRegions++;
      if (TotalRegions == 255)
        {
        printf("Segmentation incomplete.  Ran out of labels.\n");
        break;
        }
      RegionGrow(imagels,labels,ROWS,COLS,r,c,0,TotalRegions,indices,&RegionSize);
    printf("%d %d \n",r,c);
      if (RegionSize < 100)
        {    /* erase region (relabel pixels back to 0) */
        for (i=0; i<RegionSize; i++)
          labels[indices[i]]=0;
        TotalRegions--;
        }
      else
        printf("Region labeled %d is %d in size\n",TotalRegions,RegionSize);
      }
    }
  if (c < COLS-3)
    break;    /* ran out of labels -- break both loops */
  }
printf("%d total regions were found\n",TotalRegions);

if ((fpt2=fopen("seg.ppm","wb")) == NULL)
  {
  printf("Unable to open file for writing\n");
  exit(0);
  }
fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS);
fwrite(labels,1,ROWS*COLS,fpt2);
fclose(fpt2);
if ((fpt4=fopen("op.ppm","wb")) == NULL)
  {
  printf("Couldn't open");
  exit(0);
  }
fprintf(fpt4,"P5 %d %d 255\n",COLS,ROWS);
fwrite(imagels,1,128*128,fpt4);
fclose(fpt4);

}
