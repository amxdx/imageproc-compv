// Homework.cpp : Defines the entry point for the console application.
//

#include <afxwin.h>  // necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace blepo;

double m00;              // zeroth-order moment
double m10, m01, m11;    // first-order moments in x and y
double m20, m02;         // second-order moments in x and y
double mu00;             // central zeroth-order moment (always equal to m00) 
double mu10, mu01, mu11; // central first-order moments in x and y (mu10 and mu01 are always zero)
double mu20, mu02;       // central second-order moments in x and y
double area;             // same as m00
Rect bounding_rect;
double xc, yc;           // centroid
double eccentricity;     // ratio of major axis length to minor axis length
double direction;        // angle of major axis with respect to positive x-axis
double major_axis_x, major_axis_y;  // major and minor axes scaled by the standard deviation = sqrt(eigenvalue)
double minor_axis_x, minor_axis_y;


void GiffmeProps(const ImgBinary& img)
{

	int x, y;

	// compute non-central moments and bounding rect
	bounding_rect = Rect(img.Width(), img.Height(), -1, -1);
	m00 = 0;
	m10 = 0;
	m01 = 0;
	m11 = 0;
	m20 = 0;
	m02 = 0;

	for (y = 0; y < img.Height(); y++)
	{
		for (x = 0; x < img.Width(); x++)
		{


			{
				m00 += 1;
				m10 += x;
				m01 += y;
				m11 += x * y;
				m20 += x * x;
				m02 += y * y;
			}
		}
	}

	// area
	area = m00;

	// centroid
	xc = (m10) / (m00);
	yc = (m01) / (m00);

	// central moments
	mu00 = m00;
	mu10 = 0;
	mu01 = 0;
	mu11 = m11 - yc * m10;
	mu20 = m20 - xc *m10;
	mu02 = m02 - yc *m01;

	// eccentricity, or elongatedness
	double lambda1, lambda2;
	{
		double a = mu20 + mu02;
		double b = mu20 - mu02;
		double c = sqrt(4 * mu11 *mu11 + b * b);
		lambda1 = (a + c) / (2 * mu00);
		lambda2 = (a - c) / (2 * mu00);
		eccentricity = sqrt((lambda1 - lambda2) / lambda1);
	}

	// direction
	direction = 0.5 * (atan2(2 * mu11, (mu20 - mu02)));


	// major and minor axes
	major_axis_x = sqrt(lambda1) * cos(direction);
	major_axis_y = sqrt(lambda1) * sin(direction);
	minor_axis_x = sqrt(lambda2) * sin(direction);
	minor_axis_y = sqrt(lambda2) * (-cos(direction));
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

	if (argc != 2)
		printf("Error in number of input arguments");
	else {



		CString filename1 = argv[1];


		CString a = "../../images/";
		CString b = a + filename1;

		// Load image.
		ImgGray firstimg;
		ImgBinary secondimg;
		ImgBinary thirdimg;
		ImgBinary forthimg;

		Load(b, &firstimg);

		// Display image in figure window.
		Figure fig1;
		fig1.Draw(firstimg);
		unsigned char high;
		unsigned char low;
		high = 183; // high threshold value
		low = 115; // low threshold value

				   // creating binary intermediate images



		Threshold(firstimg, high, &secondimg);				// used an upper threshold 
		Threshold(firstimg, low, &thirdimg);				// used a lower threshold
		Figure fig2;
		fig2.Draw(secondimg);

		for (int x = 0; x < firstimg.Width(); x++)
			for (int y = 0; y < firstimg.Height(); y++)
				if (secondimg(x, y) == 1)
					FloodFill4(secondimg, x, y, 1, &thirdimg);				// used floodfill to merge lower and upper thresholds for a better image


																			// used erosion and dialation operations for a better noise reduction of the given fruit images
		Erode3x3Cross(thirdimg, &thirdimg);
		Erode3x3(thirdimg, &thirdimg);
		Dilate3x3(thirdimg, &thirdimg);
		Dilate3x3(thirdimg, &thirdimg);
		Dilate3x3(thirdimg, &thirdimg);
		Erode3x3Cross(thirdimg, &thirdimg);

		ImgInt labels; // labels for the connected component regions

		Figure fig3;
		fig3.Draw(thirdimg);

		ConnectedComponents4(thirdimg, &labels);				 // used connected components to label different fruit regions

		Figure fig4;
		fig4.Draw(labels);

		ImgBinary fifthimg;
		Erode3x3Cross(thirdimg, &forthimg);
		Xor(forthimg, thirdimg, &fifthimg);
		Figure fig5;
		fig5.Draw(fifthimg);
		ImgBgr seventhimg;
		Load(b, &seventhimg);
		int maxlabels = 0;
		maxlabels = Max(labels);                                  // calculating the maximum number of labels
		int perimeter[20];											// initializing the perimeter parameter
																	//int tempperi = 0;			
		ImgBgr eigthimg;
		eigthimg = seventhimg;

		for (int i = 1; i <= Max(labels); i++)
		{
			int tempperi = 0;

			for (int y = 0; y < fifthimg.Height(); y++)
			{
				for (int x = 0; x < fifthimg.Width(); x++)
				{
					if (labels(x, y) == i)
					{
						if (fifthimg(x, y) == 1)
						{
							tempperi += 1;
						}
					}
				}
			}
			perimeter[i] = tempperi;
			printf("Perimeter of Label%d = %d \n", i, perimeter[i]);
			// storing the perimeter 
		}


		for (int i = 1; i <= Max(labels); i++)
		{
			for (int y = 0; y < fifthimg.Height(); y++)
			{
				for (int x = 0; x < fifthimg.Width(); x++)
				{
					if (fifthimg(x, y) == 1 && labels(i) == i)
					{
						if (perimeter[i] > 5000 && perimeter[i] < 5800)
						{

							eigthimg(x, y).r = 255;
							eigthimg(x, y).g = 255;
							eigthimg(x, y).b = 51;
							printf("help me __DEbug Statement");
						}
						else if (perimeter[i] > 5900 && perimeter[i] < 6300)
						{

							eigthimg(x, y).r = 255;
							eigthimg(x, y).g = 0;
							eigthimg(x, y).b = 0;
							printf("help me2____Debug stetement");
						}
						else if (perimeter[i] > 2700 && perimeter[i] < 3200)
						{

							eigthimg(x, y).r = 0;
							eigthimg(x, y).g = 255;
							eigthimg(x, y).b = 0;
							printf("help me3____debug statement");
						}
					}
				}
			}
		}

		Figure fig7;
		fig7.Draw(eigthimg);
		fig7.SetTitle("Output Img");


		/*
		for (int i = 1;i <= Max(labels);i++)
		{
		for (int y = 0; y < fifthimg.Height();y++)
		{
		for (int x = 0;x < fifthimg.Width();x++)
		{
		if (fifthimg(x, y) = 1 && labels(x, y) == i)
		{
		GiffmeProps(fifthimg);

		}
		}

		}
		}
		*/





		/*Save(secondimg, c);

		ImgGray thirdimg;
		Load(d, &thirdimg);
		Figure fig3;
		fig3.Draw(thirdimg);
		ImgBgr forthimg;
		forthimg = firstimg;
		if (thirdimg.Width() != firstimg.Width() || thirdimg.Height() != firstimg.Height())
		{BLEPO_ERROR("The dimensions are not matching, please enter a matched image")
		}

		else
		for (int x1 = 0; x1 < thirdimg.Width(); x1++)
		for (int y1 = 0; y1 < thirdimg.Height(); y1++)
		if (thirdimg(x1, y1) == 0)
		{

		Bgr& pix = forthimg(x1, y1);
		pix.b = 0;
		pix.r = 0;
		pix.g = 0;
		}

		Figure fig4;
		fig4.Draw(forthimg);



		}*/
	}
	// Loop forever until user presses Ctrl-C in terminal window.
	EventLoop();

	return 0;
}

			




