// Prototype.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Prototype.h"
#include <vector>
#include <math.h>
#define REDRAW_CLIENT InvalidateRect(hWnd, NULL, TRUE)
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROTOTYPE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROTOTYPE));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROTOTYPE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROTOTYPE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

// How Can Check Enemy...?
enum class OBJTYPE
{
    OBJTYPE_PLAYER, OBJTYPE_ENEMY, OBJTYPE_END
};

enum class STATE
{
    STATE_FOCUSING,STATE_FREE, STATE_END
};
struct Player
{
    int m_ix;
    int m_iy;
    
    int m_iLook_X;
    int m_iLook_Y;
    
    STATE m_eState;
    OBJTYPE m_eType;

    Player() {};
    Player(const int _x, const int _y)
        :m_ix(_x)
        , m_iy(_y) 
        , m_eType(OBJTYPE::OBJTYPE_PLAYER)
        , m_eState(STATE::STATE_FREE)
        , m_iLook_X(0)
        , m_iLook_Y(0) {};

    void Update()
    {
        m_ix = m_iLook_X;
        m_iy = m_iLook_Y;
    }

    void Render(HDC _hdc)
    {
        HBRUSH hBrush;
        HBRUSH oldBrush;
        switch (m_eType)
        {
        case OBJTYPE::OBJTYPE_PLAYER:
            if (m_eState == STATE::STATE_FOCUSING)
            {
                hBrush = CreateSolidBrush(RGB(0, 255, 0));
                oldBrush = (HBRUSH)SelectObject(_hdc, hBrush);
                Ellipse(_hdc, m_ix - 5, m_iy - 5, m_ix + 5, m_iy + 5);
                DeleteObject(SelectObject(_hdc, oldBrush));
            }
            else
                Ellipse(_hdc, m_ix - 5, m_iy - 5, m_ix + 5, m_iy + 5);
            break;
        case OBJTYPE::OBJTYPE_ENEMY:
            hBrush = CreateSolidBrush(RGB(255, 0, 0));
            oldBrush = (HBRUSH)SelectObject(_hdc, hBrush);
            Ellipse(_hdc, m_ix - 5, m_iy - 5, m_ix + 5, m_iy + 5);
            DeleteObject(SelectObject(_hdc, oldBrush));
            break;
        }
    }

};

int DrawPlayers(HDC _hdc,std::vector<Player*>& _vecPlayer)
{
    for (int i = 0; i < _vecPlayer.size(); ++i)
        _vecPlayer[i]->Render(_hdc);
    return 0;
}

int PlayerBucketClear(std::vector<Player*>& _bucket)
{
    for (int i = 0; i < _bucket.size(); ++i)
    {
        _bucket[i]->m_eState = STATE::STATE_FREE;
    }
    _bucket.clear();
    return 0;
}

int CheckPlayerInRect(const RECT& _rect, std::vector<Player*>& _bucket , std::vector<Player*>& _vecP)
{
    // If bucket is not emty then clear Bucket
    // Because ReFocusing Player
    PlayerBucketClear(_bucket);
    for (int i = 0; i < _vecP.size(); ++i)
    {
        if (_rect.left <= _vecP[i]->m_ix && _vecP[i]->m_ix <= _rect.right
            && _rect.top <= _vecP[i]->m_iy && _vecP[i]->m_iy <= _rect.bottom)
        {
            _vecP[i]->m_eState = STATE::STATE_FOCUSING;
            _bucket.push_back(_vecP[i]);
        }
    }
    return 0;
}

int SetGroupRect(RECT& _rect,std::vector<Player*>_vecPlayer)
{
    _rect.left = _vecPlayer[0]->m_ix;
    _rect.top = _vecPlayer[0]->m_iy;
    _rect.right = _vecPlayer[0]->m_ix;
    _rect.bottom = _vecPlayer[0]->m_iy;
    
    for (int i = 1; i < _vecPlayer.size(); ++i)
    {
        if (_vecPlayer[i]->m_ix < _rect.left)
            _rect.left = _vecPlayer[i]->m_ix;
        if (_rect.right < _vecPlayer[i]->m_ix)
            _rect.right = _vecPlayer[i]->m_ix;
        if (_vecPlayer[i]->m_iy < _rect.top)
            _rect.top = _vecPlayer[i]->m_iy;
        if (_rect.bottom < _vecPlayer[i]->m_iy)
            _rect.bottom = _vecPlayer[i]->m_iy;
    }
    return 0;
}


int SetLookVecInRect(RECT& _rect, std::vector<Player*>_vecPlayer)
{
    int width_pivot =  (_rect.right - _rect.left) / 2;
    int height_pivot = (_rect.top - _rect.bottom) / 2;

    // 사각형 기준으로 차이점을 구한다 .
    for (int i = 0; i < _vecPlayer.size(); ++i)
    {
        _vecPlayer[i]->m_iLook_X = _vecPlayer[i]->m_ix - (_rect.left + width_pivot);
        _vecPlayer[i]->m_iLook_Y = _vecPlayer[i]->m_iy - (_rect.bottom + height_pivot);
        printf("원래의 사각형과 %d 번째 점의 차이 X : %d , Y ; %d\n", i+1,_vecPlayer[i]->m_iLook_X, _vecPlayer[i]->m_iLook_Y);
    }
    printf("원래의 사각형의 기준점 X : %d Y : %d\n", (_rect.left + width_pivot), (_rect.bottom + height_pivot));

    return 0;
}

int SetRealLookVector(const int _originX,const int _originY,const int _clientX,const int _clientY, std::vector<Player*>_vecPlayer)
{
    printf("이동할 점 X : %d Y : %d\n", _clientX, _clientY);

    // 사각형 기준으로 차이점을 구한다 .
    for (int i = 0; i < _vecPlayer.size(); ++i)
    {
        // ㄴ ㅏ뭐함 ?? 뇌빼고있네
        /*_vecPlayer[i]->m_iLook_X = _clientX - _originX;
        _vecPlayer[i]->m_iLook_Y = _clientY - _originY;*/
        _vecPlayer[i]->m_iLook_X = _clientX + _vecPlayer[i]->m_iLook_X;
        _vecPlayer[i]->m_iLook_Y = _clientY + _vecPlayer[i]->m_iLook_Y;
        printf("%d 번째 이동량 X : %d , Y : %d\n", i + 1, _vecPlayer[i]->m_iLook_X, _vecPlayer[i]->m_iLook_Y);
    }
    return 0;
}

int SetLookVector(const int _x,const int _y,RECT& _rect, std::vector<Player*>_vecPlayer)
{
    // 임시로 그냥 위치 박기
    if (1 < _vecPlayer.size())
    {
        // 사각형을 세팅한다
        SetGroupRect(_rect, _vecPlayer);
        // 사각형을 기준으로 플레이어들의 차이값을 계산한다.
        SetLookVecInRect(_rect, _vecPlayer);
        // 사각형을 옮기고 기존에 있던 중점을 구한다
        int X_pivot = (_rect.left + (_rect.right - _rect.left) / 2);
        int Y_pivot = (_rect.bottom + (_rect.top - _rect.bottom) / 2);
        SetRealLookVector(X_pivot, Y_pivot,_x,_y, _vecPlayer);
        // Test
        for (int i = 0; i < _vecPlayer.size(); ++i)
        {
            _vecPlayer[i]->Update();
        }
    }
    else if(1 == _vecPlayer.size())
    {
        _vecPlayer[0]->m_iLook_X = _x;
        _vecPlayer[0]->m_iLook_Y = _y;
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // UseMousePos
    static POINT p;

    static RECT MouseRect;
    
    // use GroupMove Algorythm
    static RECT GroupBox;
    
    static bool IsRectPosSet = false;
    
    static std::vector<Player*> vecPlayer;
    static std::vector<Player*> vecFocusPlayer;
    switch (message)
    {
    case WM_KEYDOWN:
        // Press Space then make player JustPrototype so useful stl Vector use
        if (wParam == VK_SPACE)
        {
            GetCursorPos(&p);
            ScreenToClient(hWnd, &p);
            Player* tmp = new Player(p.x, p.y);
            vecPlayer.push_back(tmp);
            REDRAW_CLIENT;
        }
        break;
    case WM_RBUTTONDOWN:
        GetCursorPos(&p);
        ScreenToClient(hWnd, &p);
        SetLookVector(p.x,p.y,GroupBox,vecFocusPlayer);

        REDRAW_CLIENT;
        break;
    case WM_LBUTTONDOWN:
        // MakeCursor Rect LT
        MouseRect.left = LOWORD(lParam);
        MouseRect.top = HIWORD(lParam);
        IsRectPosSet = true;
        break;
    case WM_MOUSEMOVE:
        // MakeCursor Rect RB
        if (IsRectPosSet)
        {
            MouseRect.right = LOWORD(lParam);
            MouseRect.bottom = HIWORD(lParam);
            REDRAW_CLIENT;
        }
        break;
    case WM_LBUTTONUP:
        IsRectPosSet = false;
        CheckPlayerInRect(MouseRect, vecFocusPlayer, vecPlayer);
        REDRAW_CLIENT;
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
            Rectangle(hdc, MouseRect.left, MouseRect.top, MouseRect.right, MouseRect.bottom);
            if(1 < vecFocusPlayer.size())
                Rectangle(hdc, GroupBox.left, GroupBox.top, GroupBox.right, GroupBox.bottom);
            DrawPlayers(hdc, vecPlayer);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        for (auto iter = vecPlayer.begin(); iter != vecPlayer.end(); ++iter)
            delete (*iter);
        vecPlayer.clear();
        vecFocusPlayer.clear();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
