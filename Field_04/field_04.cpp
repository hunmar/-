
//Z-буфер,    ортографическое проецирование

//строим картину линий электромагнитного поля
//рисование линий, стрелок и магнитной пластинки
//вначале происходит в Z-буфере, заполняя Z-буфер
//затем содержимое Z-буфера перерисовывается по пиксельно
//на битовую карту контекства памяти
//затем рисуются в памяти координатные оси и куб
//после этого содержимое битовой карты копируется на экран
 


#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>




//структура углов поворота системы координат
struct ANGLS {
	double fi, teta;
};

//структура 3D-точки
struct POINT3 {
	double x, y, z;
};


//прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void LineCreate();
void LineDestroy();
void LinePicture(HWND,int);
//void LinePaint1(HWND);
void LineLButtonDown(HWND,int,int);
void LineMouseMove(HWND,int,int);
void DrawBox(HWND, HDC, ANGLS);
void LineLButtonUp(HWND);
void PointCorns();
void arrowVector(HDC hdc, int x1, int y1, int x2, int y2,double ze, COLORREF rgb);
void LineField(HDC hdc,POINT3 PointB,COLORREF);
double ZPalet(double z);
void ZbufLineWidth(HDC hdc, int x1, int y1, int x2, int y2,
				   double ze1, double ze2, int w, COLORREF rgb);
void ZbufParallelogram(HDC,int,int,double,int,int,double,
					   int,int,double,int,int,double,COLORREF);
		


//главная функция
int WINAPI WinMain(HINSTANCE hInstance,      
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdParam, 
				   int       nCmdShow)     
{

	WCHAR cname[] = L"Class";                 // Имя класса окна 
	WCHAR title[] = L"Myalin M.        "     // Заголовок окна
			L"   Vector-Field,       "
			L"   rotation  by  mouse  and  keyboard (arrows)";

	
	
	//регистрация класса окна приложения
	//------------------------------------------------------------------------
	WNDCLASS wc;  // структура для регистрации класса окна приложения

	wc.style          = 0; 
	wc.lpfnWndProc    = (WNDPROC)WndProc;         //адрес функции окна
	wc.cbClsExtra     = 0;       
	wc.cbWndExtra     = 0;       
	wc.hInstance      = hInstance;       //идентификатор приложения
	wc.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION); 
	wc.hCursor		  = LoadCursor(NULL, IDC_ARROW);  
	wc.hbrBackground  = (HBRUSH)GetStockObject(GRAY_BRUSH);   
	wc.lpszMenuName   = 0;       
	wc.lpszClassName  = cname;      

	if(!RegisterClass(&wc))  //регистрация класса окна приложения
		return 0;


	//создание окна приложения
	//------------------------------------------------------------------------
	 HWND hWnd;       // идентификатор окна приложения

	 hWnd = CreateWindow(cname,
						 title,
						 WS_OVERLAPPEDWINDOW,
						 100, 
						 30, 
						 700, 
						 700, 
						 NULL,         //идентификатор окна-родителя     
						 NULL,          //идентификатор меню
						 hInstance,     //идентификатор приложения
						 NULL);         

	if(!hWnd)
		return 0;



	// Рисуем окно. Для этого после функции ShowWindow,
    // рисующей  окно, вызываем функцию UpdateWindows,
    // посылающую сообщение WM_PAINT в функцию окна
 	//--------------------------------------------------------------------------
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd); 

	MSG msg;    // структура для работы с сообщениями

	// Запускаем цикл обработки сообщений
	//-------------------------------------------------------------------------
  
	while(GetMessage(&msg,NULL,0,0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;


}

/*

//главная функция
int PASCAL WinMain(HINSTANCE hInst,
						 HINSTANCE hPrevInstance,
						 LPSTR     lpszCmdParam,
						 int       nCmdShow)
{
	MSG msg;

	if(!RegisterApp(hInst))  //регистрация приложения
		return FALSE;

	if(!CreateApp(hInst,nCmdShow))  //создание окна приложения
		return FALSE;

	while(GetMessage(&msg,NULL,0,0))   //цикл обработки сообщений
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

//регистрация класса окна 
BOOL RegisterApp(HINSTANCE hInst)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		  = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(LTGRAY_BRUSH);
	wc.lpszMenuName  = "APP_MENU";
	wc.lpszClassName = szClassName;

	return RegisterClass(&wc);
}



//создание окна
HWND CreateApp(HINSTANCE hInstance, int nCmdShow)
{
	 HWND hwnd;

	 hwnd = CreateWindow(szClassName,
									 szWindowTitle,
									 WS_OVERLAPPEDWINDOW,
									 CW_USEDEFAULT, 
									 CW_USEDEFAULT, 
									 CW_USEDEFAULT, 
									 CW_USEDEFAULT, 
									 NULL,
									 NULL,
									 hInstance,
									 NULL);

	if(hwnd == NULL)
		return hwnd;

	ShowWindow(hwnd,nCmdShow);
	UpdateWindow(hwnd);

	return hwnd;
}

*/

//углы поворота системы кординат
ANGLS angl,anglOld;


//функция обработки сообщений (функция окна) 
LRESULT CALLBACK  WndProc(HWND hwnd, UINT msg,
											WPARAM wParam, LPARAM lParam)
{

	int x,y;
	switch(msg)
	{
		case WM_CREATE:
			LineCreate();
			break;

		case WM_PAINT:
//			InvalidateRect(hwnd,NULL,TRUE);
			LinePicture(hwnd,1);
			break;

		case WM_SIZE:
			InvalidateRect(hwnd,NULL,TRUE);
			break;

		case WM_LBUTTONDOWN:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			LineLButtonDown(hwnd,x,y);
			break;

		case WM_LBUTTONUP:
			LineLButtonUp(hwnd);
			LinePicture(hwnd,2);
			break;


		case WM_MOUSEMOVE:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			LineMouseMove( hwnd,x,y);
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_LEFT:
					angl.fi += 10;
					LinePicture(hwnd,2);
					break;

				case VK_RIGHT:
					angl.fi -= 10;
					LinePicture(hwnd,2);
					break;

				case VK_UP:
					angl.teta += 10;
					LinePicture(hwnd,2);
					break;

				case VK_DOWN:
					angl.teta -= 10;
					LinePicture(hwnd,2);
					break;
			}
			break;

		case WM_DESTROY:
			LineDestroy();
			break;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0L;
}


double const M_PI = 3.141592654;

//структура операций с мышкой
struct TDATA {
	BOOL ButtonDown;
	BOOL Drawing;
};

TDATA Dat;


//структура точки в пиксельной плоскости
struct CORD {
	int x, y;
};

CORD cor,corOld;

//вершины куба, связанного с подвижной системой координат
POINT3 Point[8];
//начальные точки линий поля
POINT3 PointB[1000];

//добро пожаловать в ад

//первый заряд
double qForce1 = 3;
POINT3 q1;

//второй заряд
double qForce2 = -1;
POINT3 q2;

//структура 3D-вектора
struct VECTORS {
	double x, y, z;
	double dx, dy, dz;
};


//структура вектора магнитного поля
struct VECMAG {
	double hx, hy, hz;
};


//структура для трех цветов модели RGB
struct ColorS{
	unsigned char R, G, B;
};

//структура Z-буффер
struct ZbuffS{
	double z; //глубина буфера для данного пиксела
    ColorS c; //три цвета RGB для данного пиксела
};

ZbuffS *zb;


//число пикселей в окне вывода
unsigned long Np, Mp, NM;


//вершины магнитной пластинки
double Px[4], Py[4], Pz[4];

//размеры поля вывода в мировых координатах и в пикселях
double xe1, xe2, ye1, ye2;
int    ne1, ne2, me1, me2;

//длины координатных осей (в мировых координатах) (размер куба)
double xmax, ymax, zmax;

//размеры магнитной пластинки
double ax, ay, az;


//задаем различные параметры в начале работы приложения
void LineCreate()
{

//размеры поля вывода в мировых координатах
	xe1 = -10;  xe2 =   10; ye1 =  -10; ye2 =  10;

//длины координатных осей (в мировых координатах)
	xmax=4.8, ymax=4.8, zmax=4.8;

//задаем вершины куба, связанного с подвижной системой координат
	PointCorns();


//размеры магнитной пластинки
	ax = xmax*0.95, ay = ymax*0.95, az = zmax*0.95;


//задаем вершины магнитной пластинки
	Px[0] =   ax;  Py[0] =   ay;  Pz[0] =  az;
	Px[1] =  -ax;  Py[1] =   ay;  Pz[1] =  -az;
	Px[2] =  -ax;  Py[2] =  -ay;  Pz[2] =  az;
	Px[3] =   ax;  Py[3] =  -ay;  Pz[3] =  -az;

	q1.x = 0.1;
	q1.y = 1;
	q1.z = 1.5;

	q2.x = 0.0;
	q2.y = 0.0;
	q2.z = -1;
	

	int i;
	double Radius = 0.2;
	for (i = 0; i < 20; i++)
	{
		double randomR = 2*Radius* (double)rand() / RAND_MAX - Radius;
		double randomPhi = 2 * M_PI* (double)rand() / RAND_MAX;
		double theta = asin(randomR / Radius);
		PointB[i].x = Radius*cos(theta)*cos(randomPhi) + q1.x;
		PointB[i].y = Radius*cos(theta)*sin(randomPhi) + q1.y;
		PointB[i].z = randomR + q1.z;
	}

	for (i = 20; i < 40; i++)
	{
		double randomR = 2 * Radius* (double)rand() / RAND_MAX - Radius;
		double randomPhi = 2 * M_PI* (double)rand() / RAND_MAX;
		double theta = asin(randomR / Radius);
		PointB[i].x = Radius*cos(theta)*cos(randomPhi) + q2.x;
		PointB[i].y = Radius*cos(theta)*sin(randomPhi) + q2.y;
		PointB[i].z = randomR + q2.z;
	}

//начальные значения углов поворота системы координат
	angl.fi = 30; angl.teta = 60;
}


//задаем вершины куба, связанного с подвижной системой координат
void	PointCorns()
{
	Point[0].x =  xmax; Point[0].y =  ymax; Point[0].z = -zmax;
	Point[1].x = -xmax; Point[1].y =  ymax; Point[1].z = -zmax;
	Point[2].x = -xmax; Point[2].y = -ymax; Point[2].z = -zmax;
	Point[3].x =  xmax; Point[3].y = -ymax; Point[3].z = -zmax;
	Point[4].x =  xmax; Point[4].y =  ymax; Point[4].z = zmax;
	Point[5].x = -xmax; Point[5].y =  ymax; Point[5].z = zmax;
	Point[6].x = -xmax; Point[6].y = -ymax; Point[6].z = zmax;
	Point[7].x =  xmax; Point[7].y = -ymax; Point[7].z = zmax;
}


//вычисляем магнитное поле (hx,hy,hz) в заданной точке пространства (x,y,z)
VECMAG magn(double x, double y, double z)
{
	VECMAG mag;

	double deltaX = x - q1.x;
	double deltaY = y - q1.y;
	double deltaZ = z - q1.z;

	double znamenatelPartOne = deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ;
	double znamentelPartTwo = znamenatelPartOne*znamenatelPartOne*znamenatelPartOne;
	double znamenatel = sqrt(znamentelPartTwo);

	mag.hx =
			 (deltaX/znamenatel)*qForce1;
	mag.hy =
		(deltaY / znamenatel)*qForce1;
	mag.hz =
		(deltaZ / znamenatel)*qForce1;

	///
	deltaX = x - q2.x;
	deltaY = y - q2.y;
	deltaZ = z - q2.z;

	znamenatelPartOne = deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ;
	znamentelPartTwo = znamenatelPartOne*znamenatelPartOne*znamenatelPartOne;
	znamenatel = sqrt(znamentelPartTwo);

	mag.hx +=
		(deltaX / znamenatel)*qForce2;
	mag.hy +=
		(deltaY / znamenatel)*qForce2;
	mag.hz +=
		(deltaZ / znamenatel)*qForce2;

	return mag;
}


//закрываем окно приложения
void LineDestroy()
{
	PostQuitMessage(0);   
}


//угловые коэффициенты поворота системы координат
double sf,cf,st,ct;

double dCentral = 4;
double cCentral = 8;

//переход из мировой системы координат в видовую
//отрографическое проектирование
inline double Xe(double x, double y, double z)
{
	double u = (dCentral + cCentral - (st*cf*x + st*sf*y + ct*z)) / cCentral;
	return (-sf*x + cf*y) / u;
}

//переход из мировой системы координат в видовую
// отрографическое проектирование
inline double Ye(double x,double y,double z)
{
	double u = (dCentral + cCentral - (st*cf*x + st*sf*y + ct*z)) / cCentral;
	return (-ct*cf*x - ct*sf*y + st*z) / u;
}

//переход из мировой системы координат в видовую
//отрографическое проектирование
inline double Ze(double x,double y,double z)
{
  return st*cf*x+st*sf*y+ct*z -dCentral;
}




//переход от видовых координат к экранным координатам 
//переход от координты x к пикселю n
inline int xn(double x)
{
	return (int)((x - xe1)/(xe2 - xe1)*(ne2 - ne1)) + ne1;
}

//переход от координты y к пикселю m
inline int ym(double y)
{
	return (int)((y - ye1)/(ye2 - ye1)*(me2 - me1)) + me1;
}


//"рисуем" 2D вектор-стрелку
//с помощью функций работающих с Z-буфером
void arrowVector(HDC hdc, int x1, int y1, int x2, int y2,double ze, COLORREF rgb)
{

	int k = 5; //размеры стрелки в пикселях
	double d = sqrt((double)((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
	double ax = (x1-x2)/d, ay = (y1-y2)/d; 
	int x3, y3, x4, y4, x0, y0;
	
	x0 = int(k*ax) + x2;
	y0 = int(k*ay) + y2;
	x3 = -int(k*ay) + x0;
	y3 = int(k*ax) + y0;
	x4 = int(k*ay) + x0;
	y4 = -int(k*ax) + y0;

	
	ZbufLineWidth(hdc,x3,y3,x2,y2,ze,ze,2,rgb);	
	ZbufLineWidth(hdc,x2,y2,x4,y4,ze,ze,2,rgb);
}


//"рисует" одну линию поля из начальной точки PointB 
//с помощью функций работающих с Z-буфером
void LineField(HDC hdc,POINT3 PointB,COLORREF rgb, double force)
{

	VECTORS vect;	
	vect.x = PointB.x;
	vect.y = PointB.y;
	vect.z = PointB.z;
	
	//видовые координаты проецируемой точки
	double xe, ye, ze1, ze2;
	//координаты пикселов
	int x1,y1,x2,y2;

	double dt = force > 0 ? 0.1 : -0.1; //длина шага на линии поля
	double x, y, z, Hx, Hy, Hz, Ha;
	int k = 0;

	VECMAG mag;
	int step = 0;
	double xt1,yt1,zt1,xt2,yt2,zt2;
	do
	{
		step++;
		x = vect.x;
		y = vect.y;
		z = vect.z;

		mag = magn(x,y,z);

		Hx = mag.hx;
		Hy = mag.hy;
		Hz = mag.hz;

		Ha = sqrt(Hx*Hx + Hy*Hy + Hz*Hz);
		vect.dx = Hx/Ha;
		vect.dy = Hy/Ha;
		vect.dz = Hz/Ha;

		xt1 = vect.x; yt1 = vect.y; zt1 = vect.z;
		xt2 = xt1 + vect.dx*dt;
		yt2 = yt1 + vect.dy*dt;
		zt2 = zt1 + vect.dz*dt;

		xe = Xe(xt1, yt1, zt1);
		ye=Ye(xt1,yt1,zt1);
		ze1=Ze(xt1,yt1,zt1);
		x1=xn(xe);
		y1=ym(ye);

		xe = Xe(xt2, yt2, zt1);
		ye=Ye(xt2,yt2,zt2);
		ze2=Ze(xt2,yt2,zt2);
		x2=xn(xe);
		y2=ym(ye);

		//"рисуем" отрезок линии поля 
		ZbufLineWidth(hdc,x1,y1,x2,y2,ze1,ze2,3,rgb);

		vect.x = xt2;
		vect.y = yt2;
		vect.z = zt2;

		//после 10-и шагов на лини поля "рисуем" стрелку
		k++;
		if(k == 10)
		{
			arrowVector(hdc,x1,y1,x2,y2,ze2,RGB(0,0,255));
			k = 0;
		}

	//прекращаем рисовать линию поля на границе куба
	} while (step<1000&&(x>-xmax) && (x<xmax) && (y>-ymax) && (y<ymax) && (z>-zmax) && (z<zmax) && ((abs(x - q1.x) > 0.01) || (abs(y - q1.y) > 0.01) || (abs(z - q1.z) > 0.01))
		&& ((abs(x - q2.x) > 0.01) || (abs(y - q2.y) > 0.01) || (abs(z - q2.z) > 0.01)));
}



//рисуем картину приложения
void LinePicture(HWND hwnd, int Context)
{
//выбираем нужный контектс устройства для экрана
//---------------------------------------------
	HDC hdcWin;
	PAINTSTRUCT ps;
	//получаем контест устройства для экрана
	if(Context == 1)
		hdcWin = BeginPaint(hwnd, &ps);
	else
		hdcWin = GetDC(hwnd);
//-----------------------------------------------


//связываем размеры поля вывода с размерами клиентской области окна
//--------------------------------------------------------------------
	RECT rct;
	GetClientRect(hwnd,&rct);

	ne1 = rct.left+50; ne2 = rct.right -50;
	me1 = rct.bottom -50; me2 = rct.top + 50;
//------------------------------------------------------------------

//создаем контекст экрана
//------------------------------------------------------------
	HDC hdc = CreateCompatibleDC(hdcWin); //создаем контекст
                     //памяти связаный с контекстом экрана

   //памяти надо придать вид экрана - подходт битовая карта с форматом
   // как у экрана. В памяти будем рисовать на битовой карте
	HBITMAP hBitmap, hBitmapOld;
	hBitmap = CreateCompatibleBitmap(hdcWin, ne2, me1); //создаем
               //битовую карту совместмую с контекстом экрана
	hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap); //помещаем
                // битовую карту в контекст памяти
//--------------------------------------------------------------
	

//выводи значения углов в верхней части поля вывода
//--------------------------------------------------------	
	//создание прямоугольной области для вывода углов поворота
	HRGN hrgn2 = CreateRectRgn(ne1,me2-30,ne2,me1);

	//заливаем выделенную область серым цветом
	HBRUSH hBrush2 = CreateSolidBrush(RGB(0x80,0x80,0x80));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc,hBrush2); 
    FillRgn(hdc,hrgn2,hBrush2);

	SelectObject(hdc,hBrushOld);   
	DeleteObject(hBrush2);         
	DeleteObject(hrgn2);         

	
	//вычисление угловых коэффициентов поворота системы координат
	sf=sin(M_PI*angl.fi/180);
	cf=cos(M_PI*angl.fi/180);
	st=sin(M_PI*angl.teta/180);
	ct=cos(M_PI*angl.teta/180);


	//информация об углах поворота системы координат
	TCHAR ss[20];
	SetBkColor(hdc,RGB(0xC0,0xC0,0xC0));
	SetTextColor(hdc,RGB(0,0,0x80));
	swprintf_s(ss,20,L"fi = %4.0lf",angl.fi);
	TextOut(hdc,(ne1+ne2)/2-80,me2-25,ss,9);
	swprintf_s(ss,20,L"teta = %4.0lf",angl.teta);
	TextOut(hdc,(ne1+ne2)/2+20,me2-25,ss,11);
//------------------------------------------------

	
	


//выделение памяти под Z-буфер и начальное его заполнение
//-------------------------------------------------------------
	//вычисляем число пикселей в поле  вывода
	Np = ne2-ne1 + 1, Mp = me1-me2 +1, NM = Np*Mp;

	//выделяем память под Z-буфер для каждого пикселя
	zb = new ZbuffS [NM];

	//начальное заполнение z-буфера для каждого пикселя
	for ( long unsigned p=0; p<NM; p++)
	{
   		zb[p].z = -1000;
   		zb[p].c.R = 0xC0; zb[p].c.G = 0xC0; zb[p].c.B = 0xC0;
	}
//-----------------------------------------------------------

	

//"рисуем" магнитную пластинку заданным цветом заполняя Z-буфер	
//-----------------------------------------------------------------------
	//мировые координаты проецируемой точки
	double xt1,yt1,zt1;
	//видовые координаты проецируемой точки
	double xe,ye,ze1;
	//пиксельные координаты проецируемой точки
	int x1,y1;

	//пиксельные координаты 4-х углов пластинки
	int xp[4], yp[4];
	//видовые z-координаты 4-х углов пластинки
	double ze[4];

	for(int n=0; n<4; n++)
	{
		xt1 = Px[n]; yt1 = Py[n]; zt1 = Pz[n];
		xe = Xe(xt1, yt1, zt1);
		ye=Ye(xt1,yt1,zt1);
		ze1=Ze(xt1,yt1,zt1);
		x1=xn(xe);
		y1=ym(ye);
		xp[n] = x1; yp[n] = y1; ze[n] = ze1;
	}	
	//ZbufParallelogram(hdc,xp[0],yp[0],ze[0],xp[1],yp[1],ze[1],
		//xp[2],yp[2],ze[2],xp[3],yp[3],ze[3],RGB(255,255,0));
//------------------------------------------------------------------



//"рисуем" линии поля заполняя Z-буфер
//----------------------------------------------------------------
	for (int i = 0; i < 20; i++)
	{
		LineField(hdc,PointB[i],RGB(255,0,0),1);
		
	}
	
	for(int i=20; i<40; i++)
	{
		LineField(hdc,PointB[i],RGB(0,0,255), -1);
		
	}
	
//-----------------------------------------------------------------------


//выводим содержимое Z-буфера в контекст памяти
//--------------------------------------------------------------------
	//двигаемся по всем пикселям окна вывода
	for (unsigned long ij=0; ij<NM; ij++)
	{
   		x1 = ne1 + ij%Np;
		y1 = me2 + ij/Np;
		SetPixel(hdc,x1,y1,RGB(zb[ij].c.R,zb[ij].c.G,zb[ij].c.B));

	}

	delete [] zb; //очищаем память под Z-буфером
//-------------------------------------------------------------



//рисуем координтные оси
//------------------------------------------------------------------------

	HPEN hPen = CreatePen(PS_SOLID,1,RGB(0,255,255));
	HPEN hPenOld = (HPEN)SelectObject(hdc,hPen);    
	
	int x2,y2;

//ось Ox
	xe=Xe(-xmax/3,0,0);
	ye=Ye(-xmax/3,0,0);
	x1=xn(xe);
	y1=ym(ye);
	xe=Xe(xmax,0,0);
	ye=Ye(xmax,0,0);
	x2=xn(xe);
	y2=ym(ye);
	
	MoveToEx(hdc,x1,y1,NULL);
	LineTo(hdc,x2,y2);

	SetBkColor(hdc,RGB(0xC0,0xC0,0xC0));
	SetTextColor(hdc,RGB(120,120,120));
	TextOut(hdc,x2, y2, _T("X"),1);

//Ось Oy
	xe=Xe(0,-ymax/3, 0);
	ye=Ye(0,-ymax/3,0);
	x1=xn(xe);
	y1=ym(ye);
	xe=Xe(0,ymax, 0);
	ye=Ye(0,ymax,0);
	x2=xn(xe);
	y2=ym(ye);
	
	MoveToEx(hdc,x1,y1,NULL);
	LineTo(hdc,x2,y2);

	SetBkColor(hdc,RGB(0xC0,0xC0,0xC0));
	SetTextColor(hdc,RGB(120,120,120));
	TextOut(hdc,x2, y2, _T("Y"),1);

//Ось Oz
	xe=Xe(0,0, 0);
	ye=Ye(0,0,-zmax/3);
	x1=xn(xe);
	y1=ym(ye);
	xe=Xe(0,0, 0);
	ye=Ye(0,0,zmax);
	x2=xn(xe);
	y2=ym(ye);

	MoveToEx(hdc,x1,y1,NULL);
	LineTo(hdc,x2,y2);

	SetBkColor(hdc,RGB(0xC0,0xC0,0xC0));
	SetTextColor(hdc,RGB(120,120,120));
	TextOut(hdc,x2, y2, _T("Z"),1);


	SelectObject(hdc,hPenOld); 
	DeleteObject(hPen);        
//-----------------------------------------------------------------------------


//рисуем куб
//----------------------------------------------------------------------
	hPen = CreatePen(PS_SOLID,1,RGB(160,160,160));
	hPenOld = (HPEN)SelectObject(hdc,hPen);    

	DrawBox(hwnd, hdc, angl);

	SelectObject(hdc,hPenOld); 
	DeleteObject(hPen);        
//------------------------------------------------------------------------


//копируем контекст памяти в контекст экрана
//-----------------------------------------------------------------------	
	BitBlt(hdcWin,ne1,me2-30,ne2,me1,hdc,ne1,me2-30,SRCCOPY); 
//----------------------------------------------------------------------


//завершаем работу с контекстами и памятью
//-------------------------------------------------------------------
	SelectObject(hdc, hBitmapOld); //востанавливаем контекст памяти
	DeleteObject(hBitmap); //убираем битовую карту
	DeleteDC(hdc);  //  освобождаем контекст памяти

	//освобождаем контекст экрана
	if(Context == 1)
		EndPaint(hwnd, &ps);
	else
		ReleaseDC(hwnd, hdcWin);   
}




//обрабатывает нажатие левой клавиши мышки
void LineLButtonDown(HWND hwnd, int x, int y)
{
	Dat.ButtonDown = TRUE;
	Dat.Drawing = FALSE;
	
	anglOld.fi = angl.fi;
	anglOld.teta = angl.teta;
	corOld.x = x;
	corOld.y = y;
	
//при нажатии клавиши мыши выделяется куб синим цветом
	HDC PaintDC = GetDC(hwnd);
	HPEN hPen, hPenOld;
	hPen = CreatePen(PS_SOLID,1,RGB(0,0,255));
	hPenOld = SelectPen(PaintDC,hPen);   

	SetROP2(PaintDC, R2_NOTXORPEN);
		
	DrawBox(hwnd, PaintDC, anglOld);

	SelectPen(PaintDC,hPenOld);  
	DeletePen(hPen);           

	ReleaseDC(hwnd, PaintDC);
}


//обрабатывает отпускание левой клавиши мышки
void LineLButtonUp(HWND hwnd)
{
	if(Dat.ButtonDown && Dat.Drawing)
		Dat.Drawing = FALSE;

	Dat.ButtonDown = FALSE;
}


//обрабатывает движение мыши при нажатой левой клавиши
void LineMouseMove(HWND hwnd,int x, int y)
{

	if(Dat.ButtonDown)
	{
		Dat.Drawing = TRUE;

		HDC PaintDC = GetDC(hwnd);
		
		//перерисовывает куб в новом положении в режиме NOTXOR
		HPEN hPen, hPenOld;
		hPen = CreatePen(PS_SOLID,1,RGB(0,0,255));
		hPenOld = SelectPen(PaintDC,hPen);  

		SetROP2(PaintDC, R2_NOTXORPEN);
		
		DrawBox(hwnd, PaintDC, anglOld);

		angl.fi += corOld.x-x;
		angl.teta += corOld.y-y;
		
		corOld.x = x; corOld.y = y;

		anglOld.fi   = angl.fi;
		anglOld.teta   = angl.teta;

		DrawBox(hwnd, PaintDC, anglOld);

		SelectPen(PaintDC,hPenOld);   
		DeletePen(hPen);           


		//информация об углах поворота куба
		TCHAR ss[20];
		SetBkColor(PaintDC,RGB(0xC0,0xC0,0xC0));
		SetTextColor(PaintDC,RGB(0,0,0x80));
		swprintf_s(ss,20,L"fi = %4.0lf",angl.fi);
		TextOut(PaintDC,(ne1+ne2)/2-80,me2-25,ss,9);
		swprintf_s(ss,20,L"teta = %4.0lf",angl.teta);
		TextOut(PaintDC,(ne1+ne2)/2+20,me2-25,ss,11);

		ReleaseDC(hwnd, PaintDC);
	}

}


//рисуем куб, связанный с подвижной системой координат
void DrawBox(HWND hwnd, HDC hdc, ANGLS an)
{
	sf=sin(M_PI*an.fi/180);
	cf=cos(M_PI*an.fi/180);
	st=sin(M_PI*an.teta/180);
	ct=cos(M_PI*an.teta/180);

	double xe, ye;
	int x1,y1,x2,y2;
	double xt1,yt1,zt1,xt2,yt2,zt2;
	int j;

	for(int i=0; i<4; i++)
	{
		j = i + 1;
		if(j==4)
			j = 0;
		xt1 = Point[i].x; yt1 = Point[i].y; zt1 = Point[i].z;
		xt2 = Point[j].x; yt2 = Point[j].y; zt2 = Point[j].z;

		xe = Xe(xt1, yt1, zt1);
		ye=Ye(xt1,yt1,zt1);
		x1=xn(xe);
		y1=ym(ye);

		xe = Xe(xt2, yt2, zt2);
		ye=Ye(xt2,yt2,zt2);
		x2=xn(xe);
		y2=ym(ye);

		MoveToEx(hdc,x1,y1,NULL);
		LineTo(hdc,x2,y2);
	}



	for(int i=4; i<8; i++)
	{
		j = i + 1;
		if(j==8)
			j = 4;
		xt1 = Point[i].x; yt1 = Point[i].y; zt1 = Point[i].z;
		xt2 = Point[j].x; yt2 = Point[j].y; zt2 = Point[j].z;

		xe = Xe(xt1, yt1, zt1);
		ye=Ye(xt1,yt1,zt1);
		x1=xn(xe);
		y1=ym(ye);

		xe = Xe(xt2, yt2, zt2);
		ye=Ye(xt2,yt2,zt2);
		x2=xn(xe);
		y2=ym(ye);

		MoveToEx(hdc,x1,y1,NULL);
		LineTo(hdc,x2,y2);
	}

	for(int i=0; i<4; i++)
	{
		xt1 =   Point[i].x; yt1 =   Point[i].y; zt1 =   Point[i].z;
		xt2 = Point[i+4].x; yt2 = Point[i+4].y; zt2 = Point[i+4].z;

		xe = Xe(xt1, yt1, zt1);
		ye=Ye(xt1,yt1,zt1);
		x1=xn(xe);
		y1=ym(ye);

		xe = Xe(xt2, yt2, zt2);
		ye=Ye(xt2,yt2,zt2);
		x2=xn(xe);
		y2=ym(ye);

		MoveToEx(hdc,x1,y1,NULL);
		LineTo(hdc,x2,y2);
	}


	for(int i=0; i<2; i++)
	{
		xt1 =   Point[i].x; yt1 =   Point[i].y; zt1 =   Point[i].z;
		xt2 = Point[i+2].x; yt2 = Point[i+2].y; zt2 = Point[i+2].z;

		xe = Xe(xt1, yt1, zt1);
		ye=Ye(xt1,yt1,zt1);
		x1=xn(xe);
		y1=ym(ye);

		xe = Xe(xt2, yt2, zt2);
		ye=Ye(xt2,yt2,zt2);
		x2=xn(xe);
		y2=ym(ye);

		MoveToEx(hdc,x1,y1,NULL);
		LineTo(hdc,x2,y2);
	}


}



//функция для создания палитры, которая уменьшает яркось в глубь Z-буфера  
double ZPalet(double z)
{
	double a,b,f;
	a = -2.0*xmax - dCentral;
	b = 0.5*xmax - dCentral;

	if(z < a)
		f = 0;
	if( (z >= a) && (z <= b))
		f = (z - a)/(b-a);
	if(z > b)
		f = 1;
	return f;
}



//"рисует" линию по пикселям, цветом rgb, заполняя Z-буфер zb 
//x1,y1,x2,y2 - координаты линии в пиксельной плоскости
//ze1, ze2 - z-координаты концов линии в видовой системе координат
//на цвет rgb накладывается палитра ZPalet, которая уменьшает яркось в глубь Z-буфера  
void ZbufLine(HDC hdc, int x1, int y1, int x2, int y2,
			  double ze1, double ze2, COLORREF rgb)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = x2 >= x1 ? 1 : -1;
	int sy = y2 >= y1 ? 1 : -1;
    unsigned long p;
    double zz;

	unsigned char r,g,b;


	if(dy <= dx)
	{

		int d = (dy << 1) - dx;
		int d1 = dy << 1;
		int d2 = (dy - dx) << 1;

		if((x1 >= ne1)&&(x1 <= ne2)&&(y1 >= me2)&&(y1 <= me1))
		{
			p = (x1 - ne1) +(y1 - me2)*Np;
			zz = ze1;
			if(zb[p].z < zz)
			{
				zb[p].z = zz;
				r = unsigned char(rgb);
				g = unsigned char(rgb>>8);
				b = unsigned char(rgb>>16);
				zb[p].c.R = unsigned char(r*ZPalet(zz));
				zb[p].c.G = unsigned char(g*ZPalet(zz));
				zb[p].c.B = unsigned char(b*ZPalet(zz));
			}
		}

		for(int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx)
		{
			if(d > 0)
			{
				d += d2;
				y += sy;
			}
			else
				d += d1;

			if((x >= ne1)&&(x <= ne2)&&(y >= me2)&&(y <= me1))
			{
				p = (x - ne1) +(y - me2)*Np;
				zz = ze1 + (ze2 - ze1)*(x - x1)/(x2 - x1);
			if(zb[p].z < zz)
			{
				zb[p].z = zz;
				r = (unsigned char) rgb;
				g = (unsigned char) (rgb>>8);
				b = (unsigned char) (rgb>>16);
				zb[p].c.R = unsigned char(r*ZPalet(zz));
				zb[p].c.G = unsigned char(g*ZPalet(zz));
				zb[p].c.B = unsigned char(b*ZPalet(zz));
			}
			}
		}
	}
	else
	{

		int d = (dx << 1) - dy;
		int d1 = dx << 1;
		int d2 = (dx - dy) << 1;

		if((x1 >= ne1)&&(x1 <= ne2)&&(y1 >= me2)&&(y1 <= me1))
		{
			p = (x1 - ne1) +(y1 - me2)*Np;
			zz = ze1;
			if(zb[p].z < zz)
			{
				zb[p].z = zz;
				r = (unsigned char) rgb;
				g = (unsigned char) (rgb>>8);
				b = (unsigned char) (rgb>>16);
				zb[p].c.R = unsigned char(r*ZPalet(zz));
				zb[p].c.G = unsigned char(g*ZPalet(zz));
				zb[p].c.B = unsigned char(b*ZPalet(zz));
			}
		}
		for(int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy)
		{
			if(d > 0)
			{
				d += d2;
				x += sx;
			}
			else
				d += d1;
		
			if((x >= ne1)&&(x <= ne2)&&(y >= me2)&&(y <= me1))
			{
				p = (x - ne1) +(y - me2)*Np;
				zz = ze1 + (ze2 - ze1)*(y - y1)/(y2 - y1);
			if(zb[p].z < zz)
			{
				zb[p].z = zz;
				r = (unsigned char) rgb;
				g = (unsigned char) (rgb>>8);
				b = (unsigned char) (rgb>>16);
				zb[p].c.R = unsigned char(r*ZPalet(zz));
				zb[p].c.G = unsigned char(g*ZPalet(zz));
				zb[p].c.B = unsigned char(b*ZPalet(zz));
			}
			}
		
		}
	}
}


//"рисует" линию толщиной w - пикселей, цветом rgb, заполняя Z-буфер
//x1,y12,x2,y2 - координаты линии в пиксельной плоскости
//ze1, ze2 - z-координаты концов линии в видовой системе координат
void ZbufLineWidth(HDC hdc, int x1, int y1, int x2, int y2,
				   double ze1, double ze2, int w, COLORREF rgb)
{
	
	int n,m;
	if(w == 1)
	{
		ZbufLine(hdc,x1,y1,x2,y2,ze1,ze2,rgb);
	}
	else
	{
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);
		if(dx >= dy)
		{
			m = w/2;
			if(w%2 == 0)
				n = -m +1;
			else
				n = -m;
			for(int i = n; i <= m; i++)
				ZbufLine(hdc,x1,y1+i,x2,y2+i,ze1,ze2,rgb);
		}
		else
		{
			m = w/2;
			if(w%2 == 0)
				n = -m +1;
			else
				n = -m;
			for(int i = n; i <= m; i++)
				ZbufLine(hdc,x1+i,y1,x2+i,y2,ze1,ze2,rgb);
		}
	}


}




//"рисует" параллелограм, цветом rgb, заполняя Z-буфер
//x1,y12,x2,y2,x3,y3,x4,y4 - координаты вершин в пиксельной плоскости
//ze1,ze2,ze3,ze4 - z-координаты вершин в видовой системе координат
void ZbufParallelogram(HDC hdc,int x1,int y1,double ze1,
					   int x2,int y2,double ze2,
					   int x3,int y3,double ze3,
					   int x4,int y4,double ze4,COLORREF rgb)
{

	double zz,zzb;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = x2 >= x1 ? 1 : -1;
	int sy = y2 >= y1 ? 1 : -1;


	if(dy <= dx)
	{

		int d = (dy << 1) - dx;
		int d1 = dy << 1;
		int d2 = (dy - dx) << 1;

		ZbufLine(hdc,x1,y1,x4,y4,ze1,ze4,rgb);
		for(int x = x1 + sx, y = y1, xb = x4 + sx, yb = y4, i = 1;
			i <= dx; i++, x += sx, xb += sx)
		{
			if(d > 0)
			{
				d += d2;
				y += sy;
				yb += sy;
			}
			else
				d += d1;

			zz = ze1 + (ze2 - ze1)*(x - x1)/(x2 - x1);
			zzb = ze4 + (ze3 - ze4)*(xb - x4)/(x3 - x4);
			ZbufLineWidth(hdc,x,y,xb,yb,zz,zzb,2,rgb);
		}
	}
	else
	{

		int d = (dx << 1) - dy;
		int d1 = dx << 1;
		int d2 = (dx - dy) << 1;

		for(int x = x1, y = y1 + sy, xb = x4, yb = y4 + sy, i = 1;
		i <= dy; i++, y += sy, yb += sy)
		{
			if(d > 0)
			{
				d += d2;
				x += sx;
				xb += sx;
			}
			else
				d += d1;
		
			zz = ze1 + (ze2 - ze1)*(y - y1)/(y2 - y1);
			zzb = ze4 + (ze3 - ze4)*(yb - y4)/(y3 - y4);
			ZbufLineWidth(hdc,x,y,xb,yb,zz,zzb,2,rgb);
		
		}
	}
}










