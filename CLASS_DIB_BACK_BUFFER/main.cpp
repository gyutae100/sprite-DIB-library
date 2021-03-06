///@mainpage    DIB 버퍼 및 스프라이트 관리자를 이용한 스프라이프 클리핑 및 컬러키 테스트한다.  
///@section intro 소개
///- 부분1 : 백버퍼용 DIB 버퍼가 존재한다.
///- 부분2 : BMP 파일을 읽어 스프라이트로 보관하는 스프라이트 관리자가 존재한다.
///- 부분3 : 백버퍼용 DIB에 스프라이트를 출력한다.
///- 부분4 : 스프라이트를 출력시 출력 스프라이트가 이미지인 경우는 클리핑(짜름)만 적용한다.
///- 부분5 : 스프라이트를 출력시 출력 스프라이트가 스프라이트인 경우 클리핑(짜름) 및 컬러 키(스프라이트 배경은 제외)를 적용한다.
///@section Program 프로그램명
///- 프로그램 명  :   SPRITE-TEST
///- 프로그램 내용    : 마우스 좌표에 스프라이트가 이동하게 만들어서 클리핑 및 컬러키가 제대로 적용되는지 확인한다.
///@section CREATEINFO 작성정보
///- 작성자      :   김규태(gyutae100@gmail.com) / 원본 : 프로카데미 학원 자료
///- 작성일      :   2018/05/22
///@section MODIFYINFO 수정정보
///- 수정자/수정일   : 작성자와 동일 / 원본 : 프로카데미 학원 자료 
///- 2018.06.02    : Doxygen 주석화 및 리펙토링
///- 2018.06.05    : 리펙토링 재작업
///-@version 1.0


/**
@file main.cpp

@date 2018/06/02

@brief  스프라이트 클리핑 예제이다.
@brief  마우스를 이용한 스프라이트 중심 좌표 설정을 통한 스프라이트 클리핑 및 컬러키를 테스트한다.
*/

#include "stdafx.h"
#include "main.h"


typedef enum SPRITE_SETTING {  ///< 스프라이트 정보 셋팅
	MAX_LOAD_SPRITE = 100	   ///< 최대 스프라이트 갯수

}SPRITE_SETTING;

typedef enum SCREEN_RESOLUTION {  ///<  화면 해상도 셋팅 
	SCREEN_HEIGHT= 480, ///< 높이
	SCREEN_WIDTH =640   ///< 너비
}SCREEN_RESOLUTION;



typedef enum SPRITE  ///< 스프라이트 별 네임에 매핑하는 인덱스를 부여한다. 추후 추가한다.
{
};



//------------------------------------------------------------------------------
//	전역 싱글톤 DIB 스크린 생성한다.
//------------------------------------------------------------------------------
static CSCREEN_DIB::CScreenDib g_screen_dib(SCREEN_RESOLUTION::SCREEN_WIDTH, SCREEN_RESOLUTION::SCREEN_HEIGHT, 32);

//------------------------------------------------------------------------------
//	전역 SPRITE 관리자 생성한다.
//------------------------------------------------------------------------------
CSPRITE_DIB::CSpriteDib g_csprite(10, 0x00ffffff);


void Update(void);	   ///< 게임 화면을 갱신한다.
BOOL GameInit(void);   ///< 게임 초기 설정한다.

HWND g_hwnd;	///< 윈도우 핸들

int g_x;		///< x마우스 좌표
int g_y;		///< y마우스 좌표


// 전역 변수:
HINSTANCE hInst;												 ///< 현재 인스턴스입니다.
WCHAR szTitle[SPRITE_SETTING::MAX_LOAD_SPRITE];                  ///< 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[SPRITE_SETTING::MAX_LOAD_SPRITE];            ///< 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, SPRITE_SETTING::MAX_LOAD_SPRITE);
    LoadStringW(hInstance, IDC_CLASSDIBBACKBUFFER, szWindowClass, SPRITE_SETTING::MAX_LOAD_SPRITE);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLASSDIBBACKBUFFER));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLASSDIBBACKBUFFER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLASSDIBBACKBUFFER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.


   //윈도우 화면 생성한다.
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, SCREEN_RESOLUTION::SCREEN_WIDTH+400, SCREEN_RESOLUTION::SCREEN_HEIGHT+400,
	   nullptr, nullptr, hInstance, nullptr);


   //전역 헨들 저장
   g_hwnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	
	case WM_CREATE:

		//게임 초기화한다.
		GameInit();
			break;

	case WM_MOUSEMOVE: {

		//마우스를 움직 일 때 마다 마우스 좌표를 저장한다.
		g_x = LOWORD(lParam);
		g_y = HIWORD(lParam);

		//움직인 마우스 좌표에 따라 화면을 업데이트한다.
		//추후 인발리트 렉트를 호출해서 변경하자.	

		RECT rt;
		GetClientRect(hWnd, &rt);
		InvalidateRect(g_hwnd, &rt, false);

		break;
	}


    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.

            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
			
			Update();

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}








//게임 초기화 함수
BOOL GameInit(void)
{
	//맵 초기화 한다.
	g_csprite.LoadDibSprite(0, L"SpriteData\\_Map.bmp", 0, 0);

	//스프라이트 초기화한다.
	g_csprite.LoadDibSprite(1, L"SpriteData\\Attack1_L_01.bmp", 71, 90);
	g_csprite.LoadDibSprite(2, L"SpriteData\\Attack1_L_02.bmp", 71, 90);
	g_csprite.LoadDibSprite(3, L"SpriteData\\Attack1_L_03.bmp", 71, 90);

	return true;
}





//DIB 이용하여 윈도우 화면에 출력한다.
void Update(void)
{
	//테스트용1
	//g_screen_dib.~CScreenDib();

	//------------------------------------------------------------------------------
	//	출력 버퍼 포인터 및 정보얻는다.
	//------------------------------------------------------------------------------
	BYTE *image_buffer = g_screen_dib.GetDibBuffer();
	int dest_width = g_screen_dib.GetWidth();
	int dest_height = g_screen_dib.GetHeight();
	int dest_pitch = g_screen_dib.GetPitch();

	//------------------------------------------------------------------------------
	//	1. 맵 화면 출력한다.
	//------------------------------------------------------------------------------
	g_csprite.DrawImage(0, 0, 0, image_buffer, dest_width, dest_height, dest_pitch);

	//------------------------------------------------------------------------------
	//	2. 스프라이트 출력한다.
	//------------------------------------------------------------------------------
	g_csprite.DrawSprite(1, g_x, g_y, image_buffer, dest_width, dest_height, dest_pitch);
	g_csprite.DrawSprite(2, 200, 200, image_buffer, dest_width, dest_height, dest_pitch);
	g_csprite.DrawSprite(3, 300, 300, image_buffer, dest_width, dest_height, dest_pitch);

	//테스트용2
	g_csprite.ReleaseSprite(2);

	//------------------------------------------------------------------------------
	//	ScreenDib를 화면으로 플립한다.
	//------------------------------------------------------------------------------
	g_screen_dib.DrawBuffer(g_hwnd);
}