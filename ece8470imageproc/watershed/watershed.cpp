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
void WaterShedAlg(const ImgGray& img, ImgInt& labels, bool marker_based)
{
	int next_label = 0;
	labels.Reset(img.Width(), img.Height());
	ImgInt iimg(img.Width(),img.Height());
	Convert(img, &iimg);
	Set(&labels, -1);

	// Precompute array of pixels for each graylevel
	// (pixels[g] contains the coordinates of all the pixels with graylevel g)
	Array<Point> pixels[256];
	if (marker_based == 1)
	{
		for (int y = 0; y < img.Height(); y++)
			for (int x = 0; x < img.Width(); x++)
				pixels[img(x, y)].Push(Point(x, y));

	}
	else {
		for (int y = 0; y < img.Height(); y++)
			for (int x = 0; x < img.Width(); x++)
				pixels[img(x, y)].Push(Point(x, y));
	}
	// For each graylevel, ...
	
	std::queue <Point> frontier;
	int i;
	for (int g = 0; g < 256; g++)
	{

		// Create frontier of all the pixels with graylevel g that border a 
		// pixel in an existing catchment basin
		
		for (i = 0; i < pixels[g].Len(); i++)
		{
			 Point& pt = pixels[g][i];

			int w = labels.Width() - 1;
			int h = labels.Height() - 1;

			if (pt.x > 0 && labels(pt.x - 1, pt.y) >= 0)
			{
				labels(pt.x, pt.y) = labels(pt.x - 1, pt.y);
				frontier.push(Point(pt.x, pt.y));
			}
			if (pt.y > 0 && labels(pt.x, pt.y - 1) >= 0)
			{
				labels(pt.x, pt.y) = labels(pt.x , pt.y-1);
				frontier.push(Point(pt.x, pt.y));
			}
			if (pt.x < w && labels(pt.x + 1, pt.y) >= 0)
			{
				labels(pt.x, pt.y) = labels(pt.x + 1, pt.y);
				frontier.push(Point(pt.x, pt.y));
			}
			if (pt.y < h && labels(pt.x, pt.y + 1) >= 0)
			{
				labels(pt.x, pt.y) = labels(pt.x, pt.y + 1);
				frontier.push(Point(pt.x, pt.y));
			}

		}

		// Expand all the pixels on the frontier, moving a distance of one pixel
		// each time to correctly handle the situation when a region is adjacent
		// to more than one catchment basin.
		
			while (!frontier.empty())
			{
				Point pt = frontier.front();
				frontier.pop();
				int lab = labels(pt.x, pt.y);
				Point pt2;
				const int w = img.Width() - 1;
				const int h = img.Height() - 1;
				if (pt.x > 0 && img(pt.x - 1, pt.y) <= g && labels(pt.x - 1, pt.y) == -1)
				{
					labels(pt.x - 1, pt.y) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x - 1, pt.y));
				}
				if (pt.y > 0 && img(pt.x, pt.y - 1) <= g && labels(pt.x, pt.y - 1) == -1)
				{
					labels(pt.x, pt.y - 1) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x, pt.y - 1));
				}
				if (pt.x < w && img(pt.x + 1, pt.y) <= g && labels(pt.x + 1, pt.y) == -1)
				{
					labels(pt.x + 1, pt.y) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x + 1, pt.y));
				}
				if (pt.y < h && img(pt.x, pt.y + 1) <= g && labels(pt.x, pt.y + 1) == -1)
				{
					labels(pt.x, pt.y + 1) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x, pt.y + 1));
				}
				if (pt.x > 0 && pt.y > 0 && img(pt.x - 1, pt.y - 1) <= g && labels(pt.x - 1, pt.y - 1) == -1)
				{
					labels(pt.x - 1, pt.y - 1) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x - 1, pt.y - 1));
				}
				if (pt.x < w && pt.y > 0 && img(pt.x + 1, pt.y - 1) <= g && labels(pt.x + 1, pt.y - 1) == -1)
				{
					labels(pt.x + 1, pt.y - 1) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x + 1, pt.y - 1));
				}
				if (pt.x > 0 && pt.y < h && img(pt.x - 1, pt.y + 1) <= g && labels(pt.x - 1, pt.y + 1) == -1)
				{
					labels(pt.x - 1, pt.y + 1) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x - 1, pt.y + 1));
				}
				if (pt.x < w && pt.y < h && img(pt.x + 1, pt.y + 1) <= g && labels(pt.x + 1, pt.y + 1) == -1)
				{
					labels(pt.x + 1, pt.y + 1) = labels(pt.x, pt.y);
					frontier.push(Point(pt.x + 1, pt.y + 1));
				}
			}

		// For each connected region with graylevel g that does not yet belong to a 
		// catchment basin, declare a new catchment basin
		if (!marker_based || g == 0)
		{
			for (i = 0; i < pixels[g].Len(); i++)
			{
			    Point pt = pixels[g][i];
				int x = pt.x;
				int y = pt.y;

				if (labels(x,y) == -1)
				{
					FloodFill8(iimg, pt.x, pt.y, next_label, &labels);
					next_label++;
				}
			}
		}
	}
}



void chamf(ImgBinary& Chmfin, ImgFloat& Chmfout)
{
	int num = Chmfin.Height()*Chmfin.Width() + 1;
	for (int y = 0; y <Chmfin.Height(); y++)
	{
		for (int x = 0; x < Chmfin.Width(); x++)
		{
			if (Chmfin(x, y) == 1)
				Chmfout(x, y) = 0;
			else
			{
				if (x - 1 >= 0 && y - 1 >= 0)
				{
					Chmfout(x, y) = min((1 + Chmfout(x - 1, y)), (1 + Chmfout(x, y - 1)));
				}
				else if (x - 1 < 0 && y - 1 >= 0)
				{
					Chmfout(x, y) = (1 + Chmfout(x, y - 1));
				}
				else if (x - 1 >= 0 && y - 1 < 0)
				{
					Chmfout(x, y) = (1 + Chmfout(x - 1, y));
				}
				else if (x - 1 < 0 && y - 1 < 0)
				{
					Chmfout(x, y) = num;
				}
			}
		}
	}
	float val = 0;
	for (int y = Chmfin.Height() - 1; y >= 0; y--)
	{
		for (int x = Chmfin.Width() - 1; x >= 0; x--)
		{
			if (Chmfin(x, y) == 1)
				Chmfout(x, y) = 0;
			else
			{
				if (x  <= Chmfin.Width() - 2 && y  <= Chmfin.Height() - 2)
				{
					val = min(Chmfout(x, y), min((1 + Chmfout(x + 1, y)), (1 + Chmfout(x, y + 1))));
				}
				else if (x  > Chmfin.Width() - 2 && y <= Chmfin.Height() - 2)
				{
					val = min((1 + Chmfout(x, y)), (1 + Chmfout(x, y + 1)));
				}
				else if (y > Chmfin.Height() - 2 && x <= Chmfin.Width() - 2)
				{
					val = min((1 + Chmfout(x + 1, y)), (1 + Chmfout(x, y)));
				}
				else if (y  > Chmfin.Height() - 2  && x  > Chmfin.Width() - 2)
				{
					Chmfout(x, y) = num;
				}
				Chmfout(x, y) = min(Chmfout(x, y), val);
			}
		}

	}

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

	printf("The suggested threshold value for Holes.pgm is 73  \n The suggested threshold value for cells_small.pgm is 32 ");
	try
	{
		if (argc == 3)
		{
			CString filename1 = argv[1];
			double ttresh = atof(argv[2]);

			CString a = "../../images/";
			CString b = a + filename1;

			// Load image.
			ImgGray firstimg;
			Set(&firstimg, 0);
			Load(b, &firstimg);

			ImgBinary treshimg(firstimg.Width(), firstimg.Height());
			Set(&treshimg, 0);

			for (int y = 0;y < firstimg.Height();y++)
			{
				for (int x = 0;x < firstimg.Width();x++)
				{
					if (firstimg(x, y) > ttresh)
					{
						treshimg(x, y) = 0;
					}
					else
						treshimg(x, y) = 1;
				}
			}
			Figure fig1;
			fig1.Draw(firstimg);
			fig1.SetTitle("Figure 1 - Input Image");

			Figure fig2;
			fig2.Draw(treshimg);
			fig2.SetTitle("Figure 2 - Thresholded Image");

			ImgFloat ChamfImg(firstimg.Width(), firstimg.Height());
			Set(&ChamfImg, 0);
			
			chamf(treshimg, ChamfImg);
			ImgGray ChamfGray(firstimg.Width(), firstimg.Height());
			Set(&ChamfGray, 0);
			Convert(ChamfImg, &ChamfGray);

			Figure fig3;
			fig3.Draw(ChamfGray);
			fig3.SetTitle("Figure 3 - ChamfDist Image");

			double sigma = 1;
			double f = 2.5;
			int mu = round(f*sigma - 0.5);
			int w = 2 * mu + 1;
			double g[100];    // initialzing a gaussian kernel
			double gderi[100]; // initialzing a gaussian derivative kernel
			double sum = 0;

			//Gaussian Kernel
			for (int i = 0;i < w;i++)
			{
				g[i] = exp(-(i - mu)*(i - mu) / (2 * sigma*sigma));
				sum += g[i];
			}
			for (int i = 0;i < w;i++)
			{
				g[i] = g[i] / sum;
			}
			sum = 0;
			ImgFloat gximg(firstimg.Width(), firstimg.Height());
			ImgFloat gyimg(firstimg.Width(), firstimg.Height());
			//Derivative of Gaussian Kernel
			for (int i = 0;i < w;i++)
			{
				gderi[i] = (i - mu)*exp(-(i - mu)*(i - mu) / (2 * sigma*sigma));
				sum += gderi[i] * i;
			}
			for (int i = 0;i < w;i++)
			{
				gderi[i] = gderi[i] / sum;
			}

			ImgFloat tempimg(firstimg.Width(), firstimg.Height());
			// Gy = I*Gy'*Gx

			for (int y = 0;y < firstimg.Height();y++)
				for (int x = mu;x < firstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * firstimg(x + mu - i, y);
						tempimg(x, y) = val;
					}
				}
			for (int y = mu;y < firstimg.Height() - mu;y++)
				for (int x = 0;x < firstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * tempimg(x, y + mu - i);
						gyimg(x, y) = -1 * val;
					}
				}
			Set(&tempimg, 0);
			// Gx = I*Gx'*Gy
			for (int y = 0;y < firstimg.Height();y++)
				for (int x = mu;x < firstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * firstimg(x + mu - i, y);
						tempimg(x, y) = val;
					}
				}
			for (int y = mu;y < firstimg.Height() - mu;y++)
				for (int x = 0;x < firstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * tempimg(x, y + mu - i);
						gximg(x, y) =  val;
					}
				}

			ImgFloat Gmag(firstimg.Width(), firstimg.Height());
			Set(&Gmag, 0);
			// Calculating the Magnitude and Phase Images 
			for (int y = 0;y < firstimg.Height();y++)
				for (int x = 0;x < firstimg.Width(); x++)
				{
					Gmag(x, y) = sqrt((gximg(x, y))*(gximg(x, y)) + (gyimg(x, y))*(gyimg(x, y)));
				}


			ImgGray GmagGray(firstimg.Width(), firstimg.Height());
			Set(&GmagGray, 0);
			Convert(Gmag, &GmagGray);
			



			//Figure fig4;
			//fig4.Draw(Gmag);
			//fig4.SetTitle("Figure 4 - Gradient Magnitude Image");
			ImgInt WithoutMarkers;

			WaterShedAlg(ChamfGray, WithoutMarkers, 0);




			Figure fig4;
			fig4.Draw(WithoutMarkers);
			fig4.SetTitle("Figure 4 - Watershed Image");

			ImgBinary edgeimg(firstimg.Width(), firstimg.Height());
			Set(&edgeimg, 0);

			for (int y = 0; y < firstimg.Height()-1;y++)
				for (int x = 0; x < firstimg.Width()-1;x++)
				{
					if (WithoutMarkers(x, y) != WithoutMarkers(x + 1, y))
					{
						edgeimg(x, y) = 1;

					}
					else if (WithoutMarkers(x, y) != WithoutMarkers(x, y + 1))
						edgeimg(x, y) = 1;
				}


			Figure fig5;
			fig5.Draw(edgeimg);
			fig5.SetTitle("Figure 5 - Detected Edges of Non Marker Image");

			Figure fig6;
			fig6.Draw(Gmag);
			fig6.SetTitle("Figure 6 - Gradient Magnitude Image");

			ImgBinary ImgMark(firstimg.Width(),firstimg.Height());

				Or(edgeimg, treshimg, &ImgMark);

				Figure fig7;
				fig7.Draw(ImgMark);
				fig7.SetTitle("Figure 7 - Edge Marker Image");

				

				for (int y = 0; y < firstimg.Height();y++)
					for (int x = 0; x < firstimg.Width();x++)
					{
						if (ImgMark(x, y) == 1)
							GmagGray(x, y) = 0;
						else
							if (GmagGray(x, y) < 1)
							{
								GmagGray(x, y) = 1;
							}
					}
				ImgInt labelsNew(Gmag.Width(), Gmag.Height());
				WaterShedAlg(GmagGray, labelsNew, 1);


				Figure fig8;
				fig8.Draw(labelsNew);
				fig8.SetTitle("Figure 8 - Edge Marker Image");

				ImgBgr firstimg_bgr(firstimg.Width(), firstimg.Height());
				Convert(firstimg, &firstimg_bgr);
				for (int y = 0; y < firstimg.Height() - 1; y++)
				{
					for (int x = 0; x < firstimg.Width() - 1; x++)
					{
						if (labelsNew(x, y) != labelsNew(x + 1, y))
						{
							firstimg_bgr(x + 1, y).b = 0;
							firstimg_bgr(x + 1, y).g = 255;
							firstimg_bgr(x + 1, y).r = 0;
							firstimg_bgr(x, y).b = 0;
							firstimg_bgr(x, y).g = 255;
							firstimg_bgr(x, y).r = 0;
						}
						if (labelsNew(x, y) != labelsNew(x, y + 1))
						{
							firstimg_bgr(x, y + 1).b = 0;
							firstimg_bgr(x, y + 1).g = 255;
							firstimg_bgr(x, y + 1).r = 0;
							firstimg_bgr(x, y).b = 0;
							firstimg_bgr(x, y).g = 255;
							firstimg_bgr(x, y).r = 0;
						}
					}
				}

				Figure fig9; 
				fig9.Draw(firstimg_bgr);
				fig9.SetTitle("Detected Image");


		}
	}

	catch (const Exception& e)
	{
		e.Display();
	}

	EventLoop();
	return 0;
}





