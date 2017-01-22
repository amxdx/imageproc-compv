
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)

{
	MSG			msg;
	HWND		hWnd;
	WNDCLASS	wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, "ID_PLUS_ICON");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = "ID_MAIN_MENU";
	wc.lpszClassName = "PLUS";

	if (!RegisterClass(&wc))
		return(FALSE);

	hWnd = CreateWindow("PLUS", "plus program",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, 0, 1200, 700, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return(FALSE);

	ShowScrollBar(hWnd, SB_BOTH, FALSE);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MainWnd = hWnd;

	ShowPixelCoords = 0;
	intcon = 0;
	strcpy(filename, "");
	OriginalImage = NULL;
	ROWS = COLS = 0;

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(msg.wParam);
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)

{
	HMENU				hMenu;
	OPENFILENAME		ofn;
	FILE				*fpt;
	HDC					hDC;
	char				header[320], text[320], text1[320];
	int					BYTES, xPos, yPos;
	int					xarr[5000], yarr[5000];
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SHOWPIXELCOORDS:
			ShowPixelCoords = (ShowPixelCoords + 1) % 2;
			PaintImage();
			break;
		case ID_DISPLAY_INTIALCONTOUR:
			intcon = (intcon + 1) % 2;
			//  contfn();
			break;
		case ID_FILE_LOAD:
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage = NULL;
			}
			memset(&(ofn), 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			filename[0] = 0;
			ofn.nMaxFile = MAX_FILENAME_CHARS;
			ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
			if (!(GetOpenFileName(&ofn)) || filename[0] == '\0')
				break;		/* user cancelled load */
			if ((fpt = fopen(filename, "rb")) == NULL)
			{
				MessageBox(NULL, "Unable to open file", filename, MB_OK | MB_APPLMODAL);
				break;
			}
			fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
			if (strcmp(header, "P5") != 0 || BYTES != 255)
			{
				MessageBox(NULL, "Not a PPM (P5 greyscale) image", filename, MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}
			OriginalImage = (unsigned char *)calloc(ROWS*COLS, 1);
			header[0] = fgetc(fpt);	/* whitespace character after header */
			fread(OriginalImage, 1, ROWS*COLS, fpt);
			fclose(fpt);
			SetWindowText(hWnd, filename);
			PaintImage();
			break;

		case ID_FILE_QUIT:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_SIZE:		  /* could be used to detect when window size changes */
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_PAINT:
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_LBUTTONDOWN:
		/* if (intcon == 1)
		{
		flag=1;
		goto A;
		}*/
		flag = 1;
		goto A;
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_RBUTTONDOWN:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		trial(xPos, yPos, 10);
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_LBUTTONUP:case WM_RBUTTONUP:
		// intcon = 0;
		flag = 0;
		/*  sprintf(text1, "Count : %d  ",count);
		hDC = GetDC(MainWnd);
		TextOut(hDC, 0, 100, text1, strlen(text1));
		ReleaseDC(MainWnd, hDC);*/
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	A:case WM_MOUSEMOVE:
	{
		if (ShowPixelCoords == 1)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			if (xPos >= 0 && xPos < COLS  &&  yPos >= 0 && yPos < ROWS)
			{
				sprintf(text, "%d,%d=>%d     ", xPos, yPos, OriginalImage[yPos*COLS + xPos]);
				hDC = GetDC(MainWnd);
				TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
				SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the cursor position red */
				ReleaseDC(MainWnd, hDC);
			}
		}
	}
	{
		if (flag == 1)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			if (xPos >= 0 && xPos < COLS  &&  yPos >= 0 && yPos < ROWS)
			{


				rowar[count] = xPos;
				colar[count] = yPos;
				count++;
				sprintf(text, "%d,%d=>%d Count %d ", xPos, yPos, OriginalImage[yPos*COLS + xPos], count);
				hDC = GetDC(MainWnd);

				TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
				SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the cursor position red */

				ReleaseDC(MainWnd, hDC);


			}
		}
		//PaintImage();
		// contfn();
	}
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	break;
	case WM_KEYDOWN:
		if (wParam == 's' || wParam == 'S')
			PostMessage(MainWnd, WM_COMMAND, ID_SHOWPIXELCOORDS, 0);	  /* send message to self */
		if ((TCHAR)wParam == '1')
		{
			TimerRow = TimerCol = 0;
			SetTimer(MainWnd, TIMER_SECOND, 10, NULL);	/* start up 10 ms timer */
		}
		if ((TCHAR)wParam == '2')
		{
			KillTimer(MainWnd, TIMER_SECOND);			/* halt timer, stopping generation of WM_TIME events */
			PaintImage();								/* redraw original image, erasing animation */
		}
		if ((TCHAR)wParam == '3')
		{
			ThreadRunning1 = 1;
			contfn();
		}
		if ((TCHAR)wParam == '4')
		{
			ThreadRunning1 = 0;							/* this is used to stop the child thread (see its code below) */
		}
		if ((TCHAR)wParam == '5')
		{
			//printf("\nE internal 2 : %f\t  E int 1 %f\n",eint2[j],sqd[j]);
			sobel(newcolar, newrowar, newcount);

		}
		if ((TCHAR)wParam == '6')
		{
			//trial(10);
		}
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
		hDC = GetDC(MainWnd);
		SetPixel(hDC, TimerCol, TimerRow, RGB(0, 0, 255));	/* color the animation pixel blue */
		ReleaseDC(MainWnd, hDC);
		TimerRow++;
		TimerCol += 2;
		break;
	case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	}

	hMenu = GetMenu(MainWnd);
	if (ShowPixelCoords == 1)
		CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	else
		CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_UNCHECKED);
	if (intcon == 1)
		CheckMenuItem(hMenu, ID_DISPLAY_INTIALCONTOUR, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	else
		CheckMenuItem(hMenu, ID_DISPLAY_INTIALCONTOUR, MF_UNCHECKED);


	DrawMenuBar(hWnd);

	return(0L);
}
void sobel(int rowar[], int colar[], int count)
{

	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;
	int					 i, r, c, r2, c2;
	float				*tx, *ty, *gradient;
	sx[0] = 1;
	sx[1] = 2;
	sx[2] = 1;
	sx[6] = -1;
	sx[7] = -2;
	sx[8] = -1;
	sy[2] = 1;
	sy[5] = 2;
	sy[8] = 1;
	sy[0] = -1;
	sy[3] = -2;
	sy[6] = -1;
	tx = (float *)calloc(ROWS*COLS, sizeof(float));
	ty = (float *)calloc(ROWS*COLS, sizeof(float));
	float sum = 0;
	if (OriginalImage == NULL)
		return;		/* no image to draw */

	for (int j = 0; j<count; j++)
	{
		//int q = rowar[j + 1] * COLS + colar[j + 1];

		if (j == count - 1)
		{
			int l = rowar[0];
			int m = colar[0];
			int g = rowar[j];
			int h = colar[j];
			dist[j] = sqrt(((l - g)*(l - g)) + ((m - h)*(m - h)));
			//printf("%d %d %d %d",l,m,g,h);
		}
		else
		{
			int x1 = rowar[j];
			int x2 = rowar[j + 1];
			int y1 = colar[j];
			int y2 = colar[j + 1];
			dist[j] = sqrt(((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)));

		}
	}
	temp = 0;
	for (int j = 0; j<count; j++)
	{
		temp = temp + dist[j];
	}
	avgd = temp / count;
	int min1 = 1000;
	int max1 = 0;
	int min2 = 1000;
	int max2 = 0;
	for (int j = 0; j < count; j++)
	{
		eint2[j] = ((avgd - dist[j])*(avgd - dist[j]));
		eint1[j] = dist[j] * dist[j];
	}

	for (int r = 1; r < ROWS - 1; r++)
	{
		for (int c = 1; c < COLS - 1; c++)
		{
			sum = 0;
			for (int r2 = -1; r2 <= 1; r2++)
			{
				for (int c2 = -1; c2 <= 1; c2++)
				{
					sum += OriginalImage[(r + r2)*COLS + (c + c2)] * sx[(r2 + 1) * 3 + (c2 + 1)];
					tx[r*COLS + c] = sum;
				}
			}
		}
	}

	for (int r = 1; r < ROWS - 1; r++)
	{
		for (int c = 1; c < COLS - 1; c++)
		{
			sum = 0;
			for (int r2 = -1; r2 < 2; r2++)
			{
				for (int c2 = -1; c2 < 2; c2++)
				{
					sum += OriginalImage[(r + r2)*COLS + (c + c2)] * sy[(r2 + 1) * 3 + (c2 + 1)];		//Sobel Filter
					ty[r*COLS + c] = sum;
				}
			}
		}
	}
	gradient = (float *)calloc(ROWS*COLS, sizeof(float));

	for (int r = 1; r < ROWS - 1; r++)
	{
		for (int c = 1; c < COLS - 1; c++)
		{
			gradient[r*COLS + c] = sqrt((tx[r*COLS + c] * tx[r*COLS + c]) + (ty[r*COLS + c] * ty[r*COLS + c]));
		}
	}
	int max, min;
	max = 0;
	min = 50000;

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c<COLS; c++)
		{
			if (gradient[r*COLS + c]>max)
			{
				max = gradient[r*COLS + c];
			}
			if (gradient[r*COLS + c] < min)
			{
				min = gradient[r*COLS + c];
			}
		}
	}

	int range = 0;
	range = max - min;
	//printf("%d",range);
	finalg = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			gradient[r*COLS + c] = gradient[r*COLS + c] - min;			//Normalizing sobel
			gradient[r*COLS + c] = gradient[r*COLS + c] * 255;
			gradient[r*COLS + c] = gradient[r*COLS + c] / range;
			finalg[r*COLS + c] = gradient[r*COLS + c];
		}
	}

	int val[1000];
	int nxcoord[1000];
	int nycoord[1000];
	float dist1[1000];
	float entt2[1000];
	float entt1[1000];

	int temp;
	float range1, range2;

	float ene[100];
	float me;

	float a = 3;
	float b = 6;
	float cw = 5;
	for (int z = 0; z<50; z++)
	{
		for (int j = 0; j<count; j++)
		{
			max1 = 0;
			min1 = 40000;
			max2 = 0;
			min2 = 40000;
			temp = 0;
			for (int r1 = -3; r1<4; r1++)
				for (int c1 = -3; c1<4; c1++)
				{
					if (j == count - 1)
					{
						int l = rowar[0];
						int m = colar[0];
						int g = rowar[j] + r1;
						int h = colar[j] + c1;
						dist1[temp] = sqrt(((l - g)*(l - g)) + ((m - h)*(m - h)));
					}
					else
					{
						int x1 = r1 + rowar[j];
						int x2 = rowar[(j)+1];
						int y1 = c1 + colar[j];
						int y2 = colar[(j)+1];
						dist1[temp] = sqrt(((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)));
					}

					entt1[temp] = dist1[temp] * dist1[temp];
					entt2[temp] = (avgd - dist1[temp])*(avgd - dist1[temp]);
					temp++;

				}

			temp = 0;
			for (int r1 = -3; r1<4; r1++)
				for (int c1 = -3; c1<4; c1++)
				{
					if (entt1[temp]>max1)
						max1 = entt1[temp];
					if (entt1[temp]<min1)
						min1 = entt1[temp];

					if (entt2[temp]>max2)
						max2 = entt2[temp];
					if (entt2[temp]<min2)
						min2 = entt2[temp];

					temp++;
				}

			range1 = max1 - min1;
			range2 = max2 - min2;
			//printf("\nmax1 :%d\tmin1:%d",max1,min1);
			//printf("\nmax2 :%d\tmin2:%d",max2,min2);
			//printf("range1: %d range2:%d",range1,range2);
			temp = 0;
			for (int r1 = -3; r1<4; r1++)
				for (int c1 = -3; c1<4; c1++)
				{
					entt1[temp] = entt1[temp] - min1;
					entt1[temp] = entt1[temp] / range1;
					entt2[temp] = entt2[temp] - min2;
					entt2[temp] = entt2[temp] / range2;
					temp++;
				}
			temp = 0;
			for (int r1 = -3; r1<4; r1++)
				for (int c1 = -3; c1<4; c1++)
				{
					ene[temp] = a*entt1[temp] + b*entt2[temp] + cw*finalg[(rowar[temp] + r1)*COLS + (colar[temp] + c1)];
					//printf("%f \t",ene[temp]);        
					temp++;
				}
			temp = 0;
			ene[50] = a + b + cw;
			me = a + b + cw;
			for (int r1 = -3; r1<4; r1++)
				for (int c1 = -3; c1<4; c1++)
				{
					if (ene[temp]<me)
					{
						me = ene[temp];
						//printf("%f \t",me);
					}
					//printf("%f \t",me);                
					temp++;
				}
			temp = 0;
			for (int r1 = -3; r1<4; r1++)
				for (int c1 = -3; c1<4; c1++)
				{
					if (ene[temp] == me)
					{
						colar[j] = colar[j] + c1;
						rowar[j] = rowar[j] + r1;

						//	printf("%d \t %d %d \t %d %d \n", temp, colar[j], rowar[j], colar[j] + c1, rowar[j] + r1);
					}
					temp++;
				}
		}
		temp = 0;
	}
	PaintImage();
	hDC = GetDC(MainWnd);
	for (int k = 0; k < newcount; k++)
	{
		SetPixel(hDC, colar[k], rowar[k], RGB(255, 255, 255));
	}
	ReleaseDC(MainWnd, hDC);
}

int trial(int x, int y, int radius)
{
	HDC hDC;
	hDC = GetDC(MainWnd);
	int r, c;
	Ellipse(hDC, x + 50, y + 50, x - 50, y - 50);
	/*for (r = 0; r < 50; r++)
	{
	for (c = 0; c < 50; c++)
	{
	SetPixel(hDC, r+2*radius, c+radius, RGB(0, 255, 0));
	}
	}*/
	ReleaseDC(MainWnd, hDC);
}
void contfn()
{
	HDC		hDC;
	hDC = GetDC(MainWnd);
	for (int k = 0; k < count / 5; k++)
	{
		newrowar[k] = rowar[k * 5];
		newcolar[k] = colar[k * 5];
		newcount++;
		SetPixel(hDC, newrowar[k], newcolar[k], RGB(255, 255, 255));
	}
	ReleaseDC(MainWnd, hDC);
}


void PaintImage()

{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;
	int					i, r, c, DISPLAY_ROWS, DISPLAY_COLS;
	unsigned char		*DisplayImage;

	if (OriginalImage == NULL)
		return;		/* no image to draw */

					/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	DisplayImage = (unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS, 1);
	for (r = 0; r<ROWS; r++)
		for (c = 0; c<COLS; c++)
			DisplayImage[r*DISPLAY_COLS + c] = OriginalImage[r*COLS + c];

	BeginPaint(MainWnd, &Painter);
	hDC = GetDC(MainWnd);
	bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bm_info_header.biWidth = DISPLAY_COLS;
	bm_info_header.biHeight = -DISPLAY_ROWS;
	bm_info_header.biPlanes = 1;
	bm_info_header.biBitCount = 8;
	bm_info_header.biCompression = BI_RGB;
	bm_info_header.biSizeImage = 0;
	bm_info_header.biXPelsPerMeter = 0;
	bm_info_header.biYPelsPerMeter = 0;
	bm_info_header.biClrUsed = 256;
	bm_info_header.biClrImportant = 256;
	bm_info = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bm_info->bmiHeader = bm_info_header;
	for (i = 0; i<256; i++)
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}

	SetDIBitsToDevice(hDC, 0, 0, DISPLAY_COLS, DISPLAY_ROWS, 0, 0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		DisplayImage, bm_info, DIB_RGB_COLORS);
	ReleaseDC(MainWnd, hDC);
	EndPaint(MainWnd, &Painter);

	free(DisplayImage);
	free(bm_info);
}




void AnimationThread(HWND AnimationWindowHandle)

{
	HDC		hDC;
	char	text[300];

	ThreadRow = ThreadCol = 0;
	while (ThreadRunning == 1)
	{
		hDC = GetDC(MainWnd);
		//SetPixel(hDC,ThreadCol,ThreadRow,RGB(0,255,0));	/* color the animation pixel green */

		for (int k = 0; k < count / 5; k++)
		{
			newrowar[k] = rowar[k * 5];
			newcolar[k] = colar[k * 5];
			newcount++;
		}
		PaintImage();
		for (int k = 0; k < count / 5; k++)
		{
			SetPixel(hDC, newrowar[k], newcolar[k], RGB(0, 255, 255));
			sprintf(text, "%d,%d     ", newrowar[k], newcolar[k]);
			TextOut(hDC, 300, 0, text, strlen(text));		/* draw text on the window */
		}
		ReleaseDC(MainWnd, hDC);
		//ThreadRow+=3;
		//ThreadCol++;
		Sleep(100);		/* pause 100 ms */
	}
}

