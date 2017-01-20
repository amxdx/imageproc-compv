// Homework.cpp : Defines the entry point for the console application.
//

#include <afxwin.h>  // necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace blepo;

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
		if (argc == 3)

		{


			double sigma = atof(argv[1]);
			CString filename1 = argv[2];


			CString a = "../../images/";
			CString b = a + filename1;

			// Load image.
			ImgGray firstimg;

			Load(b, &firstimg);
			ImgFloat ffirstimg(firstimg.Width(), firstimg.Height());
			ImgFloat tempimg(firstimg.Width(), firstimg.Height());
			Set(&ffirstimg, 0);
			Set(&tempimg, 0);
			ImgFloat gyimg(firstimg.Width(), firstimg.Height());
			Set(&gyimg, 0);
			ImgFloat gximg(firstimg.Width(), firstimg.Height());
			Set(&gximg, 0);
			Convert(firstimg, &ffirstimg);
			ImgFloat gxgray(firstimg.Width(), firstimg.Height());
			Set(&gxgray, 0);
			ImgFloat gygray(firstimg.Width(), firstimg.Height());
			Set(&gygray, 0);
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
				printf("g[%d]= %f \n", i, g[i]);
			}
			sum = 0;
			//Derivative of Gaussian Kernel
			for (int i = 0;i < w;i++)
			{
				gderi[i] = (i - mu)*exp(-(i - mu)*(i - mu) / (2 * sigma*sigma));
				sum += gderi[i] * i;
			}
			for (int i = 0;i < w;i++)
			{
				gderi[i] = gderi[i] / sum;
				printf("gderi[%d]= %f \n", i, gderi[i]);
			}


			// Gy = I*Gy'*Gx

			for (int y = 0;y < ffirstimg.Height();y++)
				for (int x = mu;x < ffirstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * ffirstimg(x + mu - i, y);
						tempimg(x, y) = val;
					}
				}
			for (int y = mu;y < ffirstimg.Height() - mu;y++)
				for (int x = 0;x < ffirstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * tempimg(x, y + mu - i);
						gyimg(x, y) = -1 * val;
					}
				}

			// Gx = I*Gx'*Gy
			for (int y = 0;y < ffirstimg.Height();y++)
				for (int x = mu;x < ffirstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * ffirstimg(x + mu - i, y);
						tempimg(x, y) = val;
					}
				}
			for (int y = mu;y < ffirstimg.Height() - mu;y++)
				for (int x = 0;x < ffirstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * tempimg(x, y + mu - i);
						gximg(x, y) = -1 * val;
					}
				}

			Figure fig1;
			fig1.Draw(firstimg);
			fig1.SetTitle("Figure 1 - Input Image");
			Figure fig2;
			fig2.Draw(gximg);   //Gx Gradient Image
			fig2.SetTitle("Figure 2 - Gx Gradient Image");
			Figure fig3;
			fig3.Draw(gyimg);   //Gy Gradient Image
			fig3.SetTitle("Figure 3 - Gy Gradient Image");

			ImgFloat Gmag(firstimg.Width(), firstimg.Height());
			Set(&Gmag, 0);
			ImgFloat Gpp(firstimg.Width(), firstimg.Height());
			Set(&Gpp, 0);
			// Calculating the Magnitude and Phase Images 
			for (int y = 0;y < ffirstimg.Height();y++)
				for (int x = 0;x < ffirstimg.Width(); x++)
				{
					Gmag(x, y) = max(abs(gximg(x, y)), abs(gyimg(x, y)));
					Gpp(x, y) = atan2(gyimg(x, y), gximg(x, y));
				}



			Figure fig4;
			fig4.Draw(Gmag);
			fig4.SetTitle("Figure 4 - Magnitude Image");
			Figure fig5;
			fig5.Draw(Gpp);
			fig5.SetTitle("Figure 5 - Phase Image");

			float pi = 3.141592;
			ImgFloat Gsupp(firstimg.Width(), firstimg.Height());
			Set(&Gsupp, 0);
			ImgFloat Gphase(firstimg.Width(), firstimg.Height());
			Set(&Gphase, 0);
			Gsupp = Gmag;
			Gphase = Gpp;

			for (int y = 1;y < ffirstimg.Height() - 1;y++)
				for (int x = 1;x < ffirstimg.Width() - 1; x++)
				{

					if ((Gphase(x, y) >= (float)(-1 * (pi)) && Gphase(x, y) < (float)(-1 * 7 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y) || Gmag(x, y) < Gmag(x + 1, y))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(-1 * 7 * pi / 8) && Gphase(x, y) < (float)(-1 * 5 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y - 1) || Gmag(x, y) < Gmag(x + 1, y + 1))
							Gsupp(x, y) = 0;
						else {}

					else if ((Gphase(x, y) >= (float)(-5 * pi / 8) && Gphase(x, y) < (float)(-3 * pi / 8)))
						if (Gmag(x, y) < Gmag(x, y - 1) || Gmag(x, y) < Gmag(x, y + 1))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(-3 * pi / 8) && Gphase(x, y) < (float)(-1 * pi / 8)))
						if (Gmag(x, y) < Gmag(x + 1, y - 1) || Gmag(x, y) < Gmag(x - 1, y + 1))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(-1 * pi / 8) && Gphase(x, y) < (float)(pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y) || Gmag(x, y) < Gmag(x + 1, y))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(pi / 8) && Gphase(x, y) < (float)(3 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y - 1) || Gmag(x, y) < Gmag(x + 1, y + 1))
							Gsupp(x, y) = 0;
						else {}

					else if ((Gphase(x, y) >= (float)(3 * pi / 8) && Gphase(x, y) < (float)(5 * pi / 8)))
						if (Gmag(x, y) < Gmag(x, y - 1) || Gmag(x, y) < Gmag(x, y + 1))
							Gsupp(x, y) = 0;
						else {}

					else if ((Gphase(x, y) >= (float)(5 * pi / 8) && Gphase(x, y) < (float)(7 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y + 1) || Gmag(x, y) < Gmag(x + 1, y - 1))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(7 * pi / 8) && Gphase(x, y) < (float)(pi)))
						if (Gmag(x, y) < Gmag(x - 1, y) || Gmag(x, y) < Gmag(x + 1, y))
							Gsupp(x, y) = 0;
						else {}
				}

			Figure fig6;
			fig6.Draw(Gsupp);
			fig6.SetTitle("Figure 6 - Non Maximal Suppressed Image");

			ImgFloat sortimg(firstimg.Width(), firstimg.Height());
			Set(&sortimg, 0);
			sortimg = Gsupp;

			std::sort(sortimg.Begin(), sortimg.End());
			int count = 0;
			for (int y = 0; y < firstimg.Height();y++)
			{
				for (int x = 0; x < firstimg.Width();x++)
					if (sortimg(x, y) > 0)
						count++;
			}

			printf("count = %d", count);

			int Th = 0.9*count;
			int count1 = 0;
			float temptresh = 0;
			for (int y = 0; y < firstimg.Height();y++)
			{
				for (int x = 0; x < firstimg.Width();x++)
				{
					if (sortimg(x, y) > 0)
					{
						if (count1 == Th)
						{
							temptresh = sortimg(x, y);
							count1++;
						}
						else
							count1++;
					}
				}
			}

			printf("High Threshold = %f", temptresh);
			Th = temptresh;
			float Lh = 0;
			Lh = Th / 5;

			ImgBinary HighThresh;
			Set(&HighThresh, 0);
			ImgBinary LowThresh;
			Set(&LowThresh, 0);

			Threshold(Gsupp, Th, &HighThresh);
			Threshold(Gsupp, Lh, &LowThresh);
			ImgBinary FloodImg;
			Set(&FloodImg, 0);
			FloodImg = HighThresh;

			//Floodfill Algorithm

			for (int y = 1; y < firstimg.Height() - 1;y++)
			{
				for (int x = 1; x < firstimg.Width() - 1;x++)
				{
					for (int y1 = -1;y1 < 2;y1++)
					{
						for (int x1 = -1;x1 < 2;x1++)
						{
							if (FloodImg(x, y) == 1)
							{
								if (Gsupp(x + x1, y + y1) >= Lh)
								{
									FloodImg(x + x1, y + y1) = 1;
								}
							}
						}
					}
				}
			}

			Figure fig7;
			fig7.Draw(HighThresh);
			fig7.SetTitle("Figure 7 - HighThreshold Image");

			Figure fig8;
			fig8.Draw(LowThresh);
			fig8.SetTitle("Figure 8 - Low Threshold Image");

			Figure fig9;
			fig9.Draw(FloodImg);
			fig9.SetTitle("Figure 9 - FloodFilled Image");

			ImgFloat ChamfImg(firstimg.Width(), firstimg.Height());
			Set(&ChamfImg, 0);
			ImgFloat ChamfImg1(firstimg.Width(), firstimg.Height());
			Set(&ChamfImg1, 0);
			float temp1 = 0, temp2 = 0;

			//Chamfer Distance Calculation 

			for (int y = 0; y < firstimg.Height();y++)
				for (int x = 0;x < firstimg.Width();x++)
				{
					if (FloodImg(x, y) == 1)
						ChamfImg(x, y) = 0;
					else
					{
						temp1 = min((float)(firstimg.Height()*firstimg.Width()), 1 + ChamfImg(x - 1, y));
						ChamfImg(x, y) = min(temp1, 1 + ChamfImg(x, y - 1));
					}

				}
			ChamfImg1 = ChamfImg;
			for (int y = firstimg.Height() - 1; y > 0;y--)
				for (int x = firstimg.Width() - 1;x > 0;x--)
				{
					if (FloodImg(x, y) == 1)
						ChamfImg1(x, y) = 0;
					else
					{
						temp2 = min(ChamfImg1(x, y), 1 + ChamfImg1(x - 1, y));
						ChamfImg1(x, y) = min(temp2, 1 + ChamfImg1(x, y - 1));

					}
				}

			Figure fig10;
			fig10.Draw(ChamfImg1);
			fig10.SetTitle("Figure 10 - Chamfer Distance Image");
		}
		else if (argc == 4)
		{		
					


			double sigma = atof(argv[1]);
			CString filename1 = argv[2];
			CString filename2 = argv[3];

			CString a = "../../images/";
			CString b = a + filename1;
			CString c = a + filename2;
			// Load image.
			ImgGray firstimg;
			ImgGray tfirstimg;
			
			Load(b, &firstimg);
			Load(c, &tfirstimg);

			ImgFloat ffirstimg(firstimg.Width(), firstimg.Height());
			ImgFloat tempimg(firstimg.Width(), firstimg.Height());
			Set(&ffirstimg, 0);
			Set(&tempimg, 0);
			ImgFloat gyimg(firstimg.Width(), firstimg.Height());
			Set(&gyimg, 0);
			ImgFloat gximg(firstimg.Width(), firstimg.Height());
			Set(&gximg, 0);
			Convert(firstimg, &ffirstimg);
			ImgFloat gxgray(firstimg.Width(), firstimg.Height());
			Set(&gxgray, 0);
			ImgFloat gygray(firstimg.Width(), firstimg.Height());
			Set(&gygray, 0);

			// template
			ImgFloat tffirstimg(tfirstimg.Width(), tfirstimg.Height());
			ImgFloat ttempimg(tfirstimg.Width(), tfirstimg.Height());
			Set(&tffirstimg, 0);
			Set(&ttempimg, 0);
			ImgFloat tgyimg(tfirstimg.Width(), tfirstimg.Height());
			Set(&tgyimg, 0);
			ImgFloat tgximg(tfirstimg.Width(), tfirstimg.Height());
			Set(&tgximg, 0);
			Convert(tfirstimg, &tffirstimg);
			ImgFloat tgxgray(tfirstimg.Width(), tfirstimg.Height());
			Set(&tgxgray, 0);
			ImgFloat tgygray(tfirstimg.Width(), tfirstimg.Height());
			Set(&tgygray, 0);

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
				printf("g[%d]= %f \n", i, g[i]);
			}
			sum = 0;
			//Derivative of Gaussian Kernel
			for (int i = 0;i < w;i++)
			{
				gderi[i] = (i - mu)*exp(-(i - mu)*(i - mu) / (2 * sigma*sigma));
				sum += gderi[i] * i;
			}
			for (int i = 0;i < w;i++)
			{
				gderi[i] = gderi[i] / sum;
				printf("gderi[%d]= %f \n", i, gderi[i]);
			}


			// Gy = I*Gy'*Gx

			for (int y = 0;y < ffirstimg.Height();y++)
				for (int x = mu;x < ffirstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * ffirstimg(x + mu - i, y);
						tempimg(x, y) = val;
					}
				}
			for (int y = mu;y < ffirstimg.Height() - mu;y++)
				for (int x = 0;x < ffirstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * tempimg(x, y + mu - i);
						gyimg(x, y) = -1 * val;
					}
				}

			// Gx = I*Gx'*Gy
			for (int y = 0;y < ffirstimg.Height();y++)
				for (int x = mu;x < ffirstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * ffirstimg(x + mu - i, y);
						tempimg(x, y) = val;
					}
				}
			for (int y = mu;y < ffirstimg.Height() - mu;y++)
				for (int x = 0;x < ffirstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * tempimg(x, y + mu - i);
						gximg(x, y) = -1 * val;
					}
				}


			//template
			for (int y = 0;y < tffirstimg.Height();y++)
				for (int x = mu;x < tffirstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * tffirstimg(x + mu - i, y);
						ttempimg(x, y) = val;
					}
				}
			for (int y = mu;y < tffirstimg.Height() - mu;y++)
				for (int x = 0;x < tffirstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * ttempimg(x, y + mu - i);
						tgyimg(x, y) = -1 * val;
					}
				}

			// Gx = I*Gx'*Gy
			for (int y = 0;y < tffirstimg.Height();y++)
				for (int x = mu;x < tffirstimg.Width() - mu; x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += gderi[i] * tffirstimg(x + mu - i, y);
						ttempimg(x, y) = val;
					}
				}
			for (int y = mu;y < tffirstimg.Height() - mu;y++)
				for (int x = 0;x < tffirstimg.Width(); x++)
				{
					float val = 0;
					for (int i = 0;i < w;i++)
					{
						val += g[i] * ttempimg(x, y + mu - i);
						tgximg(x, y) = -1 * val;
					}
				}



			Figure fig1;
			fig1.Draw(firstimg);
			fig1.SetTitle("Figure 1 - Input Image");
			Figure fig2;
			fig2.Draw(gximg);   //Gx Gradient Image
			fig2.SetTitle("Figure 2 - Gx Gradient Image");
			Figure fig3;
			fig3.Draw(gyimg);   //Gy Gradient Image
			fig3.SetTitle("Figure 3 - Gy Gradient Image");

			ImgFloat Gmag(firstimg.Width(), firstimg.Height());
			Set(&Gmag, 0);
			ImgFloat Gpp(firstimg.Width(), firstimg.Height());
			Set(&Gpp, 0);
			// Calculating the Magnitude and Phase Images 
			for (int y = 0;y < ffirstimg.Height();y++)
				for (int x = 0;x < ffirstimg.Width(); x++)
				{
					Gmag(x, y) = max(abs(gximg(x, y)), abs(gyimg(x, y)));
					Gpp(x, y) = atan2(gyimg(x, y), gximg(x, y));
				}

			//template

			ImgFloat tGmag(tfirstimg.Width(), tfirstimg.Height());
			Set(&tGmag, 0);
			ImgFloat tGpp(tfirstimg.Width(), tfirstimg.Height());
			Set(&tGpp, 0);
			// Calculating the Magnitude and Phase Images 
			for (int y = 0;y < tffirstimg.Height();y++)
				for (int x = 0;x < tffirstimg.Width(); x++)
				{
					tGmag(x, y) = max(abs(tgximg(x, y)), abs(tgyimg(x, y)));
					tGpp(x, y) = atan2(tgyimg(x, y), tgximg(x, y));
				}


			Figure fig4;
			fig4.Draw(Gmag);
			fig4.SetTitle("Figure 4 - Magnitude Image");
			Figure fig5;
			fig5.Draw(Gpp);
			fig5.SetTitle("Figure 5 - Phase Image");

			float pi = 3.141592;
			ImgFloat Gsupp(firstimg.Width(), firstimg.Height());
			Set(&Gsupp, 0);
			ImgFloat Gphase(firstimg.Width(), firstimg.Height());
			Set(&Gphase, 0);
			Gsupp = Gmag;
			Gphase = Gpp;

			//template 

			ImgFloat tGsupp(tfirstimg.Width(), tfirstimg.Height());
			Set(&tGsupp, 0);
			ImgFloat tGphase(tfirstimg.Width(), tfirstimg.Height());
			Set(&tGphase, 0);
			tGsupp = tGmag;
			tGphase = tGpp;


			for (int y = 1;y < ffirstimg.Height() - 1;y++)
				for (int x = 1;x < ffirstimg.Width() - 1; x++)
				{

					if ((Gphase(x, y) >= (float)(-1 * (pi)) && Gphase(x, y) < (float)(-1 * 7 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y) || Gmag(x, y) < Gmag(x + 1, y))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(-1 * 7 * pi / 8) && Gphase(x, y) < (float)(-1 * 5 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y - 1) || Gmag(x, y) < Gmag(x + 1, y + 1))
							Gsupp(x, y) = 0;
						else {}

					else if ((Gphase(x, y) >= (float)(-5 * pi / 8) && Gphase(x, y) < (float)(-3 * pi / 8)))
						if (Gmag(x, y) < Gmag(x, y - 1) || Gmag(x, y) < Gmag(x, y + 1))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(-3 * pi / 8) && Gphase(x, y) < (float)(-1 * pi / 8)))
						if (Gmag(x, y) < Gmag(x + 1, y - 1) || Gmag(x, y) < Gmag(x - 1, y + 1))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(-1 * pi / 8) && Gphase(x, y) < (float)(pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y) || Gmag(x, y) < Gmag(x + 1, y))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(pi / 8) && Gphase(x, y) < (float)(3 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y - 1) || Gmag(x, y) < Gmag(x + 1, y + 1))
							Gsupp(x, y) = 0;
						else {}

					else if ((Gphase(x, y) >= (float)(3 * pi / 8) && Gphase(x, y) < (float)(5 * pi / 8)))
						if (Gmag(x, y) < Gmag(x, y - 1) || Gmag(x, y) < Gmag(x, y + 1))
							Gsupp(x, y) = 0;
						else {}

					else if ((Gphase(x, y) >= (float)(5 * pi / 8) && Gphase(x, y) < (float)(7 * pi / 8)))
						if (Gmag(x, y) < Gmag(x - 1, y + 1) || Gmag(x, y) < Gmag(x + 1, y - 1))
							Gsupp(x, y) = 0;
						else {}
					else if ((Gphase(x, y) >= (float)(7 * pi / 8) && Gphase(x, y) < (float)(pi)))
						if (Gmag(x, y) < Gmag(x - 1, y) || Gmag(x, y) < Gmag(x + 1, y))
							Gsupp(x, y) = 0;
						else {}
				}

			//template 

			for (int y = 1;y < tffirstimg.Height() - 1;y++)
				for (int x = 1;x < tffirstimg.Width() - 1; x++)
				{

					if ((tGphase(x, y) >= (float)(-1 * (pi)) && tGphase(x, y) < (float)(-1 * 7 * pi / 8)))
						if (tGmag(x, y) < tGmag(x - 1, y) || tGmag(x, y) < tGmag(x + 1, y))
							tGsupp(x, y) = 0;
						else {}
					else if ((tGphase(x, y) >= (float)(-1 * 7 * pi / 8) && tGphase(x, y) < (float)(-1 * 5 * pi / 8)))
						if (tGmag(x, y) < tGmag(x - 1, y - 1) || tGmag(x, y) < tGmag(x + 1, y + 1))
							tGsupp(x, y) = 0;
						else {}

					else if ((tGphase(x, y) >= (float)(-5 * pi / 8) && tGphase(x, y) < (float)(-3 * pi / 8)))
						if (tGmag(x, y) < tGmag(x, y - 1) || tGmag(x, y) < tGmag(x, y + 1))
							tGsupp(x, y) = 0;
						else {}
					else if ((tGphase(x, y) >= (float)(-3 * pi / 8) && tGphase(x, y) < (float)(-1 * pi / 8)))
						if (tGmag(x, y) < tGmag(x + 1, y - 1) || tGmag(x, y) < tGmag(x - 1, y + 1))
							tGsupp(x, y) = 0;
						else {}
					else if ((tGphase(x, y) >= (float)(-1 * pi / 8) && tGphase(x, y) < (float)(pi / 8)))
						if (tGmag(x, y) < tGmag(x - 1, y) || tGmag(x, y) < tGmag(x + 1, y))
							tGsupp(x, y) = 0;
						else {}
					else if ((tGphase(x, y) >= (float)(pi / 8) && tGphase(x, y) < (float)(3 * pi / 8)))
						if (tGmag(x, y) < tGmag(x - 1, y - 1) || tGmag(x, y) < tGmag(x + 1, y + 1))
							tGsupp(x, y) = 0;
						else {}

					else if ((tGphase(x, y) >= (float)(3 * pi / 8) && tGphase(x, y) < (float)(5 * pi / 8)))
						if (tGmag(x, y) < tGmag(x, y - 1) || tGmag(x, y) < tGmag(x, y + 1))
							tGsupp(x, y) = 0;
						else {}

					else if ((tGphase(x, y) >= (float)(5 * pi / 8) && tGphase(x, y) < (float)(7 * pi / 8)))
						if (tGmag(x, y) < tGmag(x - 1, y + 1) || tGmag(x, y) < tGmag(x + 1, y - 1))
							tGsupp(x, y) = 0;
						else {}
					else if ((tGphase(x, y) >= (float)(7 * pi / 8) && tGphase(x, y) < (float)(pi)))
						if (tGmag(x, y) < tGmag(x - 1, y) || tGmag(x, y) < tGmag(x + 1, y))
							tGsupp(x, y) = 0;
						else {}
				}

			Figure fig6;
			fig6.Draw(Gsupp);
			fig6.SetTitle("Figure 6 - Non Maximal Suppressed Image");

			ImgFloat sortimg(firstimg.Width(), firstimg.Height());
			Set(&sortimg, 0);
			sortimg = Gsupp;

			std::sort(sortimg.Begin(), sortimg.End());
			int count = 0;
			for (int y = 0; y < firstimg.Height();y++)
			{
				for (int x = 0; x < firstimg.Width();x++)
					if (sortimg(x, y) > 0)
						count++;
			}

			printf("count = %d", count);

			float Th = 0.9*count;
			int count1 = 0;
			float temptresh = 0;
			for (int y = 0; y < firstimg.Height();y++)
			{
				for (int x = 0; x < firstimg.Width();x++)
					if (sortimg(x, y)>0)
					{
						if (count1 == Th)
						{
							temptresh = sortimg(x, y);
							count1++;
						}
						else
							count1++;
					}
			}

			printf("High Threshold = %f", temptresh);
			Th = temptresh;
			float Lh = 0;
			Lh = Th / 5;

			ImgBinary HighThresh;
			Set(&HighThresh, 0);
			ImgBinary LowThresh;
			Set(&LowThresh, 0);

			Threshold(Gsupp, Th, &HighThresh);
			Threshold(Gsupp, Lh, &LowThresh);
			ImgBinary FloodImg;
			Set(&FloodImg, 0);
			FloodImg = HighThresh;

			
			//template

			ImgFloat tsortimg(tfirstimg.Width(), tfirstimg.Height());
			Set(&tsortimg, 0);
			tsortimg = tGsupp;

			std::sort(tsortimg.Begin(), tsortimg.End());
			int tcount = 0;
			for (int y = 0; y < tfirstimg.Height();y++)
			{
				for (int x = 0; x < tfirstimg.Width();x++)
					if (tsortimg(x, y) > 0)
						tcount++;
			}

			printf("tcount = %d", tcount);

			int tTh = 0.8*tcount;
			int tcount1 = 0;
			float ttemptresh = 0;
			for (int y = 0; y < tfirstimg.Height();y++)
			{
				for (int x = 0; x < tfirstimg.Width();x++)
					if (tsortimg(x, y)>0)
					{
						if (tcount1 == tTh)
						{
							ttemptresh = tsortimg(x, y);
							tcount1++;
						}
						else
							tcount1++;
					}
			}

			printf("High Threshold = %f", ttemptresh);
			tTh = ttemptresh;
			float tLh = 0;
			tLh = tTh / 5;

			ImgBinary tHighThresh;
			Set(&tHighThresh, 0);
			ImgBinary tLowThresh;
			Set(&tLowThresh, 0);

			Threshold(tGsupp, tTh, &tHighThresh);
			Threshold(tGsupp, tLh, &tLowThresh);
			ImgBinary tFloodImg;
			Set(&tFloodImg, 0);
			tFloodImg = tHighThresh;


			//Floodfill Algorithm

			for (int y = 1; y < firstimg.Height() - 1;y++)
			{
				for (int x = 1; x < firstimg.Width() - 1;x++)
				{
					for (int y1 = -1;y1 < 2;y1++)
					{
						for (int x1 = -1;x1 < 2;x1++)
						{
							if (FloodImg(x, y) == 1)
							{
								if (Gsupp(x + x1, y + y1) >= Lh)
								{
									FloodImg(x + x1, y + y1) = 1;
								}
							}
						}
					}
				}
			}


			//template floodfill 

			for (int y = 1; y < tfirstimg.Height() - 1;y++)
			{
				for (int x = 1; x < tfirstimg.Width() - 1;x++)
				{
					for (int y1 = -1;y1 < 2;y1++)
					{
						for (int x1 = -1;x1 < 2;x1++)
						{
							if (tFloodImg(x, y) == 1)
							{
								if (tGsupp(x + x1, y + y1) >= tLh)
								{
									tFloodImg(x + x1, y + y1) = 1;
								}
							}
						}
					}
				}
			}

			Figure fig7;
			fig7.Draw(HighThresh);
			fig7.SetTitle("Figure 7 - HighThreshold Image");

			Figure fig8;
			fig8.Draw(LowThresh);
			fig8.SetTitle("Figure 8 - Low Threshold Image");

			Figure fig9;
			fig9.Draw(FloodImg);
			fig9.SetTitle("Figure 9 - FloodFilled Image");

			ImgFloat ChamfImg(firstimg.Width(), firstimg.Height());
			Set(&ChamfImg, 0);
			float temp1 = 0, temp2 = 0;

			//Chamfer Distance Calculation 

			for (int y = 0; y < firstimg.Height();y++)
				for (int x = 0;x < firstimg.Width();x++)
				{
					if (FloodImg(x, y) == 1)
						ChamfImg(x, y) = 0;
					else
					{
						temp1 = min((float)(firstimg.Height()*firstimg.Width()), 1 + ChamfImg(x - 1, y));
						ChamfImg(x, y) = min(temp1, 1 + ChamfImg(x, y - 1));
					}

				}

			ImgFloat ChamfImg1(firstimg.Width(), firstimg.Height());
			Set(&ChamfImg1, 0);
			ChamfImg1 = ChamfImg;

			for (int y = firstimg.Height() - 1; y > 0;y--)
				for (int x = firstimg.Width() - 1;x > 0;x--)
				{
					if (FloodImg(x, y) == 1)
						ChamfImg1(x, y) = 0;
					else
					{
						temp2 = min(ChamfImg1(x, y), 1 + ChamfImg1(x - 1, y));
						ChamfImg1(x, y) = min(temp2, 1 + ChamfImg1(x, y - 1));

					}
				}

			printf(" x =%d y =%d", ChamfImg.Width(), ChamfImg.Height());
			printf("Hello");
			Figure fig10;
			fig10.Draw(ChamfImg1);
			fig10.SetTitle("Figure 10 - Chamfer Distance Image");

			Figure fig11;
			fig11.Draw(tFloodImg);
			fig11.SetTitle("Figure 11 - Canny edge of the template");
			ImgFloat Probmap(firstimg.Width() - tfirstimg.Width(), firstimg.Height() - tfirstimg.Height());
			Set(&Probmap, 255);
			

			for (int y1 =0; y1 < (firstimg.Height() - tfirstimg.Height());y1++)
				for (int x1 = 0; x1 < (firstimg.Width() - tfirstimg.Width());x1++)
				{
				
					for (int y2 = 0;y2 < tfirstimg.Height();y2++)
					{
						for (int x2 = 0;x2 < tfirstimg.Width();x2++)
						{
							if (tFloodImg(x2, y2) == 1)
								Probmap(x1,y1) += ChamfImg1(x1+x2 , y1+y2 );

						}
					}
					
				}

			
			Figure fig12;
			fig12.Draw(Probmap);
			fig12.SetTitle("Figure 12 - Inverse Probability Map");
			
			float max = 0;
			for (int y = 1; y < Probmap.Height() - 1; y++)
			{
				for (int x = 1; x < Probmap.Width() - 1; x++)
				{
					if (Probmap(x, y) > max)
					{
						max = Probmap(x, y);

					}

				}
			}
			float min = max;
			int x3, y3;
			for (int y = 1; y < Probmap.Height() - 1; y++)
			{
				for (int x = 1; x < Probmap.Width() - 1; x++)
				{
					if (Probmap(x, y) < min)
					{
						min = Probmap(x, y);
						x3 = x;
						y3 = y;
					}

				}
			}
			
			Point p1, p2;
			p1.x = x3;
			p1.y = y3;
			p2.x = x3 + tfirstimg.Width();
			p2.y = y3 + tfirstimg.Height();
			Figure Rectangle;
			Rect r(p1, p2);
			ImgBgr InputImg;
			Load(b, &InputImg);

			DrawRect(r, &InputImg, Bgr(0, 255, 0), 1);
			Rectangle.Draw(InputImg);
			Rectangle.SetTitle("Input Image with matched template");





		}
		else
		{
			printf("Error in the Number of Input Arguments");
		}
	}
	catch (const Exception& e)
	{
		e.Display();
	}
	EventLoop();

	return 0;


}




