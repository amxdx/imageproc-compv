// Homework.cpp : Defines the entry point for the console application.
//

#include <afxwin.h>  // necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"
#include <queue>
#include <vector>
#include <stdio.h>


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
		if (argc == 4)
		{
			
			
			CString filename1 = argv[1];
			CString filename2 = argv[2];
			double max_disp = atof(argv[3]);

			CString a = "../../images/";
			CString b = a + filename1;
			CString c = a + filename2;

			
			// Load image.
			ImgGray firstimg;
			Set(&firstimg, 0);
			Load(b, &firstimg);
			

			ImgGray secondimg;
			Set(&secondimg, 0);
			Load(c, &secondimg);
			
			vector<vector<vector<int>>>  dispmap(firstimg.Width(), vector<vector<int>>(firstimg.Height(), vector<int>(max_disp)));
			vector<vector<vector<int>>>  dispmap2(firstimg.Width(), vector<vector<int>>(firstimg.Height(), vector<int>(max_disp)));
			vector<vector<vector<int>>>  dissim(firstimg.Width(), vector<vector<int>>(firstimg.Height(), vector<int>(max_disp)));
			vector<vector<vector<int>>>  dissim2(firstimg.Width(), vector<vector<int>>(firstimg.Height(), vector<int>(max_disp)));
			int d = 0;
			for ( d = 0; d < max_disp;d++)
			{
				for (int y = 0;y < firstimg.Height();y++)
				{
					for (int x = d;x < firstimg.Width();x++)
					{
						dispmap[x][y][d]= abs(firstimg(x,y)-secondimg(x-d,y));
					}
				}
			}
			
			Figure fig1;
			fig1.Draw(firstimg);
			fig1.SetTitle("Figure 1 - Input Image - Left");

			Figure fig2;
			fig2.Draw(secondimg);
			fig2.SetTitle("Figure 2 - Input Image - Right");
			int w = 2;
			ImgFloat tempimg(firstimg.Width(),firstimg.Height());
			for (d = 0; d < max_disp;d++)
			{
				for (int y = 0;y < firstimg.Height();y++)
					for (int x = w;x < firstimg.Width() - w; x++)
					{
						float val = 0;
						for (int i = -w;i < w + 1;i++)
						{
							val += dispmap[x - i][y][d];
							tempimg(x, y) = val;
						}
					}
				for (int y = w;y < firstimg.Height() - w;y++)
					for (int x = 0;x < firstimg.Width(); x++)
					{
						float val = 0;
						for (int i = -w;i < w+1;i++)
						{
							val += tempimg(x, y - i);
							dissim[x][y][d] = val;
						}
					}
			}
			ImgInt dispmapint(firstimg.Width(), firstimg.Height());
			Set(&dispmapint, 0);
			int min = 10000;
			for (int y = 0;y < firstimg.Height();y++)
				for (int x = 0;x < firstimg.Width(); x++)
				{
					min = 10000;
					for (d = 0;d < max_disp;d++)
					{
						if (dissim[x][y][d] < min)
						{
							min = dissim[x][y][d];
							dispmapint(x, y) = d;
						}

					}
				}

			ImgInt newdispmap(firstimg.Width(), firstimg.Height());
			Set(&newdispmap, 0);
		
			
			
			
			
			Figure fig3;
			fig3.Draw(dispmapint);
			fig3.SetTitle("Figure 3 - Disparity Map");
			/////////

			ImgInt checkedintmap(firstimg.Width(), firstimg.Height());
			Set(&checkedintmap, 0);

				for (int y = 0;y < firstimg.Height();y++)
					for (int x = 0;x < firstimg.Width(); x++)
					{
						min = 10000;
						for (d = 0;d < max_disp;d++)
						{							
							if ((x - dispmapint(x, y) + d) > 0 && (x - dispmapint(x, y) + d) < firstimg.Width())
							{
								if (dissim[x - dispmapint(x, y) + d][y][d] < min)
								{
									min = dissim[x - dispmapint(x, y) + d][y][d];
									newdispmap(x, y) = d;
								}
							}
							else
								newdispmap(x, y) = dispmapint(x, y);
						}
						if (newdispmap(x, y) == dispmapint(x, y))
							checkedintmap(x, y) = dispmapint(x, y);
						else
							checkedintmap(x, y) = 0;
					}
			
		/*	for (d = 0;d < max_disp;d++)
				for (int y = 0;y < firstimg.Height();y++)
				for (int x = 0 ;x < firstimg.Width(); x++)
				{					
					{
						if ((x + dispmapint(x, y) - d) > 0 && (x + dispmapint(x, y) - d) < firstimg.Width())
						{
							dispmap2[x + dispmapint(x, y) - d][y][d] = dispmap[x][y][d];
						}
						else
							dispmap2[x][y][d] = dispmap[x][y][d];
					}
				}

			ImgFloat tempimg2(firstimg.Width(), firstimg.Height());
			Set(&tempimg2, 0);
			for (d = 0; d < max_disp;d++)
			{
				for (int y = 0;y < firstimg.Height();y++)
					for (int x = w;x < firstimg.Width() - w; x++)
					{
						float val = 0;
						for (int i = -w;i < w + 1;i++)
						{
							val += dispmap2[x - i][y][d];
							tempimg2(x, y) = val;
						}
					}
				for (int y = w;y < firstimg.Height() - w;y++)
					for (int x = 0;x < firstimg.Width(); x++)
					{
						float val = 0;
						for (int i = -w;i < w + 1;i++)
						{
							val += tempimg2(x, y - i);
							dissim2[x][y][d] = val;
						}
					}
			}


			for (int y = 0;y < firstimg.Height();y++)
				for (int x = 0;x < firstimg.Width(); x++)
				{
					min = 10000;
					for (d = 0;d < max_disp;d++)
					{
						if (dissim2[x][y][d] < min)
						{
							min = dissim2[x][y][d];
							newdispmap(x, y) = d;
						}

					}
				}
			
			ImgInt checkedintmap(firstimg.Width(), firstimg.Height());
			Set(&checkedintmap, 0);

			checkedintmap = dispmapint;
			for (int y = 0;y < firstimg.Height();y++)
				for (int x = 0;x < firstimg.Width(); x++)
				{
					if (dispmapint(x, y) != newdispmap(x, y))
					{
						checkedintmap(x,y) = 0;
					}
				}*/
				


			Figure fig4;
			fig4.Draw(checkedintmap);
			fig4.SetTitle("Figure 4 - New Disparity Map");

			ImgBgr colorimg(firstimg.Width(), firstimg.Height());
			Load(b, &colorimg);
			int dpt, z;
			int k =225 ;                               
			int counter = 0;
			int count1 = 0;
			for (int y = 0; y < firstimg.Height(); y++)
			{
				for (int x = 0; x < firstimg.Width(); x++)
				{
					if (checkedintmap(x, y) == 0)
					{
						count1++;
					}
				}
			}
			counter = firstimg.Height()*firstimg.Width() - count1;
			ofstream fpt("final.ply");                           
			if (fpt.is_open())                                   
			{
				fpt << "ply\n";
				fpt << "format ascii 1.0\n";
				fpt << "element vertex " << counter << "\n";
				fpt << "property float x\n";                           
				fpt << "property float y\n";
				fpt << "property float z\n";
				fpt << "property uchar diffuse_red\n";
				fpt << "property uchar diffuse_green\n";
				fpt << "property uchar diffuse_blue\n";
				fpt << "end_header\n";
				for (int y = 0; y < firstimg.Height(); y++)
				{
					for (int x = 0; x < firstimg.Width(); x++)
					{
						if (checkedintmap(x, y) != 0)                           
						{
							dpt = k / checkedintmap(x, y);
							z = -1 * dpt;
							fpt << x << " " << -y << " " << z << " " << (int)colorimg(x, y).r << " " << (int)colorimg(x, y).g << " " << (int)colorimg(x, y).b << "\n";
						}
					}
				}
			}
			fpt .close();

			
		}
		else 
			printf(" Input 3 commandline parameters only");
	}

	catch (const Exception& e)
	{
		e.Display();
	}

	EventLoop();
	return 0;
}





