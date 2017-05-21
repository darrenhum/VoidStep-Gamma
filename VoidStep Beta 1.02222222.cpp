#include<allegro.h>
#include<stdio.h>
#include<iostream>
#include<math.h>
#define maxbullets 1000 

const float ENb_speed = 0.5;
const int screen_h = 750;
const int screen_w = 500;

using namespace std;

//Timers
volatile long speed_counter = 0;
void increment_speed_counter(){
	speed_counter++;
}END_OF_FUNCTION(increment_speed_counter);
//Menus
void DisplayMenu(int &Num,int &Num_Player);
void PauseScreen(int &Num,BITMAP* background);
//EXPLOSION FRAME RESET
int ex_reset(int num);
//Force Field frame reset
int FFframe_reset(int num);
//Collsion
int collide(float p1_bb_left,float p1_bb_top,float p1_width, float p1_height, float p2_bb_left,float p2_bb_top, float p2_width, float p2_height);
//Detect Keyrelease
int keyrel(int k);


struct pos
{
    int x;
    int y;
};
//----------------------------------------------------------------------------------------------------------------
class moveable
{
    public:
        moveable(){HP = 0;}
        void setup(int,int,float,float,int);
        void updatepos(){global_x+= x_speed; global_y+=y_speed;}
        float x(){return global_x;}
        float y(){return global_y;}
        bool ready(){return ee != 0;}
        bool dead(){return HP <= 0;}
        void kill(){if(global_y > screen_h || global_y < -60)HP = 0;}
        void shoot(){ee=1;}
        void NOTshoot(){ee=0;}
        double x_speed;
        double y_speed;
        double pj_x;
        double pj_y;
        double pjx_speed;
        double pjy_speed;
        double testxspeed;
        double testyspeed;
        bool FFshow;
        bool ex;
        int x_change;
        int y_change;
        int type;
        void hurt(){HP -= 1;}
        void insta_death(){HP = 0;}
    private:
        float global_x;
        float global_y;
        int shoot_counter;
        int HP;
        int ee;                               //Enemy-exist
};
//----------------------------------------------------------------------------------------------------------------
class Bullet
{
    public:
        Bullet();                             
        void shoot();                         //Shooting the bullet
        void NOTshoot();                      //Not shooting the bullet
        void movebullets(int map_Scroll);     //Moving the mullet
        int x(){return bx;}                   //x-pos
        void setx(int n){bx=n;}               //setting x-pos
        int y(){return by;}                   //y-pos
        void sety(int n){by=n;}               //setting y-pos
        bool ready(){return be != 0;}         //Bullet-ready
        bool impact;
        //int type;                             //Bullet-type
    private:
        int bx;                               //Bullet-x-pos
        int by;                               //Bullet-y-pos
        int bs;                               //Bullet-speed
        int be;                               //Bullet-exist
};
//----------------------------------------------------------------------------------------------------------------
//Player 1+2 Structure
class jet
{
    public:
        int x;                                     //player x-position
        int y;                                     //player y-position
        int a;                                     //Blit width
        int b;                                     //Blit Height
        int c;                                     //Width
        int d;                                     //Height
        int i;                                     //laser wave effect
        int ex;                                    //explosion
        int Left;                                  //Turning left
        int Right;                                 //Turning right
        int turn;                                  //Activates Spinning animation
        int shot;                                  //When gun is shot
        int shoot;                                 //When press shooting
        int shoot_x;                               //Muzzle flare default x-position
        int shoot_y;                               //Muzzle flare default y-position
        int shotX;                                 //Muzzle flare position shift 
        int HP;                                    //player HP
        int life;                                  //Player Life
        int blit_shoot_x;                          //Muzzle flare animation sprite position
        int frame_counter;                         //jet animation counter
        int frame_counterLR;                       //Left/Right animation counter
        int frame_counterSHOT;                     //Muzzle flare counter
        int wave_counter;                          //Wavy laser
        int life_counter;
        int tgm_counter;
        void frame_reset();                        //jet animation reset
        void frame_resetLR ();                     //Left/Right animation reset
        void frame_resetSHOT();                    //Muzzle Flare reset
        void wave_reset();                         //Wavy laser reset
        void wave_frame(int num);                  //Wavy Lazer animation
        void frameLR(int num,int L, int R);        //Left/Right animation
        void frame(int num);                       //jet animation
        void frameSHOT(int num);                   //Muzzle flare animation
        void hurt(){HP --;}
        void insta_death(){HP = 0;}
        bool dead(){return HP <= 0;}               //player dead status
        bool tgm(){return tgm_counter <= 250;}
        bool FFshow;                               //Force Field
    private:
};

struct animation
{
       int x;
       int y;
};
// End of Player 2
/******************************************************************************************************************************/
int main(int argc, char *argv[]){
    
    allegro_init();
    install_keyboard();
    install_mouse(); 
    install_timer();
    set_color_depth(desktop_color_depth());
    install_sound (MIDI_AUTODETECT, MIDI_AUTODETECT , 0); //Needed to play Music
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 500, 750, 0, 0);
    LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(200));
    srand(time(NULL));
    
    jet player[2];
    animation animation[1000];
    moveable enemy[1000];
    Bullet bullet[maxbullets];
    for(int i = 0; i <= 1000; i ++)
    {
        enemy[i].NOTshoot();
    }
    //explosion
    int ex_counter[1000];
    int ex_counterP[2];
    //ForceField Shield
    int FFframe_counter[1000];
    int FFframe_counterP[2];
    //Map_scroll vertical
    float bgy[2] = {-1920,0};
    //Force Field shield counter
    //Laser variables
    int current_clip = 0;
    int canshoot = 1;
    int current_clip2 = 0;
    int canshoot2 = 1;
    //Enemy
    int current_enemy = 0;
    int conjuration_num = 0;
    int enemy_counter = 0;
    //Mapscroll
    int map_scroll;
    //Enemy Bullets
    int EN_shoot;
    int n;
    //impact length
    int impact_num;
    //trololol
    int MODE = 0;
    int number_of_players = 1;
    int origin_x[2];
    //Load le picz
    BITMAP *p1 = load_bitmap("sprites/P1.bmp", NULL);
    BITMAP *p2 = load_bitmap("sprites/P2.bmp", NULL);
    BITMAP *shoot = load_bitmap("sprites/shoot.bmp", NULL);
    BITMAP *laser = load_bitmap("sprites/laser3.bmp", NULL);
    BITMAP *pj = load_bitmap("sprites/enPJ.bmp", NULL); 
    BITMAP *explosion = load_bitmap("sprites/explosion.bmp", NULL);
    BITMAP *impact = load_bitmap("sprites/impact.bmp", NULL);
    BITMAP *bg = load_bitmap("sprites/bg.bmp", NULL);
    BITMAP *FF[3] = {NULL};
    FF[0] = load_bitmap("sprites/FF1.bmp", NULL);
    FF[1] = load_bitmap("sprites/FF2.bmp", NULL);
    FF[2] = load_bitmap("sprites/FF3.bmp", NULL);
    
    BITMAP *ex[8] = {NULL};
    ex[0] = load_bitmap("sprites/exx1.bmp",NULL);
    ex[1] = load_bitmap("sprites/exx2.bmp",NULL);
    ex[2] = load_bitmap("sprites/exx3.bmp",NULL);
    ex[3] = load_bitmap("sprites/exx4.bmp",NULL);
    ex[4] = load_bitmap("sprites/exx5.bmp",NULL);
    ex[5] = load_bitmap("sprites/exx6.bmp",NULL);
    ex[6] = load_bitmap("sprites/exx7.bmp",NULL);
    ex[7] = load_bitmap("sprites/exx8.bmp",NULL);
    
    BITMAP *enemyp[5] = {NULL};
    enemyp[0] = load_bitmap("sprites/enemy1.bmp", NULL);
    enemyp[1] = load_bitmap("sprites/enemy2.bmp", NULL);
    enemyp[2] = load_bitmap("sprites/enemy3.bmp", NULL);
    enemyp[3] = load_bitmap("sprites/enemy4.bmp", NULL);
    enemyp[4] = load_bitmap("sprites/enemy5.bmp", NULL);
    
    BITMAP *buffer = create_bitmap(500, 750);
    BITMAP *pause_background = create_bitmap(screen_w,screen_h);
    int x, y = 0;
    //Setting Allegro to text mode
    if(buffer == NULL){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not create buffer!");		
		exit(EXIT_FAILURE);
	}
	
	player[0].wave_counter = 0;
	player[1].wave_counter = 0;
    player[0].frame_counter = 0;
    player[0].frame_counterLR = 0;
    player[0].y = 650;
    player[0].x = 530; 
    player[0].c = 34;
    player[0].d = 44;
    player[0].a = 114;
    player[0].HP = 4;
    player[0].life = 4;
    player[1].life = 4;
    player[1].HP = 4;
    player[1].frame_counter = 0;
    player[1].frame_counterLR = 0;
    player[1].y = 650;
    player[1].x = 660; 
    player[1].d = 44;
    player[1].a = 114;
    player[1].c = 34;
    player[0].i = 0;
    player[1].i = 0;
    
    while(MODE != 2)
    { 
        
        if(MODE == 0)
        {
            DisplayMenu(MODE,number_of_players);
            if( number_of_players == 1)
            {
                player[1].HP = 0;
                player[1].life = 0;
            }
            else
            {
                player[1].HP = 4;
                player[1].life = 4;
            }
        }
        else if(MODE == 1)
        {
            draw_sprite(buffer, bg, 0-map_scroll, -1150+(int)bgy[0]);
            draw_sprite(buffer, bg, 0-map_scroll, -1150+(int)bgy[1]);
            if(bgy[0] > 1920)
                bgy[0] = -1920;
            if(bgy[1] > 1920)
                bgy[1] = -1920;
                
            textprintf_ex(buffer, font, 10,10, makecol(255,255,255), -1, "Player #1");
            if (player[0].life > 0)
            {
                textprintf_ex(buffer, font, 10,25, makecol(255,255,255), -1, "Life: %d", player[0].life-1);
                textprintf_ex(buffer, font, 10,35, makecol(255,255,255), -1, "HP: %d", player[0].HP);
            }
            else
                textprintf_ex(buffer, font, 10,25, makecol(255,255,255), -1, "GAME OVER");
                
            textprintf_ex(buffer, font, 370,10, makecol(255,255,255), -1, "Player #2");
            if (player[1].life > 0)
            {
            textprintf_ex(buffer, font, 370,25, makecol(255,255,255), -1, "Life: %d", player[1].life-1);
            textprintf_ex(buffer, font, 370,35, makecol(255,255,255), -1, "HP: %d", player[1].HP);
            }
            else
                textprintf_ex(buffer, font, 370,25, makecol(255,255,255), -1, "GAME OVER");
                
        		while(speed_counter > 0)
                {
                    bgy[0]+=1;
                    bgy[1]+=1;
                    player[0].Left = false;
                    player[0].Right = false;
                    player[0].shot = false;
                    player[1].Left = false;
                    player[1].Right = false;
                    player[1].shot = false;
                    //Player one controls
                    
                    origin_x[0] = player[0].x;
                    origin_x[1] = player[1].x;
                    
                    
                    if(!player[0].dead())
                    {
                        
                        if(key[KEY_D] && key[KEY_A])
                        {
                            cout << "zuqi" << endl;
                         /*   if(player[0].Left == true)
                            {
                                 player[0].turn = 1;
                            }
                            else if(player[0].Right == true)
                            {
                                 player[1].turn = 2;
                            }*/
                        }
                		else if(key[KEY_D] && !key[KEY_A] && player[0].x < map_scroll+500-34)
                        {
                			player[0].x += 2;
                		}
                		else if(key[KEY_A] && !key[KEY_D] && player[0].x > map_scroll)
                        { 
                			player[0].x -= 2;
                		}
                		if(key[KEY_W] && player[0].y > 0)
                        {
                			player[0].y -= 1.5;
                		} 
                		if(key[KEY_S] && player[0].y < 750-player[0].d)
                        {
                			player[0].y += 2;
                		}
            		    
                		if(key[KEY_U] && canshoot == 1)
                        {
                            if (!bullet[current_clip].ready())
                            {
                                  bullet[current_clip].setx(player[0].x+player[0].i);
                                  bullet[current_clip].sety(player[0].y-7);
                                  bullet[current_clip].shoot();
                                  canshoot = 0;
                                  current_clip++;
                                  player[0].shoot = true;
                            }
                        }
                        else if(!(key[KEY_U]))
                        {
                             player[0].shoot = false;
                        }
                    }
                    for(int jae = 0; jae < 2; jae++)
                    {
                        if(player[jae].x < origin_x[jae])
                        {
                            player[jae].Left = true;
                        }
                        else if(player[jae].x > origin_x[jae])
                        {
                            player[jae].Right = true;
                        }
                        else
                        {
                            if(keyrel(KEY_A))
                            {
                                 player[0].turn = 1;
                            }
                            if(keyrel(KEY_D))
                            {
                                 player[0].turn = 2;
                            }
                        }
                    }
                   //Player two controls
                    if(!player[1].dead())
                    {
                		if(key[KEY_RIGHT] && !key[KEY_LEFT] && player[1].x < map_scroll+500-34)
                        {
                    			player[1].x += 2;
                    			player[1].Right = true;
                		} 
                		else if(key[KEY_LEFT] && !key[KEY_RIGHT] && player[1].x > map_scroll)
                        {
                    			player[1].x -= 1.5;
                    			player[1].Left = true;
                		}
                		if(key[KEY_UP] && player[1].y > 0)
                        {
                			player[1].y -= 1.5;
                		} 
                		if(key[KEY_DOWN] && player[1].y < 750-player[1].d)
                        {
                			player[1].y += 2;
                		}
            
                		if(mouse_b && canshoot2 == 1)
                        {
                          if (!bullet[current_clip].ready())
                          {
                                bullet[current_clip].setx(player[1].x+player[1].i);
                                bullet[current_clip].sety(player[1].y-7);
                                bullet[current_clip].shoot();
                                canshoot2 = 0;
                                current_clip++;
                                player[1].shoot = true;
                            }
                        }
                        else if(!(mouse_b))
                        {
                            player[1].shoot = false;
                        } 
                    }
        //ZHAO HUAN enemy
        if(key[KEY_P])
        {
            blit(screen,pause_background,0,0,0,0,screen_w,screen_h);
            PauseScreen(MODE,pause_background);
        }
                    if(current_enemy+8 >= 1000)
                    {
                        current_enemy = 0;
                    }
                    if(enemy_counter == 0)
                    {
                        conjuration_num = rand()%3;
                        conjuration_num = 3;
                        if(conjuration_num == 0)
                        {
                            for(int i = 0; i < 8; i++)
                            {
                                 enemy[current_enemy].setup(rand()%750+225, -50,0,1.5,rand()%5);
                                 current_enemy++;
                            }
                        }
                        else if(conjuration_num == 1)
                        {
                             enemy[current_enemy].setup(rand()%750+225, -50,0,1.5,rand()%5);
                             current_enemy++;
                        }
                        else
                        {
                        }
                    }
                    
                    enemy_counter++;
                    if(player[0].life != 0 && player[1].life != 0)
                        map_scroll = (player[0].x+34+player[1].x)/2 - screen_w/2;
                    else if(player[0].life == 0) 
                        map_scroll = player[1].x+17-screen_w/2;
                    else if(player[1].life == 0)
                        map_scroll = player[0].x+17-screen_w/2;
                    
                    if(map_scroll < 225)
                        map_scroll = 225;
                    else if(map_scroll+screen_w >= 975)
                    {
                        map_scroll = 975 - screen_w;
                    }
        
                    //Plauyer muzzle flare
                    player[0].shoot_x = player[0].x + 8;
                    player[0].shoot_y = player[0].y - 3;
                    
                    player[1].shoot_x = player[1].x + 8;
                    player[1].shoot_y = player[1].y - 3;    
                    //Conjuration
                    if(conjuration_num == 0)
                    {
                        if(enemy_counter > 40)
                            {
                                enemy_counter = 0;
                            }
                    }
                    else if (conjuration_num == 1)
                    {
                        if(enemy_counter > 40)
                        {
                            enemy_counter = 0;
                        }
                    }
                    //Shooting
                    for (int i = 0; i < maxbullets; i++)
                    {
                        if (bullet[i].ready())
                            bullet[i].movebullets(map_scroll);
                    }
                    if(current_clip >= maxbullets)
                        current_clip = 0;
                    if (player[0].frame_counterSHOT  >4)
                    {
                       canshoot = 1;
                    }
                    if (player[1].frame_counterSHOT >4)
                    {
                       canshoot2 = 1;
                    }
                    //Countners
                    speed_counter -= 2;            
                    impact_num ++;
                    if(impact_num > 1)
                        impact_num -= 4;
                         
                    for(int i = 0; i < 1000; i++)
                    {
                        ex_counter[i]++;
                        ex_counter[i] = ex_reset(ex_counter[i]);
                        FFframe_counter[i]++;
                        FFframe_counter[i] = FFframe_reset(FFframe_counter[i]);
                    }
                    for(int j = 0; j<2; j++)
                    {
                        player[j].wave_counter ++;
                        player[j].wave_reset();
                        player[j].frame_counterLR++;
                        player[j].frame_resetLR();
                        player[j].frame_counter ++;
                        player[j].frame_reset();
                        player[j].frame_counterSHOT ++;
                        player[j].frame_resetSHOT();
                        player[j].wave_frame(player[j].wave_counter);
                        ex_counterP[j] ++;
                        ex_counterP[j] = ex_reset(ex_counterP[j]);
                        FFframe_counterP[j]++;
                        FFframe_counterP[j] = FFframe_reset(FFframe_counterP[j]);
                        player[j].life_counter ++;
                        player[j].tgm_counter ++;
                    }             
                       
                }
                player[0].frameLR(player[0].frame_counterLR,KEY_A, KEY_D);
                player[1].frameLR(player[1].frame_counterLR, KEY_LEFT, KEY_RIGHT);
                player[0].frame(player[0].frame_counter);
                player[1].frame(player[1].frame_counter);
                player[0].frameSHOT(player[0].frame_counterSHOT);
                player[1].frameSHOT(player[1].frame_counterSHOT);
                        
                //Player one
                if (!player[0].dead())
                    masked_stretch_blit(p1, buffer,player[0].a,player[0].b,player[0].c, player[0].d, player[0].x - map_scroll, player[0].y, player[0].c, 43);
                if (player[0].shoot == true)
                    masked_blit(shoot, buffer, player[0].blit_shoot_x, 0, player[0].shoot_x  - map_scroll - player[0].shotX, player[0].shoot_y, 23 ,17);
                //Player two
                if (!player[1].dead())
                    masked_stretch_blit(p2, buffer,player[1].a,player[1].b,player[1].c, player[1].d, player[1].x  - map_scroll, player[1].y, player[1].c,43);
                if (player[1].shoot == true)
                    masked_blit(shoot, buffer, player[1].blit_shoot_x, 0, player[1].shoot_x  - map_scroll - player[1].shotX, player[1].shoot_y, 23 ,17);
        
                for (int i = 0; i < maxbullets; i++)
                {
                    if (bullet[i].ready())
                    {   
                        draw_sprite(buffer, laser, bullet[i].x()-map_scroll,bullet[i].y());
                    }
                }
                
        //WARNING-----INEFFICIENT CODING BELOW-----------------------------------------------------------------------------------------------------------
                for(int i = 0; i < 1000; i++)
                {
                    //Enemy Presets        
                    if(!enemy[i].dead())
                    {
                        if (enemy[i].type == 4) 
                            enemy[i].y_speed-=0.00225;
                        if (enemy[i].type == 3)
                        {
                            enemy[i].x_speed+=0.001;
                            enemy[i].y_speed=0.75;
                        }
                        if (enemy[i].type == 2)
                        {
                            enemy[i].x_speed-=0.001;
                            enemy[i].y_speed=0.75;
                        }
                        if (enemy[i].type == 1)
                        {
                            enemy[i].x_speed = 0.1;
                            enemy[i].y_speed -= 0.00225;
                        }
                        if (enemy[i].type == 0)
                        {
                            enemy[i].x_speed = -0.1;
                            enemy[i].y_speed -= 0.00225;
                        }
                        enemy[i].updatepos();
                        draw_sprite(buffer, enemyp[enemy[i].type], enemy[i].x() - map_scroll, enemy[i].y());
                        //Bullet Colision
                        for (int j = 0; j < maxbullets; j++)
                        {
                            if (bullet[j].ready())
                            {
                                if(collide(bullet[j].x()-map_scroll, bullet[j].y(), 16, 20, enemy[i].x()-map_scroll, enemy[i].y(), enemyp[enemy[i].type]->w,enemyp[enemy[i].type]->h))
                                 {
                                     bullet[j].NOTshoot();
                                     enemy[i].hurt();                    
                                     enemy[i].FFshow = true;
                                     bullet[j].impact = true;
                                     FFframe_counter[i] = 0;
                                     if(enemy[i].dead() && enemy[i].ex  == false)
                                     {
                                         enemy[i].ex = true;
                                         ex_counter[i] = 0;
                                     }
                                 }
                             }
                        }
                        //Enemy shield
                        if (enemy[i].FFshow == true)
                        {
                           if (FFframe_counter[i] < 5)
                               masked_stretch_blit(FF[0], buffer,0,0,53,49,enemy[i].x()-map_scroll-12, enemy[i].y()-2,70,70);
                           if (FFframe_counter[i] >= 5 && FFframe_counter[i] < 10)
                               masked_stretch_blit(FF[1], buffer,0,0,53,49,enemy[i].x()-map_scroll-12, enemy[i].y()-2,70,70);
                           if (FFframe_counter[i] >= 10)
                               masked_stretch_blit(FF[2], buffer,0,0,53,49,enemy[i].x()-map_scroll-12, enemy[i].y()-2,70,70);
                           if (FFframe_counter[i] > 14)
                              enemy[i].FFshow = false;
                        }
                    }
                }
                
                //Bullet Impact
                for(int j = 0; j < 1000; j++)
                {
                    if(bullet[j].impact == true)
                    {
                         if(impact_num < 1)
                             draw_sprite(buffer,impact,bullet[j].x()-map_scroll-37+8, bullet[j].y());
                         if(impact_num >= 1)
                             bullet[j].impact = false;
                     }
                 }
                 
                //enemy explosion
                
                for(int i = 0; i < 1000; i++)
                {
                    if(enemy[i].ex == true)
                    {
                         if(ex_counter[i] < 6)
                             draw_sprite(buffer, ex[0],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 6 && ex_counter[i] <12)
                             draw_sprite(buffer, ex[1],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 12 && ex_counter[i] < 18)
                             draw_sprite(buffer, ex[2],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 18 && ex_counter[i] < 24)
                             draw_sprite(buffer, ex[3],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 24 && ex_counter[i] < 30)
                             draw_sprite(buffer, ex[4],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 30 && ex_counter[i] < 36)
                             draw_sprite(buffer, ex[5],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 36 && ex_counter[i] < 42)
                             draw_sprite(buffer, ex[6],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] >= 42 && ex_counter[i] < 48)
                             draw_sprite(buffer, ex[7],enemy[i].x()-map_scroll-17, enemy[i].y()+32-23);
                         if(ex_counter[i] > 47)
                             enemy[i].ex = false;
                             
                    }
                } 
                //Enemy Gun
                for(int i = 0; i < 1000; i++)
                {
                    EN_shoot = rand()%100;
                    if(EN_shoot == 0)
                    {
                        if(enemy[i].y()>=20)
                        {
                            if(!enemy[i].dead())
                            {
                                if (!enemy[i].ready())
                                {
                                    n = rand()%2;
                                    //Calculaions for enemy bullets
                                    enemy[i].pj_x = enemy[i].x()+(enemyp[enemy[i].type]->w)/2-7;
                                    enemy[i].pj_y = enemy[i].y()+(enemyp[enemy[i].type]->h);
                                    enemy[i].testxspeed = ((player[n].x+player[n].c/2) - (enemy[i].x()+(enemyp[enemy[i].type]->w)/2-7));
                                    enemy[i].testyspeed = ((player[n].y+player[n].d/2) - (enemy[i].y()+(enemyp[enemy[i].type]->h)));
                                    
                                    if(enemy[i].testxspeed>0)
                                        enemy[i].pjx_speed = ENb_speed/sqrt((1+pow(enemy[i].testyspeed,2)/pow(enemy[i].testxspeed,2)));
                                    else
                                        enemy[i].pjx_speed = -ENb_speed/sqrt((1+pow(enemy[i].testyspeed,2)/pow(enemy[i].testxspeed,2)));
                                        
                                    enemy[i].pjy_speed = enemy[i].pjx_speed*enemy[i].testyspeed/enemy[i].testxspeed;
                                    enemy[i].shoot();
                                }
                            }
                        }
                    }
                }
                for(int i = 0; i < 2; i ++)
                {
                    if(!player[i].dead())
                    {        
                        if (player[i].FFshow == true)
                        {
                           if (FFframe_counterP[i] < 5)
                               masked_stretch_blit(FF[0], buffer,0,0,53,49,player[i].x-map_scroll+player[i].c/2-35, player[i].y+player[i].d/2-35,70,70);
                           if (FFframe_counterP[i] >= 5 && FFframe_counter[i] < 10)
                               masked_stretch_blit(FF[1], buffer,0,0,53,49,player[i].x-map_scroll+player[i].c/2-35, player[i].y+player[i].d/2-35,70,70);
                           if (FFframe_counterP[i] >= 10)
                               masked_stretch_blit(FF[2], buffer,0,0,53,49,player[i].x-map_scroll+player[i].c/2-35, player[i].y+player[i].d/2-35,70,70);
                           if (FFframe_counterP[i] > 14)
                              player[i].FFshow = false;
                        }
                    }
                }
                //Enemy bullet SHOOTING
                for(int i = 0; i < 1000; i++)
                {
                    if (enemy[i].ready())
                    {
                        enemy[i].pj_x += enemy[i].pjx_speed;
                        enemy[i].pj_y += enemy[i].pjy_speed;
                        draw_sprite(buffer, pj, enemy[i].pj_x - map_scroll, enemy[i].pj_y);
                        if (enemy[i].pj_x > 1100+50 || enemy[i].pj_x < -50 || enemy[i].pj_y > 1920+50 || enemy[i].pj_y < -50)
                           enemy[i].NOTshoot();
                        for (int j = 0; j<2;j++)
                        {
                            if(!player[j].dead())
                            {
                                if(collide(player[j].x-map_scroll, player[j].y, player[j].c, player[j].d,enemy[i].pj_x-map_scroll, enemy[i].pj_y, pj->w,pj->h) && !player[j].tgm())
                                {
                                    enemy[i].NOTshoot();
                                    FFframe_counterP[j] = 0;
                                    player[j].hurt();
                                    player[j].FFshow = true;
                                    if(player[j].dead())
                                    {
                                        player[j].ex = true;
                                        player[j].life --;
                                        ex_counterP[j] = 0;
                                        player[j].life_counter = 0;
                                    }
                                }
                            }
                        }
                    }
                }
                
                //Player Collision
                for(int i = 0; i < 1000; i++)
                {
                    if(!enemy[i].dead())
                    {
                        for(int j = 0; j<2;j++)
                        {
                            if(!player[j].dead())
                            {
                                if(collide(player[j].x - map_scroll, player[j].y, player[j].c, player[j].d, enemy[i].x()-map_scroll, enemy[i].y(), enemyp[enemy[i].type]->w,enemyp[enemy[i].type]->h) && !player[j].tgm())
                                {   
                                    enemy[i].insta_death();
                                    enemy[i].ex = true;
                                    ex_counter[i] = 0;
                                    player[j].insta_death();
                                    if (player[j].dead())   
                                    {
                                        player[j].ex = true;
                                        player[j].life --;
                                        ex_counterP[j] = 0;
                                        player[j].life_counter = 0;
                                    }
                                }
                            }
                        }
                    }
                }
                
                for(int i = 0; i < 2;i++)
                {
                    if(player[i].ex == true)
                    {
                        if(ex_counterP[i] < 6)
                             draw_sprite(buffer, ex[0],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 6 && ex_counterP[i] <12)
                             draw_sprite(buffer, ex[1],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 12 && ex_counterP[i] < 18)
                             draw_sprite(buffer, ex[2],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 18 && ex_counterP[i] < 24)
                             draw_sprite(buffer, ex[3],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 24 && ex_counterP[i] < 30)
                             draw_sprite(buffer, ex[4],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 30 && ex_counterP[i] < 36)
                             draw_sprite(buffer, ex[5],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 36 && ex_counterP[i] < 42)
                             draw_sprite(buffer, ex[6],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] >= 42 && ex_counterP[i] < 48)
                             draw_sprite(buffer, ex[7],player[i].x-map_scroll+player[i].c/2-32, player[i].y+player[i].d/2-32);
                         if(ex_counterP[i] > 47)
                             player[i].ex = false;
                    }
                }
                for(int i = 0; i < 2; i ++)
                {
                    if(player[i].life > 0 && player[i].dead())
                    {
                        if(player[0].life > 0 && player[0].dead())
                            masked_stretch_blit(p1, buffer,player[0].a,player[0].b,player[0].c, player[0].d, player[0].x - map_scroll, 1300-player[0].life_counter*2, player[0].c, 43);
                        if(player[1].life > 0 && player[1].dead())
                            masked_stretch_blit(p2, buffer,player[1].a,player[1].b,player[1].c, player[1].d, player[1].x - map_scroll, 1300-player[1].life_counter*2, player[1].c, 43);
                        if (1300-player[i].life_counter*2 <= 650)
                        {
                            player[i].HP = 3;
                            player[i].y = 650;
                            player[i].tgm_counter = 0;
                        }
                    }   
                }
                blit(buffer, screen, 0, 0, 0, 0, screen_w, screen_h);
                clear_bitmap(buffer);
            }
  
    }			
	destroy_bitmap(buffer);

    return(0);
}END_OF_MAIN();

/***********************************************************************************************/
//Functions
//Menu
void DisplayMenu(int &Num,int &Num_Player)
{
     SAMPLE *MenuSong = load_wav("music/Coven - Wake You Up.wav"); //the title music
     static BITMAP *buffer2 = create_bitmap(screen_w,screen_h); //the buffer
     static BITMAP *Scroll = load_bitmap("sprites/Star Background.bmp", NULL);
     static BITMAP *jetani = load_bitmap("sprites/Menu_ani.bmp", NULL);
     static BITMAP *Menu[3];
     Menu[0]= load_bitmap("sprites/Menu.bmp", NULL);
     Menu[1]= load_bitmap("sprites/Menu 2.bmp", NULL);
     Menu[2]= load_bitmap("sprites/Menu 3.bmp", NULL);
     static BITMAP *Select = load_bitmap("sprites/cursor.bmp", NULL);
     //BITMAP *Instruction_Pic = load_bitmap("Instruction Menu.bmp", NULL);    
     int map_scroll=0; //background map scroll
     int mode = 0;
     int select = 0;
     int keyhold = FALSE;
     int Loop = 0;
     int ani_counter = 0;
     pos cursor;
     float background_pos[2] = {-2250,-750};
     play_sample(MenuSong, 255, 128, 1000, 1); // plays the song when menu starts
     while(Loop==0){
         while(speed_counter > 0)
         {       
             if(key[KEY_UP] && keyhold == FALSE)
             {
                  select--;   
                  keyhold = TRUE;
             } 
             else if(key[KEY_DOWN] && keyhold == FALSE)
             {
                  
                  select++;
                  keyhold = TRUE;
             }
             if(select < 0)
             {
                 select = 2;
             }
             else if(select > 2)
             {
                 select = 0;
             }
             if(key[KEY_ENTER] && keyhold == FALSE)
             {
                 if(mode == 0)
                 {
                     if(select == 0)
                     {
                         mode = 1;
                     }
                     else if(select == 1)
                     {
                         mode = 2;
                     }
                     else if(select == 2)
                     {
                         Num = 2;
                         Loop = 1;                          
                     }
                 }                               
                 else if(mode == 1)
                 {
                     if(select == 2)
                     {
                         mode = 0;
                         select = 0;
                     }
                     else
                     {
                         if(select == 0)
                         {
                             Num_Player = 1;
                         }
                         else if(select == 1)
                         {
                             Num_Player = 2;
                         }
                         Num = 1;
                         Loop = 1;
                     }
                 }
                 else if(mode == 2)
                 {
                      mode = 0;
                 }

                 keyhold = TRUE;
             }
             
             if(!key[KEY_ENTER] && !key[KEY_DOWN] && !key[KEY_UP])
             {
                 keyhold = FALSE;
             }
             if(mode == 0)
             {
                 if(select == 0)
                 {
                     cursor.x = 241;
                     cursor.y = 433;
                 }
                 else if(select == 1)
                 {
                     cursor.x = 241;
                     cursor.y = 518;
                 }
                 else if(select == 2)
                 {
                     cursor.x = 241;
                     cursor.y = 610;
                 }
             }
             else if(mode == 1)
             {
                 if(select == 0)
                 {
                     cursor.x = 234;
                     cursor.y = 500;
                 }
                 else if(select == 1)
                 {
                     cursor.x = 234;
                     cursor.y = 558;
                 }
                 else if(select == 2)
                 {
                     cursor.x = 234;
                     cursor.y = 626;
                 }
             }
             else
             {
                 cursor.x = 234;
                 cursor.y = 595;
             }
             
             background_pos[0]+=0.1;
             background_pos[1]+=0.1;
             if(background_pos[0] > 1500)
             {
                 background_pos[0] = -1500;  
             }
             if(background_pos[1] > 1500)
             {
                 background_pos[1] = -1500;  
             }    
             speed_counter--;        
             ani_counter++; 
             if(ani_counter >16)
                 ani_counter = 0;
         }                 
         blit(Scroll, buffer2, 0, 0, 0, (int)background_pos[0], 500,1500);
         blit(Scroll, buffer2, 0, 0, 0, (int)background_pos[1], 500,1500);
         masked_blit(Menu[mode], buffer2, 0, 0, 0, 0, screen_w ,screen_h);
         masked_blit(Select, buffer2, 0, 0, cursor.x, cursor.y, 61 ,43);
         if(ani_counter >= 8)
             draw_sprite(buffer2, jetani,12,289);
         blit(buffer2,screen,0,0,0,0,screen_w,screen_h);
         clear_bitmap(buffer2);
     }
     destroy_sample(MenuSong);
}

void PauseScreen(int &Num,BITMAP* background)
{
     static BITMAP *buffer2 = create_bitmap(screen_w,screen_h); //the buffer
     static BITMAP *Menu[2];
     Menu[0]= load_bitmap("sprites/pause.bmp", NULL);
     Menu[1]= load_bitmap("sprites/pause2.bmp", NULL);
     static BITMAP *Select = load_bitmap("sprites/cursor.bmp", NULL);
     int mode = 0;
     int select = 0;
     int keyhold = FALSE;
     int Loop = 0;
     pos cursor;
     while(Loop==0){
         while(speed_counter > 0)
         {
             if(key[KEY_UP] && keyhold == FALSE)
             {
                  select--;
                  keyhold = TRUE;
             }
             else if(key[KEY_DOWN] && keyhold == FALSE)
             {

                  select++;
                  keyhold = TRUE;
             }
             if(select < 0)
             {
                 select = 3;
             }
             else if(select > 3)
             {
                 select = 0;
             }
             
             if(key[KEY_ENTER] && keyhold == FALSE)
             {
                 if(mode == 0)
                 {
                     if(select == 0)
                     {
                         Loop = 1;
                     }
                     else if(select == 1)
                     {
                         mode = 1;
                     }
                     else if(select == 2)
                     {
                         Num = 0;
                         Loop = 1;
                     }
                     else if(select == 3)
                     {
                         Num = 2;
                         Loop = 1;
                     }
                 }
                 else if(mode == 1)
                 {
                     mode = 0;
                 }
                 keyhold = TRUE;
             }

             if(!key[KEY_ENTER] && !key[KEY_DOWN] && !key[KEY_UP])
             {
                 keyhold = FALSE;
             }
             if(mode == 0)
             {
                 if(select == 0)
                 {
                     cursor.x = 125;
                     cursor.y = 270;
                 }
                 else if(select == 1)
                 {
                     cursor.x = 125;
                     cursor.y = 333;
                 }
                 else if(select == 2)
                 {
                     cursor.x = 125;
                     cursor.y = 384;
                 }
                 else if(select == 3)
                 {
                     cursor.x = 125;
                     cursor.y = 444;
                 }
             }
             else
             {
                     cursor.x = 268;
                     cursor.y = 472;
             }

             speed_counter--;
         }
         blit(background, buffer2, 0,0,0,0,screen_w,screen_h);
         masked_blit(Menu[mode], buffer2, 0, 0, 0, 0, screen_w ,screen_h);
         masked_blit(Select, buffer2, 0, 0, cursor.x, cursor.y, 61 ,43);
         blit(buffer2,screen,0,0,0,0,screen_w,screen_h);
         clear_bitmap(buffer2);
     }
}


//Collision
int collide(float p1_bb_left,float p1_bb_top,float p1_width, float p1_height, float p2_bb_left,float p2_bb_top, float p2_width, float p2_height){
    float p1_bb_right = (p1_bb_left + p1_width);
    float p1_bb_bottom = (p1_bb_top + p1_height);
    
    float p2_bb_right = (p2_bb_left + p2_width);
    float p2_bb_bottom = (p2_bb_top + p2_height); 
    
    if(p1_bb_bottom < p2_bb_top){
    	return false;
    }
    else if(p1_bb_top > p2_bb_bottom){
    	return false;
    }
    else if(p1_bb_right < p2_bb_left){
    	return false;
    }
    else if(p1_bb_left > p2_bb_right){
    	return false;
    }
    return true;
}
//Jets
void jet::frame_resetSHOT()
{
     if (frame_counterSHOT > 5)
         frame_counterSHOT = 0;
}

void jet::frameSHOT(int num)
{
     if(shoot == true){
         if(num<=2)
         {
            if(a == 0)
            {
                blit_shoot_x = 0;
                shotX= 8;
            }
            else if(a == 18)
            {
                blit_shoot_x = 24;
                shotX= 5;
            }
            else if(a == 37)
            {
                blit_shoot_x = 48;
                shotX= 5;
            }
            else if(a == 59)
            {
                blit_shoot_x = 72;
                shotX= 5;
            }
            else if(a == 85)
            {
                blit_shoot_x = 129;
                shotX= 5;
            }
            else if(a == 114)
            {
                blit_shoot_x = 118;
                shotX= 0;
            }
            else if(a == 148)
            {
                blit_shoot_x = 136;
                shotX= 5;
            }
            else if(a == 177)
            {
                blit_shoot_x = 160;
                shotX= 5;
            }
            else if(a == 203)
            {
                blit_shoot_x = 190;
                shotX= 2;
            }
            else if(a == 225)
            {
                blit_shoot_x = 212;
                shotX= 5;
            }
            else if(a == 244)
            {
                blit_shoot_x = 235;
                shotX= 8;
            }
            else
                shotX = 0;
         }
         else
             shoot = false;
     }
}
void jet::frame(int num)
{
    if(num < 3)
    	b = 0;
    else if(num >= 3 && num < 6)
    	b = 43;
    else if(num >= 6 && num < 9)
    	b = 86;
    else if(num >= 9 && num < 12)
         b = 129;
}
void jet::frame_reset()
{
    if(frame_counter > 12)
        frame_counter = 0;
}
void jet::frame_resetLR()
{
    if (Left != true && Right != true)
    {
        if (frame_counterLR > 25)
            frame_counterLR = 0;
    }
}
void jet::frameLR(int num, int L, int R)
{  
    if(Left == true)
    {
        if(num < 5)
        {
            a = 85;
            c = 29;
        }
        else if(num >= 5 && num < 10)
        {
            a = 59;
            c = 26;
        }
        else if(num >= 10 && num < 15)
        {
            a = 37;
            c = 22;
        }
        else if(num >= 15 && num < 20)
        {
             a = 18;
             c = 19;
        }
        else if (num < 25)
        {
             a = 0;
             c = 18;
        }
    }

    if(a != 144&& Left == false && Right == false && turn == 1)
    {
        if(num >= 0 && num < 5)
        {
             a = 18;
             c = 19;
        }
        else if(num >= 5 && num < 10)
        {
            a = 37;
            c = 22;
        }
        else if(num >= 10 && num < 15)
        {
            a = 59;
            c = 26;
        }
        else if(num <= 15 && num < 20)
        {
            a = 85;
            c = 29;
        }	
        else if(num < 25)
        {
            a = 114;
            c = 34;
            turn = 0;
        }
    }
    if(Right == true)
    {
        if(num < 5)
        {
            a = 148;
            c = 29;
        }
        else if(num >= 5 && num < 10)
        {
            a = 177;
            c = 26;
        }
        else if(num >= 10 && num < 15)
        {
            a = 203;
            c = 22;
        }
        else if(num >= 15 && num < 20)
        {
             a = 225;
             c = 19;
        }
        else if (num < 25)
        {
             a = 244;
             c = 18;
        }
    }
    if (keyrel(R))
       turn = 2;
    if(a != 144 && Right == false && Left == false && turn == 2)
    {
        if(num >= 0 && num < 5)
        {
             a = 225;
             c = 19;
        }
        else if(num >= 5 && num < 10)
        {
            a = 203;
            c = 22;
        }
        else if(num >= 10 && num < 15)
        {
            a = 177;
            c = 26;
        }
        else if(num <= 15 && num < 20)
        {
            a = 148;
            c = 29;
        }
        else if(num < 25)
        {     
            a = 114;
            c = 34;
            turn = 0;
        }
    }        
}

void jet::wave_reset()
{
    if (wave_counter > 17)
       wave_counter = 0;
}
void jet::wave_frame(int num)
{
    
    //Big laser
    
    switch(num)
    {
        case 0:
        case 9: i = 9;
                break;
        case 1:
        case 8: i = 6;
                break;
        case 2:
        case 7: i = 3;
                break;
        case 3:
        case 6: i = 0;
                break;
        case 4:
        case 5: i = -3;
                break;
        case 10:
        case 17: i = 12;
                break;
        case 11:
        case 16:i = 15;
                break;
        case 12:
        case 15:i = 18;
                break;
        case 13:
        case 14:i = 21;
                break;
    }
    
    //Medium Laser
    /*
    switch(num)
    {
        case 0:
        case 7: i = 9;
                break;
        case 1:
        case 6: i = 6;
                break;
        case 2:
        case 5: i = 3;
                break;
        case 3:
        case 4: i = 0;
                break;
        case 8:
        case 13: i = 12;
                break;
        case 9:
        case 12:i = 15;
                break;
        case 10:
        case 11: i = 18;
                break;
    }
    */
    //Small Laser
    /*
    switch(num)
    {
        case 0:
        case 5: i = 9;
                break;
        case 1:
        case 4: i = 6;
                break;
        case 2:
        case 3: i = 3;
                break;
        case 6:
        case 9: i = 12;
                break;
        case 7:
        case 8:i = 15;
                break;
    }
    */
    
    //Default
    i = 9;
}
//Force Field
int FFframe_reset(int num)
{
    if (num > 15)
       num = 0;
    return num;
}
//explosion
int ex_reset(int num)
{
    if (num > 48)
       num = 0;
    return num;
}
//Bullet Functions

Bullet::Bullet(){
    bx = 0;
    by = 0;
    
    bs = 10;
    be = 0;
}

void Bullet::NOTshoot(){
    be = 0;
}

void Bullet::shoot(){
    be = 1;
}

void Bullet::movebullets(int num){
     by -= bs;          
     if (bx > screen_w + num)
         be = 0;
     if(by + 20 < 0)
         be = 0;          
} 

//Enemy functions

void moveable::setup(int x,int y,float xs,float ys,int n)
{
    HP = 4;
    global_x = x;
    global_y = y;
    x_speed = xs;
    y_speed = ys;
    type = n;
}

//Detect Keyrelease

int keyrel(int k)
{
    //static int initialized = 0;
    static int keyp[KEY_MAX] = {0};
 
    /*if(!initialized)
    {
        int i;
        for(i = 0; i < KEY_MAX; i++) keyp[i] = 0;
        initialized = 1;
    }*/
 
    if(key[k] && !keyp[k])
    {
        keyp[k] = 1;
        return false;
    }
    else if(!key[k] && keyp[k])
    {
        keyp[k] = 0;
        return true;
    }
    return false;
}
