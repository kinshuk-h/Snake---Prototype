#ifndef CLASSES_FOR_SNAKE_GAME_HPP
#define CLASSES_FOR_SNAKE_GAME_HPP

#include <random>
#include <chrono>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <initializer_list>

#include "Defines.hpp"

#define sc(x) static_cast<int>(x)

enum class Speed : unsigned short { Easy=40, Hard=30 };
enum class Screen_Size : unsigned short { XMin = 0, YMin = 0, XMax = 101, YMax = 41 };

struct _COORD;
class Console;
class fstream;
class string_view;
template<class T> class vector;
template<class RAIter, class __TP_Pred> extern void fill(RAIter, RAIter, __TP_Pred);
template<class RAIter, class __TP_Pred> extern RAIter find(RAIter, RAIter, __TP_Pred);

namespace Color
{
    enum class Foreground : WORD;
}

namespace std
{
    extern ostream cout;
}

namespace Game
{
    struct Coordinate
    {
        typedef unsigned short size_t;
        short X,Y;
        Coordinate(size_t x=0, size_t y=0):X(x),Y(y){}
        Coordinate(const COORD& c) : X(c.X),Y(c.Y){}
        Coordinate& operator=(const Coordinate& C2)
        { if(this!=&C2) { X=C2.X; Y=C2.Y; } return *this; }
        COORD data() const noexcept { COORD c; c.X=X; c.Y=Y; return c; }
        bool operator==(const Coordinate& C) { return X==C.X && Y==C.Y; }
        bool operator==(const COORD& C) { return X==C.X && Y==C.Y; }
        friend bool operator==(const COORD& C1, const Coordinate& C2) { return C2.X==C1.X && C2.Y==C1.Y; }
        bool operator!=(const Coordinate& C) { return X!=C.X || Y!=C.Y; }
        bool operator!=(const COORD& C) { return X!=C.X || Y!=C.Y; }
        friend bool operator!=(const COORD& C1, const Coordinate& C2) { return C2.X!=C1.X || C2.Y!=C1.Y; }
        friend std::ostream& operator<<(std::ostream& os, const Coordinate& C2)
        {
            std::stringstream ss; ss<<"("<<C2.X<<","<<C2.Y<<")";
            os<<ss.str(); return os;
        }
    };
    class LevelMap
    {
        enum class Material : unsigned char { Wood='W', Metal='M', Space=' ', Trampoline='T', Glass='G', CherryArea='o' };
        std::vector<std::vector<char>> area; std::vector<std::vector<Coordinate>> pos;
        static int ctr;
    public:
        LevelMap()
        {
            area.resize(40); for(std::vector<char>& v:area) { v.resize(100); std::fill(v.begin(),v.end(),' '); }
            pos.resize(40); for(size_t i=1;i<41;i++) { for(size_t j=1;j<101;j++) pos[i-1].emplace_back(j,i); }
        }
        void load(std::string_view f)
        {
            std::fstream file; file.open(f.data(),std::ios::in); size_t i=0,j=0; char ch;
            while(!file.eof())
            {
                ch = file.get();
                if(ch=='T'||ch=='M'||ch=='W'||ch=='o'||ch==' '||ch=='G') area[i][j++] = ch;
                else {i++; j=0;}
            }
            file.close();
        }
        char at(const Coordinate& c)
        {
            if(c.Y<=41 && c.X<=101 && c.X>=1 && c.Y>=1) return (area[c.Y-1][c.X-1]);
            return '\0';
        }
        void set(const Coordinate& c, char ch)
        {
            if(c.Y<=41 && c.X<=101 && c.X>=1 && c.Y>=1) area[c.Y-1][c.X-1]=ch;
        }
        void print()
        {
            for(size_t i=0;i<40;i++)
            {
                for(size_t j=0;j<100;j++)
                {
                    Console::CursorAt(pos[i][j].X,pos[i][j].Y);
                    switch(static_cast<Material>(area[i][j]))
                    {
                        case Material::Trampoline : { Console::Color(GREEN,BG_DARK_GREEN); std::cout<<char(219); } break;
                        case Material::Space : { Console::Color(WHITE); std::cout<<' '; } break;
                        case Material::Wood : { Console::Color(BROWN); std::cout<<char(219); } break;
                        case Material::CherryArea : { Console::Color(BLACK); std::cout<<char(219); } break;
                        case Material::Metal : { Console::Color(GRAY); std::cout<<char(219); } break;
                        case Material::Glass : { Console::Color(TURQUOISE); std::cout<<char(219); } break;
                    }
                }
            }
            Console::CursorAt(0,0);
        }
        void render()
        {
            for(size_t i=0;i<40;i++)
            {
                for(size_t j=0;j<100;j++)
                {
                    if(area[i][j]!=' ')
                    {
                        Console::CursorAt(pos[i][j].X,pos[i][j].Y);
                        switch(static_cast<Material>(area[i][j]))
                        {
                            case Material::Trampoline : { Console::Color(GREEN,BG_DARK_GREEN); std::cout<<char(219); } break;
                            case Material::Space : { Console::Color(WHITE); std::cout<<' '; } break;
                            case Material::Wood : { Console::Color(BROWN); std::cout<<char(219); } break;
                            case Material::CherryArea : { Console::Color(BLACK); std::cout<<char(219); } break;
                            case Material::Metal : { Console::Color(GRAY); std::cout<<char(219); } break;
                            case Material::Glass : { Console::Color(TURQUOISE); std::cout<<char(219); } break;
                        }
                    }
                }
            }
        }
    };
    int LevelMap::ctr = 0;
    class Snake
    {
    public:
        enum class Direction : unsigned short { Nil=0, Up, Down, Left, Right };
        enum class Power : unsigned short { Nil=0, Slow, Fast, Pass_Through, Radiation, Magnet, Shrink, NoGrow, FastGrow };
        Snake()
        {
            pow = Power::Nil; c_dir = Direction::Nil; sd = static_cast<short>(Speed::Easy);
            self_hit = true; FC1 = BLUE; FC2 = GREEN; body=char(219); growth=1;
        }
        Snake(std::initializer_list<Coordinate> ls) : Snake() { for(Coordinate c:ls) { S.push_back(c); } }
        Coordinate head() const { return S[0]; }
        Coordinate tail() const { return S[S.size()-2]; }
        Direction& move_direction() noexcept { return c_dir; }
        void reload(std::initializer_list<Coordinate> ls)
        { S.resize(ls.size()); size_t i=0; for(Coordinate c:ls) {S[i++]=c;} }
        void speed(Speed spd=Speed::Easy) { sd = static_cast<short>(spd); }
        void enable_self_hit() { self_hit = true; }
        void enable(Power p)
        {
            pow = p; switch(pow)
            {
                case Power::Pass_Through : body = char(176); self_hit = false; break;
                case Power::Radiation : FC2 = RED; break;
                case Power::Shrink : S.resize(7); pow=Power::Nil; break;
                case Power::NoGrow : FC1 = RED; FC2 = YELLOW; growth=0; break;
                case Power::FastGrow : FC1 = YELLOW; FC2 = TURQUOISE; growth+=2; break;
                case Power::Magnet : FC1 = DARK_GREEN; FC2 = GRAY; break;
                case Power::Fast : sd-=10; break;
                case Power::Slow : sd+=10; break;
                default : break;
            }
        }
        void apply_power(LevelMap& l)
        {
            if(pow!=Power::Nil)
            {
                switch(pow)
                {
                    case Power::Pass_Through :
                        if(l.at(S[S.size()-1])!=' '&&l.at(S[S.size()-1])!='o') { l.render(); break; }
                        break;
                    case Power::Radiation : Console::Color(BLACK);
                        if(c_dir == Direction::Up) { for(int i=S[0].Y;i>(int)Screen_Size::YMin;i--)
                          if(l.at(Coordinate(S[0].X,i))!='M'&&l.at(Coordinate(S[0].X,i))!=' ')
                          { l.set(Coordinate(S[0].X,i),' '); GOTO(S[0].X,i); std::cout<<' '; } }
                        else if(c_dir == Direction::Down) { for(int i=S[0].Y;i<(int)Screen_Size::YMax;i++)
                          if(l.at(Coordinate(S[0].X,i))!='M'&&l.at(Coordinate(S[0].X,i))!=' ')
                          { l.set(Coordinate(S[0].X,i),' '); GOTO(S[0].X,i); std::cout<<' '; } }
                        else if(c_dir == Direction::Left) { for(int i=S[0].X-1;i>(int)Screen_Size::XMin;i--)
                          if(l.at(Coordinate(i,S[0].Y))!='M'&&l.at(Coordinate(i,S[0].Y))!=' ')
                          { l.set(Coordinate(i,S[0].Y),' '); GOTO(i,S[0].Y); std::cout<<' '; } }
                        else if(c_dir == Direction::Right) { for(int i=S[0].X+1;i<(int)Screen_Size::XMax;i++)
                          if(l.at(Coordinate(i,S[0].Y))!='M'&&l.at(Coordinate(i,S[0].Y))!=' ')
                          { l.set(Coordinate(i,S[0].Y),' '); GOTO(i,S[0].Y); std::cout<<' '; } } break;
                    default : break;
                }
            }
        }
        void disable()
        {
            if(pow!=Power::Nil)
            {
                switch(pow)
                {
                    case Power::Pass_Through : self_hit = false; break;
                    case Power::NoGrow : growth=1; break;
                    case Power::FastGrow : growth-=2; break;
                    case Power::Fast : sd+=10; break;
                    case Power::Slow : sd-=10; break;
                    case Power::Radiation : break;
                    default : break;
                }
                pow = Power::Nil; FC1 = BLUE; FC2 = GREEN; body = char(219);
            }
        }
        void grow()
        {
            Direction c_prev_dir = c_dir; c_dir = Direction::Nil;
            //for(int i=0;i<growth;i++)
            //{
            //    if(c_prev_dir == Snake::Direction::Up) { down(); }
            //    else if(c_prev_dir == Snake::Direction::Down) { up(); }
            //    else if(c_prev_dir == Snake::Direction::Left) { right(); }
            //    else if(c_prev_dir == Snake::Direction::Right) { left(); }
            //}
            for(int i=0;i<growth;i++)
            {
                Coordinate c = S[S.size()-1];
                if(c_prev_dir == Snake::Direction::Up) { up(); }
                else if(c_prev_dir == Snake::Direction::Down) { down(); }
                else if(c_prev_dir == Snake::Direction::Left) { left(); }
                else if(c_prev_dir == Snake::Direction::Right) { right(); }
                S.push_back(c);
            }
            //print();
            c_dir = c_prev_dir; //Sleep(100000);//system("pause");
        }
        void up()
        {
            if(c_dir!=Direction::Down)
            {
                c_dir = Direction::Up; Console::CursorAt(S[S.size()-2].X, S[S.size()-2].Y); std::cout<<" ";
                for(size_t i=S.size()-1;i>0;i--) {S[i]=S[i-1];} S[0].Y--;
                if(S[0].Y<static_cast<unsigned short>(Screen_Size::YMin)+1)
                    S[0].Y=static_cast<unsigned short>(Screen_Size::YMax)-1;

            }
        }
        void down()
        {
            if(c_dir!=Direction::Up)
            {
                c_dir = Direction::Down; Console::CursorAt(S[S.size()-2].X, S[S.size()-2].Y); std::cout<<" ";
                for(size_t i=S.size()-1;i>0;i--) {S[i]=S[i-1];} S[0].Y++;
                if(S[0].Y>static_cast<unsigned short>(Screen_Size::YMax)-1)
                    S[0].Y=static_cast<unsigned short>(Screen_Size::YMin)+1;
            }
        }
        void left()
        {
            if(c_dir!=Direction::Right)
            {
                c_dir = Direction::Left; Console::CursorAt(S[S.size()-2].X,S[S.size()-2].Y); std::cout<<" ";
                for(size_t i=S.size()-1;i>0;i--) {S[i]=S[i-1];} S[0].X--;
                if(S[0].X<static_cast<unsigned short>(Screen_Size::XMin)+1)
                    S[0].X=static_cast<unsigned short>(Screen_Size::XMax)-1;
            }
        }
        void right()
        {
            if(c_dir!=Direction::Left)
            {
                c_dir = Direction::Right; Console::CursorAt(S[S.size()-2].X,S[S.size()-2].Y); std::cout<<" ";
                for(size_t i=S.size()-1;i>0;i--) {S[i]=S[i-1];} S[0].X++;
                if(S[0].X>static_cast<unsigned short>(Screen_Size::XMax)-1)
                    S[0].X=static_cast<unsigned short>(Screen_Size::XMin)+1;
            }
        }
        void print()
        {
            Sleep(sd);
            for(size_t i=0;i<S.size()-1;i++)
            {
                Console::CursorAt(S[i].X,S[i].Y); if(i%2){Console::Color(FC1);}
                else{Console::Color(FC2);} std::cout<<body;
            }
            Console::Color(BLACK);
        }
        bool hit(Coordinate c)
        {
            if(pow==Power::Magnet)
            {
                for(size_t i=0;i<S.size();i++)
                {
                    if((S[i].X>c.X-5&&S[i].X<c.X+5)&&(S[i].Y>c.Y-5&&S[i].Y<c.Y+5)) return true;
                }
                return false;
            }
            return S[0]==c;
        }
        bool hit_itself()
        {
            if(self_hit) { return std::find(S.begin()+1,S.end()-1,S[0])!=S.end()-1; }
            return false;
        }
        bool hit(LevelMap l)
        {
            if(pow!=Power::Pass_Through)
            {
                switch(l.at(S[0]))
                {
                    case 'T' : for(size_t i=0;i<S.size()/2;i++) std::swap(S[i],S[S.size()-i-1]);
                               if(c_dir==Direction::Up) c_dir = Direction::Down;
                               if(c_dir==Direction::Down) c_dir = Direction::Up;
                               if(c_dir==Direction::Right) c_dir = Direction::Left;
                               if(c_dir==Direction::Left) c_dir = Direction::Right;
                               return false;
                    case 'G' : l.set(S[0],' '); return (sd>30?true:false);
                    case ' ' : return false;
                    case 'o' : return false;
                    default : return true;
                }
            }
            return false;
        }
    private:
        std::vector<Coordinate> S; Direction c_dir; Power pow; bool self_hit;
        Color::Foreground FC1, FC2; unsigned char body; short growth, sd;
    };
    class Cherry
    {
    protected:
        Snake* p_snake; LevelMap* p_map; Coordinate c;
    public:
        std::uniform_int_distribution<int> rangeX {sc(Screen_Size::XMin)+2,sc(Screen_Size::XMax)-2};
        std::uniform_int_distribution<int> rangeY {sc(Screen_Size::YMin)+2,sc(Screen_Size::YMax)-2};
        std::default_random_engine re;
        Cherry(Snake& snake, LevelMap& amap)
        { p_snake = &snake; p_map = &amap; }
        virtual Coordinate spawn()
        {
            unsigned int seed; Coordinate cc;
            do { seed = std::chrono::system_clock::now().time_since_epoch().count();
                 re.seed(seed); cc.X = rangeX(re); cc.Y = rangeY(re); }
            while(p_snake->hit(cc)||cc==c||p_map->at(cc)!=' '); c=cc; return c;
        }
        virtual void print()
        { Console::CursorAt(c.X,c.Y); Console::Color(RED); std::cout<<char(4); Console::Color(BLACK); }
        void clear()
        { Console::CursorAt(c.X,c.Y); Console::Color(BLACK); std::cout<<" "; }
        Coordinate pt() const noexcept { return c; }
    };
    class PowerUp : public Cherry
    {
        Snake::Power pow;
    public:
        std::uniform_int_distribution<unsigned short> pRange{static_cast<unsigned short>(Snake::Power::Slow)+0,
                                                             static_cast<unsigned short>(Snake::Power::FastGrow)+0};
        PowerUp(Snake& snake, LevelMap& amap) : Cherry(snake,amap) {  }
        Coordinate spawn(Coordinate cherry)
        {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); re.seed(seed);
            pow = static_cast<Snake::Power>(pRange(re));
            pow = Snake::Power::Pass_Through; Coordinate temp; do{ temp = Cherry::spawn(); } while(temp==cherry);
            return temp;
        }
        Snake::Power powerup() const noexcept { return pow; }
        void print()
        {
            Console::CursorAt(c.X,c.Y);
            switch(pow)
            {
                case Snake::Power::Slow : Console::Color(YELLOW); std::cout<<"v";
                case Snake::Power::Fast : Console::Color(TURQUOISE); std::cout<<"^";
                case Snake::Power::Shrink : Console::Color(DARK_GREEN,BG_WHITE); std::cout<<"@";
                case Snake::Power::FastGrow : Console::Color(PURPLE,BG_WHITE); std::cout<<"@";
                case Snake::Power::Radiation : Console::Color(BLUE,BG_RED); std::cout<<"+";
                case Snake::Power::NoGrow : Console::Color(MAGENTA,BG_WHITE); std::cout<<"@";
                case Snake::Power::Pass_Through : Console::Color(DARK_TURQUOISE); std::cout<<"~";
                case Snake::Power::Magnet : Console::Color(RED,BG_WHITE); std::cout<<"U";
                default : break;
            }
            Console::Color(BLACK);
        }
    };
}

#endif // CLASSES_FOR_SNAKE_GAME_HPP
