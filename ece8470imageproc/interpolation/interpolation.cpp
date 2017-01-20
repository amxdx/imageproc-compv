// Homework.cpp : Defines the entry point for the console application.
//

#include <afxwin.h>  // necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"
#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace blepo;

float biinter(ImgFloat &img, float x, float y)				// bilinear interpolation function
{
	float x0, y0, alpx, alpy;
	x0 = floor(x);
	y0 = floor(y);
	alpx = x - x0;
	alpy = y - y0; 

	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x0 >= img.Width() - 1) x0 = img.Width() - 2;
	if (y0 >= img.Height() - 1) y0 = img.Height() - 2;

	return (1 - alpx)*(1 - alpy)*img(x0, y0) + alpx*(1 - alpy)*img(x0 + 1, y0) + (1 - alpx)*alpy*img(x0, y0 + 1) + alpx*alpy*img(x0 + 1, y0 + 1);

}


int main(int argc, const char* argv[], const char* envp[])
{

	// Initialize MFC and return if failure
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		printf("Fatal Error: MFC initialization failed (hModule = %x)\n", (int)hModule);
		return 1;
	}

	try
	{
		if (argc == 6)
		{
			CString filename = argv[1];
			int firstframe = atoi(argv[2]);
			int lastframe = atoi(argv[3]);
			float sigma = atof(argv[4]);
			int windsize = atoi(argv[5]);
			CString filename3;
			filename3.Format(filename, firstframe);  
			CString a = "../../images/";
			CString b = a + filename3;
			CString c, d; 
									
										
			ImgGray firstimg;				//loading the first frame of the sequence for extracting good features
			Set(&firstimg, 0);
			Load(b, &firstimg);
			ImgFloat firstimgfloat(firstimg.Width(), firstimg.Height());
			Set(&firstimgfloat, 0);
			Convert(firstimg, &firstimgfloat);
			ImgFloat gradx(firstimg.Width(), firstimg.Height());
			Set(&gradx, 0);
			ImgFloat grady(firstimg.Width(), firstimg.Height());
			Set(&grady, 0);
			Gradient(firstimgfloat, 1.1, &gradx, &grady);

			float lambda1 = 0, lambda2 = 0;
			float zxx = 0, zxy = 0, zyy = 0, cornerness = 0;
			float threshold = 300;
			ImgFloat pointimg(firstimg.Width(), firstimg.Height());
			Set(&pointimg, 0);						

			for (int y = 1;y < firstimg.Height() - 1;y++)						// finding the good features
			{
				for (int x = 1;x < firstimg.Width() - 1;x++)
				{
					lambda1 = 0;
					lambda2 = 0;
					zxx = 0;zxy = 0;zyy = 0;
					for (int w = -1; w <= 1; w++)
					{
						for (int ww = -1; ww <= 1;ww++)
						{
							zxx += gradx(x - w, y - ww)*gradx(x - w, y - ww);
							zxy += gradx(x - w, y - ww)*grady(x - w, y - ww);
							zyy += grady(x - w, y - ww)*grady(x - w, y - ww);
						}
					}
					lambda1 = 0.5*((zxx + zyy) + sqrt((zxx - zyy)*(zxx - zyy) + 4 * zxy*zxy));
					lambda2 = 0.5*((zxx + zyy) - sqrt((zxx - zyy)*(zxx - zyy) + 4 * zxy*zxy));
					//printf("%f %f \n", lambda1, lambda2);
					cornerness = min(lambda1, lambda2);
					//printf("%f", cornerness);
					if (cornerness > threshold)
						pointimg(x, y) = cornerness;
					else
						pointimg(x, y) = 0;
				}
			}
			ImgFloat pointsuppimg(firstimg.Width(), firstimg.Height());
			Set(&pointsuppimg, 0);
			NonmaxSuppress8(pointimg, &pointsuppimg, 0);
			Figure fig1;
			fig1.Draw(pointsuppimg);
			ImgGray blankimg(firstimg.Width(), firstimg.Height());
			Set(&blankimg, 0);
			ImgBgr goodfeat(firstimg.Width(), firstimg.Height());
			Convert(firstimg, &goodfeat);
			vector <Point2f> storept;
			Point2f pt;
			int count = 0;
			for (int y = 1;y < firstimg.Height() - 1;y++)
				for (int x = 1;x < firstimg.Width() - 1;x++)
					if (pointsuppimg(x, y) >0)
					{

						for (int w = -1; w < 2;w++)
							for (int ww = -1;ww < 2; ww++)
							{
								if(x+w >0 && y+ww>0 && x+w<firstimg.Width() && y+ww < firstimg.Height())
								goodfeat(x+w, y+ww).r = 255;								// writing good features in a 8 neighbour hood of the pixel. 
							}
						count++;
						pt.x = x;
						pt.y = y;
						storept.push_back(pt);												// storing pixel locations of good features.

					}
			Figure fig2;
			fig2.Draw(goodfeat);
			ImgGray ithimg(firstimg.Width(), firstimg.Height());
			Set(&ithimg, 0);
			ImgGray iplusimg(firstimg.Width(), firstimg.Height());
			Set(&iplusimg, 0);
			ImgFloat ithfimg(firstimg.Width(), firstimg.Height());
			Set(&ithfimg, 0);
			ImgFloat iplusfimg(firstimg.Width(), firstimg.Height());
			Set(&iplusfimg, 0);
			ImgBgr output(firstimg.Width(), firstimg.Height());
			Figure fig3;
			

			for (int i = firstframe; i < lastframe; i++)
			{
				CString filename4,filename5;
				filename4.Format(filename, i);
				c = a + filename4;
				Load(c, &ithimg);							// loading ith frame.
				Convert(ithimg, &ithfimg);
				filename5.Format(filename, i + 1);
				d = a + filename5;
				Load(d, &iplusimg);							// loading i+1 th frame.
				Convert(iplusimg, &iplusfimg);
				Gradient(ithfimg, sigma, &gradx, &grady);


				for (int j = 0; j < count; j++)
				{
					zxx = 0; zxy = 0; zyy = 0;
					float  u = 0, v = 0, iter = 0, errx = 0, erry = 0;

					for (int iter = 0; iter < 6;iter++)					// running iterations 
					{
						//flowergarden/img%2503d.pgm 30 58 0.9 11
						//statue_seq/img%2504d.bmp 588 618 0.9 11
						for (int w = -(windsize-1) / 2; w <= (windsize-1) / 2; w++)
							for (int ww = -(windsize-1) / 2; ww <= (windsize-1) / 2;ww++)
							{
								if ((storept[j].x + w) >= 0 && (storept[j].y + ww) >= 0 && (storept[j].x + w) < firstimg.Width() && (storept[j].y + ww) < firstimg.Height())
								{
									zxx += biinter(gradx, storept[j].x + w, storept[j].y + ww)*biinter(gradx, storept[j].x + w, storept[j].y + ww);
									zxy += biinter(gradx, storept[j].x + w, storept[j].y + ww)*biinter(grady, storept[j].x + w, storept[j].y + ww);
									zyy += biinter(grady, storept[j].x + w, storept[j].y + ww)*biinter(grady, storept[j].x + w, storept[j].y + ww);
								}
							}

						for (int w = -(windsize - 1) / 2; w <= (windsize - 1) / 2; w++)
							for (int ww = -(windsize - 1) / 2; ww <= (windsize - 1) / 2;ww++)
							{
								if ((storept[j].x + w) >= 0 && (storept[j].y + ww) >= 0 && (storept[j].x + w) < firstimg.Width() && (storept[j].y + ww) < firstimg.Height() && (storept[j].x + w + u) >= 0 && (storept[j].y + ww + v) >= 0 && (storept[j].x + w + u) < firstimg.Width() && (storept[j].y + ww + v) < firstimg.Height())
								{			// calculating error. 
									errx += biinter(gradx, storept[j].x + w, storept[j].y + ww)*(biinter(ithfimg, storept[j].x + w, storept[j].y + ww) - biinter(iplusfimg, storept[j].x + w + u, storept[j].y + ww + v));
									erry += biinter(grady, storept[j].x + w, storept[j].y + ww)*(biinter(ithfimg, storept[j].x + w, storept[j].y + ww) - biinter(iplusfimg, storept[j].x + w + u, storept[j].y + ww + v));
								}
							}
						float det = 0;
						det = zxx*zyy - zxy*zxy;
						if (det != 0)
						{
							u += (1 / det) * ((zyy*errx) - (zxy*erry)); // updating the u and v 
							v += (1 / det) * ((zxx*erry) - (zxy*errx));

						}


					}
					storept[j].x = storept[j].x + u;					// updating the good features. 
					storept[j].y = storept[j].y + v;

				}
						Convert(iplusfimg, &output);
						for (int newj = 0; newj < count;newj++)
						{
							for (int w = -1; w < 2;w++)
								for (int ww = -1;ww < 2; ww++)
								{
									if (storept[newj].x +w>= 0 && storept[newj].y+ww >= 0 && storept[newj].x +w< firstimg.Width() && storept[newj].y +ww< firstimg.Height())
										output(storept[newj].x+w, storept[newj].y+ww).r = 255; // motion tracking. S
								}
						}
						fig3.Draw(output);

			}
		}
	}
	catch (const Exception& e)
	{
		e.Display();
	}

	EventLoop();
	return 0;
}





