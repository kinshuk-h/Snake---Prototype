#include "Game\UI_Element.hpp"
#include "Game\Classes.hpp"
using namespace std;
using namespace Game;

const int max_cherries = 20;

int main()
{
    try
    {
        Console::Init(); Console::Size(102,50); Console::BufferSize(102,50); Console::MouseInput(0); Console::Cursor(false);
        SetConsoleTitle("Snake - The Game"); Snake MySnake = {{8,2},{7,2},{6,2},{5,2},{4,2},{3,2},{2,2}}; LevelMap block;
        Console::Border(102,42);
        INPUT_RECORD Rec; DWORD EventCount; short lvl = 0, cherry_ctr = max_cherries; long long ticks = 1;
        bool init = false, resume = true, disp1 = false, disp2 = false, run = true, choice = false, power = false;
        string lvlpath; Cherry cchry(MySnake, block); PowerUp cpow(MySnake, block); Coordinate c1,c2;

        InfoBox("Welcome! Your objective is to help the Snake \neat as much cherries (or the red 0s) as possible."
                "\n\nThe direction buttons help in \nmovement, and 'p' pauses the game."
                "\n\nTry not to bite yourself. Have fun!\n\n~Kinshuk Vasisht","Welcome to Snake!",[](){});
        QuestionBox("Would you like to switch to Hard Difficulty?","Game Status",
                    [&MySnake,&choice](){MySnake.speed(Speed::Hard); choice=true;},
                    [&MySnake,&choice](){MySnake.speed(Speed::Easy); choice=false;});
        MySnake.enable(Snake::Power::Nil); power = true;
        Radio_Button R1(1,43,RED,WHITE), R2(10,43,RED,WHITE), R3(24,43,RED,WHITE), R4(41,43,RED,WHITE), R5(50,43,RED,WHITE);
        GOTO(0,44); std::cout<<"     None     Radiation     Pass Through     Fast     Slow";
        R1.Check(true); R1.Print(); R2.Print(); R3.Print(); R4.Print(); R5.Print();
        Radio_Button R6(1,46,RED,WHITE), R7(12,46,RED,WHITE), R8(24,46,RED,WHITE), R9(38,46,RED,WHITE), *PR = &R1;
        GOTO(0,47); std::cout<<"     Shrink     No Grow     Fast Grow     Magnet";
        R1.Check(true); R6.Print(); R7.Print(); R8.Print(); R9.Print();
		do
        {
            //GOTO(30,41); Console::Color(WHITE); cout.fill('0'); cout<<setw(6)<<ticks; Sleep(40);
            if(ticks>10000) {ticks=0;} MySnake.apply_power(block);
            if(cherry_ctr>=max_cherries)
            {
                MySnake.reload({{8,2},{7,2},{6,2},{5,2},{4,2},{3,2},{2,2}}); MySnake.move_direction() = Snake::Direction::Nil;
                lvl++; lvlpath = "Game\\Levels\\"+string(choice?"Hard":"Easy")+"\\Level "+to_string(lvl)+".map";
                if(Exist(lvlpath)) { cherry_ctr=0; block.load(lvlpath); block.print(); }
                else {run = false; break;} init=false;
            }
            //if( power && ticks%500==0) { power = false; MySnake.disable(); disp2 = false; }
            if(!disp1) { cchry.spawn(); cchry.print(); disp1 = true; }
            //if(!disp2 && ticks%100==0 && !power) { cpow.spawn(cchry.pt()); cpow.print(); disp2 = true; power = true; }
            //if(disp2 && ticks%300==0 && power)  { cpow.clear(); disp2 = false; }
            if(MySnake.hit_itself())          { InfoBox("Self Hit!","",[](){}); run = false; break; }
            if(MySnake.hit(block))            { run = false; break; }
            //if(MySnake.hit(cpow.pt())&&disp2) { MySnake.enable(cpow.powerup()); power = true; }
            if(MySnake.hit(cchry.pt()))       { cherry_ctr++; MySnake.grow(); cchry.clear(); disp1 = false; }
            MySnake.print();
            GetNumberOfConsoleInputEvents(Console::In(),&EventCount);
            Console::Title("Level "+to_string(lvl)+" : "+to_string(cherry_ctr)+
                            string(cherry_ctr==1?" Cherry":" Cherries")+" Consumed.");
            if(EventCount>0)
            {
                ReadConsoleInput(Console::In(),&Rec,1,&EventCount);
                if(Rec.EventType==KEY_EVENT)
                {
                    if(!init && Rec.Event.KeyEvent.bKeyDown) { MySnake.move_direction() = Snake::Direction::Right; init = true; }
                    if(Rec.Event.KeyEvent.wVirtualKeyCode == VK_UP && Rec.Event.KeyEvent.bKeyDown
                        && MySnake.move_direction()!=Snake::Direction::Down ) { MySnake.up(); resume=true; }
                    else if(Rec.Event.KeyEvent.wVirtualKeyCode == VK_DOWN && Rec.Event.KeyEvent.bKeyDown
                        && MySnake.move_direction()!=Snake::Direction::Up) { MySnake.down(); resume=true; }
                    else if(Rec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT && Rec.Event.KeyEvent.bKeyDown
                        && MySnake.move_direction()!=Snake::Direction::Right) { MySnake.left(); resume=true; }
                    else if(Rec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT && Rec.Event.KeyEvent.bKeyDown
                        && MySnake.move_direction()!=Snake::Direction::Left) { MySnake.right(); resume=true; }
                    else if(Rec.Event.KeyEvent.uChar.AsciiChar=='p' && Rec.Event.KeyEvent.bKeyDown && resume)
                    { MySnake.move_direction() = Snake::Direction::Nil;
                      InfoBox("Game has been Paused! \nPress the Direction Keys"
                              " to continue.","Game Status",[](){}); resume=false; }
                    else continue;
                }
                else if(Rec.EventType==MOUSE_EVENT && Rec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED
                        && Rec.Event.MouseEvent.dwEventFlags == 0)
                {
                    PR->Check(false); power = false; MySnake.disable();
                    if(R1.IsClicked(Rec.Event.MouseEvent.dwMousePosition)) { PR = &R1; }
                    else if(R2.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R2; power = true; MySnake.enable(Snake::Power::Radiation); }
                    else if(R3.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R3; power = true; MySnake.enable(Snake::Power::Pass_Through); }
                    else if(R4.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R4; power = true; MySnake.enable(Snake::Power::Fast); }
                    else if(R5.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R5; power = true; MySnake.enable(Snake::Power::Slow); }
                    else if(R6.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R6; power = true; MySnake.enable(Snake::Power::Shrink); }
                    else if(R7.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R7; power = true; MySnake.enable(Snake::Power::NoGrow); }
                    else if(R8.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R8; power = true; MySnake.enable(Snake::Power::FastGrow); }
                    else if(R9.IsClicked(Rec.Event.MouseEvent.dwMousePosition))
                    { PR = &R9; power = true; MySnake.enable(Snake::Power::Magnet); }
                    else { PR = &R1; } PR->Check(true);
                }
            }
            else
            {
                if(MySnake.move_direction() == Snake::Direction::Up) { MySnake.up(); }
                else if(MySnake.move_direction() == Snake::Direction::Down) { MySnake.down(); }
                else if(MySnake.move_direction() == Snake::Direction::Left) { MySnake.left(); }
                else if(MySnake.move_direction() == Snake::Direction::Right) { MySnake.right(); }
            }
            if(resume) ticks++;
        }
        while(run);
        if(cherry_ctr>=max_cherries)
        {
            InfoBox("Congratulations! You won! Your Final Score was "+to_string(cherry_ctr)+" cherries, in Level "
            +to_string(lvl)+" at "+string(choice?"Hard Difficulty.":"Easy Difficulty"),"Game Status",[](){system("cls");});
        }
        else
        {
            InfoBox("Game Over! Your Final Score was "+to_string(cherry_ctr)+" cherries, in Level "+to_string(lvl)+" at "
            +string(choice?"Hard Difficulty.":"Easy Difficulty"),"Game Status",[](){system("cls");});
        }
        Console::To_Original(); Console::Size(80,25); return 0;
    }
    catch(exception& e) { cout<<e.what()<<endl; }
}
