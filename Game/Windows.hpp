#ifndef WINDOWS_HPP_INCLUDED
#define WINDOWS_HPP_INCLUDED

#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>

#define GOTO Console::CursorAt

/// Text Attribute Color Enumerations
namespace Color
{
  enum class Foreground : WORD { Black=0, Dark_Blue, Dark_Green, Dark_Turquoise, Brown, Purple, Olive,
                                 Gray, Dark_Gray, Blue, Green, Turquoise, Red, Magenta, Yellow, White };
  enum class Background : WORD { Black=0, Dark_Blue=16, Dark_Green=32, Dark_Turquoise=48, Brown=64, Purple=80, Olive=96, Gray=112,
                                 Dark_Gray=128, Blue=144, Green=160, Turquoise=176, Red=192, Magenta=208, Yellow=224, White=240 };
};

///Border Items
const unsigned char DC = 0xCA, UC = 0xCB, ULB = 201, DLB = 200, URB = 187, DRB = 188, HT = 205,
                    VT = 186, LC = 0xCC, RC = 0xB9, NC = 0xCE;

class Console
{
    friend class BUTTON_TYPE;
    friend class Box;
    static HANDLE in, out; static HWND CsWnd;
    static CONSOLE_CURSOR_INFO cursorInfo;
    static CONSOLE_SCREEN_BUFFER_INFOEX csbix;
    static RECT Pos; static bool ictr; static DWORD state;
    static std::map<size_t, SMALL_RECT> Cur_UI_Elm;
    Console(){}
    /// Allows UI Element Classes to reserve space for objects on the Console Window.
    static size_t Reserve(SHORT X1, SHORT Y1, SHORT X2, SHORT Y2)
    { SMALL_RECT R = {X1,Y1,X2,Y2}; Cur_UI_Elm.insert(std::make_pair(Cur_UI_Elm.size(),R)); return Cur_UI_Elm.size()-1; }
    /// Releases released space for use on the console.
    static void Release(size_t IDX) { Cur_UI_Elm.erase(IDX); }
public:
    /// Returns the Input Handle
    static HANDLE In()  { return in;  }
    /// Returns the Output Handle
    static HANDLE Out() { return out; }
    /// Return the current areas which are reserved on the console.
    static std::map<size_t,SMALL_RECT> Reserved() { return Cur_UI_Elm; }
    /// Initializes I/O Handles and records initial attributes.
    static void Init()
    {
        if(ictr) {throw std::logic_error(">> Console : Already Initialized!");} csbix.cbSize = sizeof(csbix);
        in  = GetStdHandle(STD_INPUT_HANDLE ); out = GetStdHandle(STD_OUTPUT_HANDLE);
        if(in==INVALID_HANDLE_VALUE||out==INVALID_HANDLE_VALUE)
            throw std::logic_error(">> Console : Unable to retrieve I/O Handles");
        CsWnd = GetConsoleWindow(); if(CsWnd==0) throw std::logic_error(">> Console : Could not retrieve Window Handle!");
        if(!GetConsoleScreenBufferInfoEx(out,&csbix)) throw std::logic_error(">> Console : Could not Initiate CSBI EX!");
        if(!GetConsoleCursorInfo(out,&cursorInfo)) throw std::logic_error(">> Console : Could not Initiate CursorInfo!");
        if(!GetWindowRect(CsWnd, &Pos)) throw std::logic_error(">> Console : Could not Initiate Window Size!");
        if(!GetConsoleMode(in, &state)) {throw std::logic_error(">> Console : Could not Initiate ConsoleMode!");} ictr=1;
        csbix.ColorTable[0] = RGB(0,0,0); csbix.ColorTable[15] = RGB(255,255,255);
        SetConsoleTextAttribute(out,WORD(Color::Foreground::White)); SetConsoleCursorPosition(out,{0,0});
        SMALL_RECT s = {10,10,89,34}; SetConsoleWindowInfo(out,TRUE,&s); csbix.srWindow = s; csbix.wAttributes = WORD(Color::Foreground::White);
        MoveWindow(CsWnd,10,10,80,25,FALSE); SetConsoleScreenBufferSize(out,{80,100}); csbix.dwCursorPosition = {0,0};
        csbix.dwSize = {80,25}; SetConsoleScreenBufferInfoEx(out,&csbix);
    }
    /// Returns the Current Console Window Size
    static COORD CSize()
    {
        CONSOLE_SCREEN_BUFFER_INFOEX cbi; cbi.cbSize = sizeof(cbi); GetConsoleScreenBufferInfoEx(out,&cbi);
        SHORT Width = cbi.srWindow.Right-cbi.srWindow.Left+1, Height = cbi.srWindow.Bottom-cbi.srWindow.Top+1;
        COORD R = {Width,Height}; return R;
    }
    /// Saves Current Settings as Original Settings.
    static void Save()
    {
        if(!ictr) {throw std::logic_error(">> Console : Uninitialized!");} csbix.cbSize = sizeof(csbix);
        if(!GetConsoleScreenBufferInfoEx(out,&csbix)) throw std::logic_error(">> Console : Could not Save CSBI EX!");
        if(!GetConsoleCursorInfo(out,&cursorInfo)) throw std::logic_error(">> Console : Could not Save CursorInfo!");
        if(!GetWindowRect(CsWnd, &Pos)) throw std::logic_error(">> Console : Could not Save Window Size!");
        if(!GetConsoleMode(in,&state)) throw std::logic_error(">> Console : Could not Save ConsoleMode!");
    }
    /// Resets all attributes to original configuration.
    static void To_Original()
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        SetConsoleTextAttribute(out,csbix.wAttributes); SetConsoleCursorPosition(out,csbix.dwCursorPosition);
        MoveWindow(CsWnd,Pos.left,Pos.top,csbix.srWindow.Right-csbix.srWindow.Left,csbix.srWindow.Bottom-csbix.srWindow.Top,FALSE);
        SetConsoleScreenBufferSize(out,csbix.dwSize); SetConsoleWindowInfo(out,TRUE,&csbix.srWindow);
        SetConsoleScreenBufferInfoEx(out,&csbix);
    }
    /// Sets the color (foreground and background) for text attributes of the console.
    static void Color(Color::Foreground Fore, Color::Background Back=Color::Background::Black)
    { if(!ictr) throw std::logic_error(">> Console : Uninitialized"); SetConsoleTextAttribute(out,WORD(Fore)|WORD(Back)); }
    /// Sets the size of the console window. ( In character units or no. of characters )
    static void Size(SHORT W, SHORT H)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        COORD BufferCoord = {200,200}; SetConsoleScreenBufferSize(out,BufferCoord);
        SMALL_RECT SIZE = {0,0,SHORT(W-1),SHORT(H-1)}; SetConsoleWindowInfo(out,TRUE,&SIZE);
        BufferCoord = {W,SHORT(2*H)}; SetConsoleScreenBufferSize(out,BufferCoord);
    }
    /// Sets the position of the console window. ( In pixel units relative to the screen )
    static void Position(LONG LeftPos, LONG TopPos)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        CONSOLE_SCREEN_BUFFER_INFOEX c; c.cbSize = sizeof(c); GetConsoleScreenBufferInfoEx(out,&c);
        MoveWindow(CsWnd,LeftPos,TopPos,c.srWindow.Right-c.srWindow.Left,c.srWindow.Bottom-c.srWindow.Top,FALSE);
        Size(c.srWindow.Right-c.srWindow.Left+1,c.srWindow.Bottom-c.srWindow.Top+1);
    }
    /// Sets the size of the buffer, or the extra space available which requires scrolling.
    static void BufferSize(SHORT w, SHORT h)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        COORD buf = {w,h}; SetConsoleScreenBufferSize(out,buf);
    }
    /// Sets the position of the console window to the center of the screen. ( In pixel units relative to the screen )
    static void Center()
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        CONSOLE_SCREEN_BUFFER_INFOEX c; c.cbSize = sizeof(c); GetConsoleScreenBufferInfoEx(out,&c);
        SHORT Width = c.srWindow.Right-c.srWindow.Left+1, Height = c.srWindow.Bottom-c.srWindow.Top+1;
        LONG X = 630 - (7*Width)/2, Y = 310 - (26*Height)/5;
        LONG X2 = (2240/3) - (40*Width)/9, Y2 = 456 - 8*Height;
        MoveWindow(CsWnd,(Width>130?X2:X),(Height>50?Y2:Y),Width,Height,FALSE); Size(Width,Height);
    }
    /// Moves Cursor on the console to an arbitrary location.
    static void CursorAt(SHORT X, SHORT Y)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        COORD CPos = {X,Y}; SetConsoleCursorPosition(out,CPos);
    }
    /// Toggles support for Mouse Input on console.
    static void MouseInput(bool flag = true)
    {
        DWORD st, fmt = ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT; GetConsoleMode(in,&st);
        bool enabled = ((st&fmt)==fmt); if(!enabled&&flag) SetConsoleMode(in,st|fmt);
        else if(enabled&&(!flag)) { SetConsoleMode(in,st&(~fmt)); }
    }
    /// Checks if current console configuration supports Mouse Input or not.
    static bool IsMouseInputEnabled()
    {
        DWORD fmt = ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT;
        DWORD st; GetConsoleMode(in,&st); return (st&fmt)==fmt;
    }
    /// Toggles Cursor Visbility ON or OFF on the console.
    static void Cursor(bool flag = true)
    {
        CONSOLE_CURSOR_INFO cci;
        GetConsoleCursorInfo(out, &cci);
        if(cci.bVisible!=flag)
        {
            cci.bVisible = flag;
            SetConsoleCursorInfo(out, &cci);
        }
    }
    /// Sets the Title of the Console Window
    static void Title(std::string_view s)
    {
        SetConsoleTitle(s.data());
    }
    /// Draws a Secondary Border on the Console Window, to make it attractive.
    static void Border(SHORT W, SHORT H)
    {
        GOTO(0,0); std::cout<<ULB; for(int i=1;i<(W-1);i++) std::cout<<HT; std::cout<<URB;
        GOTO(0,H-1); std::cout<<DLB; for(int i=1;i<(W-1);i++) std::cout<<HT; std::cout<<DRB;
        for(int i=1;i<H-1;i++) {GOTO(0,i);std::cout<<VT;} for(int i=1;i<H-1;i++) {GOTO(W-1,i);std::cout<<VT;} GOTO(1,1);
    }
    virtual ~Console(){}
};

HANDLE Console::in, Console::out; CONSOLE_CURSOR_INFO Console::cursorInfo; HWND Console::CsWnd; RECT Console::Pos;
bool Console::ictr=0; DWORD Console::state; std::map<size_t,SMALL_RECT> Console::Cur_UI_Elm;
CONSOLE_SCREEN_BUFFER_INFOEX Console::csbix;

/// Prints a std::string with a border around it, as a header.
void Head(std::ostream& obj, std::string_view name, char sym1='-',char sym2='|')
{
    for(SHORT i=1;i<=(Console::CSize().X);i++) obj<<sym1;
    if(name.size()<static_cast<unsigned>(Console::CSize().X))
    {
        obj<<sym2; for(size_t i=1;i<=((Console::CSize().X)-2-name.size())/2;i++) obj<<" "; if(name.size()%2==0) obj<<name;
        else {obj<<name<<" ";} for(size_t i=1;i<=((Console::CSize().X)-2-name.length())/2;i++) obj<<" "; obj<<sym2;
    }
    else { obj<<name; } for(SHORT i=1;i<=(Console::CSize().X);i++) obj<<sym1;
}

/// A special version of Head(), Prints a std::string with a Decorative border around it, as a header.
void SplHead(std::ostream& obj, std::string_view name)
{
    obj<<ULB; for(SHORT i=0;i<(Console::CSize().X)-2;i++) obj<<HT; obj<<URB;
    if(name.length()<static_cast<unsigned>((Console::CSize().X)-2))
    {
        obj<<VT; for(size_t i=1;i<=((Console::CSize().X)-2-name.length())/2;i++) obj<<" ";
        if(name.size()%2!=unsigned(Console::CSize().X%2)) std::cout<<name<<" "; else std::cout<<name;
        for(size_t i=1;i<=((Console::CSize().X)-2-name.length())/2;i++) {obj<<" ";} obj<<char(186);
    }
    else if(name.length()==static_cast<unsigned>((Console::CSize().X)-2)) { obj<<VT<<name<<VT; } else { obj<<name; }
    obj<<DLB; for(size_t i=0;i<static_cast<unsigned>((Console::CSize().X)-2);i++) obj<<HT; obj<<DRB;
}

/// Generates an Information Dialog Box, used to indicate Status/Completion/Requirement/etc.
template<typename fx> void InfoBox(std::string_view content, std::string_view title, fx f)
{
    UINT buttons = MB_OK; int res = MessageBox(nullptr,content.data(),title.data(),MB_ICONINFORMATION|buttons);
    if(res==IDOK) {f();} return;
}

/// Generates an Error Dialog Box, used to indicate Failure/Incompletion/Requirement/etc.
template<typename fx> void ErrorBox(std::string_view content, std::string_view title, fx f)
{
    UINT buttons = MB_OK; int res = MessageBox(nullptr,content.data(),title.data(),MB_ICONERROR|buttons);
    if(res==IDOK) {f();} return;
}

/// Generates an Question Dialog Box, used to confirm Choice/Decision/Change/Option/etc.
template<typename fx1, typename fx2> void QuestionBox(std::string_view content, std::string_view title, fx1 f1, fx2 f2)
{
    UINT buttons = MB_YESNO; int res = MessageBox(nullptr,content.data(),title.data(),MB_ICONQUESTION|buttons);
    if(res==IDYES) {f1();} else if(res==IDNO) {f2();} return;
}

/// Checks if a particular file or directory exists on the current system. Requires complete path.
bool Exist(std::string_view file)
{
    WIN32_FIND_DATA FData;
    HANDLE h = FindFirstFile(file.data(), &FData) ;
    bool found = h != INVALID_HANDLE_VALUE;
    if(found) { FindClose(h); } return found;
}

#endif // WINDOWS_HPP_INCLUDED
