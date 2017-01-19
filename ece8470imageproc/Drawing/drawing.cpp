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
		if (argc != 4)
			printf("Insufficient input arguments");
		else {



			CString filename1 = argv[1];
			CString filename2 = argv[2];
			CString filename3 = argv[3];

			CString a = "../../images/";
			CString b = a + filename1;
			CString c = a + filename2;
			CString d = a + filename3;
			// Load image.
			ImgBgr firstimg;
			Load(b, &firstimg);

			// Display image in figure window.
			Figure fig1;
			fig1.Draw(firstimg);
			// creating a synthetic grayscale image
			ImgGray secondimg;
			secondimg.Reset(firstimg.Width(), firstimg.Height());
			if (firstimg.Width() > 100 && firstimg.Height() > 100)

				for (int x = 0; x < firstimg.Width(); x++)
					for (int y = 0;y < firstimg.Height();y++)
						if (x>((firstimg.Width() / 2) - 50) && x<((firstimg.Width() / 2) + 50) && y>((firstimg.Height() / 2) - 50) && y < ((firstimg.Height() / 2) + 50))
							secondimg(x, y) = 255;
						else
							secondimg(x, y) = 0;

			else if ((firstimg.Width() < 100 && firstimg.Height() > 100))
				for (int x = 0; x < firstimg.Width(); x++)
					for (int y = 0;y < firstimg.Height();y++)
						if (y>((firstimg.Height() / 2) - 50) && y < ((firstimg.Height() / 2) + 50))
							secondimg(x, y) = 255;
						else
							secondimg(x, y) = 0;
			else if ((firstimg.Width() > 100 && firstimg.Height() < 100))
				for (int x = 0; x < firstimg.Width(); x++)
					for (int y = 0;y < firstimg.Height();y++)
						if (x>((firstimg.Width() / 2) - 50) && x < ((firstimg.Width() / 2) + 50))
							secondimg(x, y) = 255;
						else
							secondimg(x, y) = 0;
			else
				for (int x = 0; x < firstimg.Width(); x++)
					for (int y = 0;y < firstimg.Height();y++)
						secondimg(x, y) = 255;



			Figure fig2;
			fig2.Draw(secondimg);

			Save(secondimg, c);

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



		}
		// Loop forever until user presses Ctrl-C in terminal window.
		EventLoop();
		
		return 0;


	}




