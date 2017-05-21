#include<allegro.h>
#include<stdio.h>
#include<iostream>
#include<math.h>
#define maxbullets 1000 
#define PURPLE makecol(255,0,255)
const float ENb_speed = 1.5;
const int screen_h = 750;
const int screen_w = 500;
const int level_length = 1800;
using namespace std;

//Timers
volatile long speed_counter = 0;
void increment_speed_counter(){
	speed_counter++;
}END_OF_FUNCTION(increment_speed_counter);
//Menus
void DisplayMenu(int &Num,int &Num_Player);
void PauseScreen(int &Num,BITMAP* background);
void CloseScreen(BITMAP *background);
void OpenScreen(BITMAP* background);
void UpgradeScreen(int number_of_players,int damage[],int speed[],int life[], int bomb[], int shield[], BITMAP *background);
//EXPLOSION FRAME RESET
int ex_reset(int num);
//Force Field frame reset
int FFframe_reset(int num);
//Collsion
int collide(float p1_bb_left,float p1_bb_top,float p1_width, float p1_height, float p2_bb_left,float p2_bb_top, float p2_width, float p2_height);

int bulletcollide(float p1_bb_left,float p1_bb_top,float p1_width, float p1_height, float p2_bb_left,float p2_bb_top, float p2_width, float p2_height, BITMAP *enemy);
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
        void setup(int,int,int,float,float,int);
        void updatepos(){global_x+= x_speed; global_y+=y_speed;}
        float x(){return global_x;}
        float y(){return global_y;}
        bool ready(){return ee != 0;}
        bool dead(){return HP <= 0;}
        void kill(){if(global_y > screen_h || global_y < -500)HP = 0;}
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
        int HP;
        float global_x;
        float global_y;
        int shoot_counter;
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
        int bomb_num;
        int tgm_counter;
        void frame_reset();                        //jet animation reset
        void frame_resetLR ();                     //Left/Right animation reset
        void frame_resetSHOT();                    //Muzzle Flare reset
        void wave_reset();                         //Wavy laser reset
        void wave_frame(int num, int damage);                  //Wavy Lazer animation
        void frameLR(int num,int L, int R);        //Left/Right animation
        void frame(int num);                       //jet animation
        void frameSHOT(int num);                   //Muzzle flare animation
        void hurt(){HP --;}
        void insta_death(){HP = 0;}
        bool dead(){return HP <= 0;}               //player dead status
        bool tgm(){return tgm_counter <= 250;}
        bool FFshow;                               //Force Field
        float speed;
    private:
};

struct animation
{
       int x;
       int y;
       bool show;
       int counter;
       int type;
       int counter_length;
       void initialize();
       void setup(int x_pos, int y_pos,int ani_type);
       int bombw;
       int bombh;
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
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));
    srand(time(NULL));
    
    jet player[2];
    moveable enemy[1000];
    Bullet bullet[2][maxbullets];
    animation blowup[1000];
    
    for(int i = 0; i <= 1000; i ++)
    {
        enemy[i].NOTshoot();
    }
    //ani
    int animation_counter = 0;
    //ForceField Shield
    int FFframe_counter[1000];
    int FFframe_counterP[2];
    //Map_scroll vertical
    float bgy[2] = {-1919,0};
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
    int conjuration_counter = 0;
    //Mapscroll
    int map_scroll;
    //Enemy Bullets
    int EN_shoot;
    int n;
    //impact length
    int impact_num;
    //
    int bool_frame = 0;
    //trololol
    int MODE = 0;
    int number_of_players = 1;
    int epos;
    //more trololol
    int damage[2] = {0};
    int dex[2] = {0};
    int life[2] = {0};
    int boom[2] = {3};
    int shield[2] = {0};
    bool musicstart = true;
    //highscore
    int highscore1;
    int highscore2;
    FILE *fptr;
    
    srand(time(NULL));
    //Load le picz
    MIDI *sound;
    BITMAP *p1 = load_bitmap("sprites/P1.bmp", NULL);
    BITMAP *p2 = load_bitmap("sprites/P2.bmp", NULL);
    BITMAP *shoot = load_bitmap("sprites/shoot.bmp", NULL);
    BITMAP *laser[4];
    laser[0] = load_bitmap("sprites/laser.bmp", NULL);
    laser[1] = load_bitmap("sprites/laser2.bmp", NULL);
    laser[2] = load_bitmap("sprites/laser3.bmp", NULL);
    laser[3] = load_bitmap("sprites/laser4.bmp", NULL);
    BITMAP *pj = load_bitmap("sprites/enPJ.bmp", NULL); 
    BITMAP *explosion = load_bitmap("sprites/explosion.bmp", NULL);
    BITMAP *impact = load_bitmap("sprites/impact.bmp", NULL);
    BITMAP *bg = load_bitmap("sprites/bg.bmp", NULL);
    BITMAP *bg2 = load_bitmap("sprites/bg2.bmp", NULL);
    BITMAP *level_completed = load_bitmap("sprites/level complete.bmp", NULL);
    BITMAP *game_over = load_bitmap("sprites/gameover.bmp", NULL);
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
    
    
    BITMAP *bomb = load_bitmap("sprites/Bomb.bmp", NULL);
    BITMAP *enemyp[8] = {NULL};
    BITMAP *Pause_screen = create_bitmap(screen_w,screen_h);
    enemyp[0] = load_bitmap("sprites/enemy1.bmp", NULL);
    enemyp[1] = load_bitmap("sprites/enemy2.bmp", NULL);
    enemyp[2] = load_bitmap("sprites/enemy3.bmp", NULL);
    enemyp[3] = load_bitmap("sprites/enemy4.bmp", NULL);
    enemyp[4] = load_bitmap("sprites/enemy5.bmp", NULL);
    enemyp[5] = load_bitmap("sprites/drone2.bmp", NULL);
    enemyp[6] = load_bitmap("sprites/drone1.bmp", NULL);
    enemyp[7] = load_bitmap("sprites/boss.bmp", NULL);
    enemyp[8] = load_bitmap("sprites/boss2.bmp", NULL);
    BITMAP *buffer = create_bitmap(500, 750);
   	int level = 0;
   	int level_complete = FALSE;
   	int level_counter = 0;
   	int conjur_boss = TRUE;
    int boss_alive = FALSE;
	int speed = 2;
    int x, y = 0;
    int initialize = FALSE;
    int initialize_counter = 0;
    int keyhold[2] = {FALSE};
    bool spawn_minion;
    int bosshurt_counter;
    bool bosshurt = false;
    
    //Setting Allegro to text mode
    if(buffer == NULL){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not create buffer!");		
		exit(EXIT_FAILURE);
	}
    while(MODE != 2)
    {
        /*if(musicstart == true)
        {
            sound = load_midi("music/Onestop.mid");
            play_midi(sound,1);
            musicstart = false;
        }*/
        if(MODE == 0)
        {
            DisplayMenu(MODE,number_of_players);
            if( number_of_players == 1)
            {
                damage[0] = 0;
                dex[0] = 0;
                life[0] = 4;
                boom[0] = 3;
                shield[0] = 5;
                damage[1] = 0;
                dex[1] = 0;
                life[1] = 0;
                boom[1] = 0;
                shield[1] = 0;
            }
            else
            {
                damage[0] = 0;
                dex[0] = 0;
                life[0] = 4;
                boom[0] = 3;
                shield[0] = 5;
                damage[1] = 0;
                dex[1] = 0;
                life[1] = 4;
                boom[1] = 3;
                shield[1] = 5;
            }
            initialize= TRUE;
           	level = 0;
          
        }
        else if(MODE == 1)
        {
            if(conjur_boss == FALSE && boss_alive == FALSE)
            {
                level ++;
                initialize_counter = 0;
                initialize = TRUE;
                blit(screen,Pause_screen,0,0,0,0,screen_w,screen_h);
                UpgradeScreen(number_of_players,damage,dex,life,boom,shield,Pause_screen);
            }
            if(initialize_counter == 0 && initialize == TRUE)
            {
                for (int i=0;i<1000;i++)
                {
                    enemy[i].insta_death();
                    blowup[i].initialize();
                    enemy[i].NOTshoot();
                    bullet[0][i].NOTshoot();
                    bullet[1][i].NOTshoot();
                    enemy[i].ex = false;
                }
                animation_counter = 0;
             	level_complete = FALSE;
            	level_counter = 0;
   	            conjur_boss = TRUE;
                boss_alive = FALSE;
                spawn_minion = false;
            	player[0].wave_counter = 0;
            	player[1].wave_counter = 0;
            	player[0].FFshow = false;
            	player[1].FFshow = false;
            	FFframe_counterP[0] = 0;
            	FFframe_counterP[1] = 0;
            	player[0].Left = false;
            	player[1].Left = false;
            	player[0].Right = false;
            	player[1].Right = false;
                player[0].frame_counter = 0;
                player[0].frame_counterLR = 24;
                player[0].y = 650;
                player[0].x = 530;
                player[0].c = 34;
                player[0].d = 44;
                player[0].a = 114;
                player[0].tgm_counter = 900;
                player[1].tgm_counter = 900;
                player[1].frame_counter = 0;
                player[1].frame_counterLR = 24;
                player[1].y = 650;
                player[1].x = 660;
                player[1].d = 44;
                player[1].a = 114;
                player[1].c = 34;
                player[0].i = 0;
                player[1].i = 0;
                for(int i = 0; i <2;i++)
                {
                    player[i].HP = shield[i];
                    player[i].bomb_num = boom[i];
                }
                initialize_counter++;
                blit(screen,Pause_screen,0,0,0,0,screen_w,screen_h);
                CloseScreen(Pause_screen);
            }
            if(initialize_counter >30)
            {
                initialize_counter = 0;
                initialize = FALSE;
                OpenScreen(Pause_screen);
            }
            draw_sprite(buffer, bg, 0-map_scroll, -1150+(int)bgy[0]);
            draw_sprite(buffer, bg2, 0-map_scroll, -1150+(int)bgy[1]);
            if(bgy[0] > 1919)
                bgy[0] = -1919;
            if(bgy[1] > 1919)
                bgy[1] = -1919;
            
            for(int i = 0; i <2;i++)
            {
                player[i].speed = speed + dex[i];
                player[i].life = life[i];
            }
            textprintf_ex(buffer, font, 215,10, makecol(255,255,255), -1, "Wave: %d", level+1);
            textprintf_ex(buffer, font, 10,10, makecol(255,255,255), -1, "Player #1");
            if (player[0].life > 0)
            {
                textprintf_ex(buffer, font, 10,25, makecol(255,255,255), -1, "Life: %d", player[0].life-1);
                textprintf_ex(buffer, font, 10,35, makecol(255,255,255), -1, "HP: %d", player[0].HP);
                textprintf_ex(buffer, font, 10,45, makecol(255,255,255), -1, "Bomb: %d", player[0].bomb_num);
            }
            else
                textprintf_ex(buffer, font, 10,25, makecol(255,255,255), -1, "GAME OVER");
                
            textprintf_ex(buffer, font, 390,10, makecol(255,255,255), -1, "Player #2");
            if (player[1].life > 0)
            {
                textprintf_ex(buffer, font, 390,25, makecol(255,255,255), -1, "Life: %d", player[1].life-1);
                textprintf_ex(buffer, font, 390,35, makecol(255,255,255), -1, "HP: %d", player[1].HP);
                textprintf_ex(buffer, font, 390,45, makecol(255,255,255), -1, "Bomb: %d", player[1].bomb_num);
            }
            else
                textprintf_ex(buffer, font, 390,25, makecol(255,255,255), -1, "GAME OVER");
                
        		while(speed_counter > 0)
                {
                     
                    if(level_counter < level_length)
                    {
                        level_counter++;
                    }
                    if(initialize == TRUE)
                    {
                        initialize_counter++;
                    }
                    if(key[KEY_P])
                    {
                        blit(screen,Pause_screen,0,0,0,0,screen_w,screen_h);
                        PauseScreen(MODE,Pause_screen);
                    }
                    
                    bgy[0]+=1;
                    bgy[1]+=1;
                    player[0].Left = false;
                    player[0].Right = false;
                    player[0].shot = false;
                    player[1].Left = false;
                    player[1].Right = false;
                    player[1].shot = false;
                    //Player one controls
                    if(!player[0].dead())
                    {
                        if(key[KEY_D] && key[KEY_A])
                        {
                        }
                		else if(key[KEY_D] && player[0].x < map_scroll+500-player[0].c)
                        {
                			player[0].x += player[0].speed;
                			player[0].Right = true;
                		} 
                		else if(key[KEY_A] && player[0].x > map_scroll)
                        { 
                			player[0].x -= player[0].speed;
                			player[0].Left = true;
                		}
                		if(key[KEY_W] && player[0].y > 0)
                        {
                			player[0].y -= player[0].speed;
                		} 
                		if(key[KEY_S] && player[0].y < 750-player[0].d)
                        {
                			player[0].y += player[0].speed;
                		}
            		    
                		if(key[KEY_U] && canshoot == 1)
                        {
                            if (!bullet[0][current_clip].ready())
                            {
                                  bullet[0][current_clip].setx(player[0].x+player[0].i);
                                  bullet[0][current_clip].sety(player[0].y-7);
                                  bullet[0][current_clip].shoot();
                                  canshoot = 0;
                                  if (damage[0]>=2)
                                      canshoot = 1;
                                  current_clip++;
                                  player[0].shoot = true;
                            }
                        }
                        else if(!(key[KEY_U]))
                        {
                             player[0].shoot = false;
                        }
                        if (key[KEY_I] && keyhold[0] != TRUE)
                        {
                            keyhold[0] = TRUE;
                            if (player[0].bomb_num > 0)
                            {
                                player[0].bomb_num --;
                                blowup[animation_counter].setup(player[0].x+player[0].c/2,player[0].y+player[0].d/2,3);
                                animation_counter++;
                            }
                        }
                        if(!key[KEY_I])
                        {
                            keyhold[0] = FALSE;
                        }
                    }
                   //Player two controls
                    if(!player[1].dead())
                    {
                        if(key[KEY_RIGHT] && key[KEY_LEFT])
                        {
                        }
                		else if(key[KEY_RIGHT] && player[1].x < map_scroll+500-player[1].c)
                        {
                    			player[1].x += player[1].speed;
                    			player[1].Right = true;
                		} 
                		else if(key[KEY_LEFT] && player[1].x > map_scroll)
                        {
                    			player[1].x -= player[1].speed;
                    			player[1].Left = true;
                		}
                		if(key[KEY_UP] && player[1].y > 0)
                        {
                			player[1].y -= player[1].speed;
                		} 
                		if(key[KEY_DOWN] && player[1].y < 750-player[1].d)
                        {
                			player[1].y += player[1].speed;
                		}
            
                		if(mouse_b & 1 && canshoot2 == 1)
                        {
                          if (!bullet[1][current_clip].ready())
                          {
                                bullet[1][current_clip].setx(player[1].x+player[1].i);
                                bullet[1][current_clip].sety(player[1].y-7);
                                bullet[1][current_clip].shoot();
                                canshoot2 = 0;
                                if (damage[1]>=2)
                                      canshoot2 = 1;
                                current_clip++;
                                player[1].shoot = true;
                            }
                        }
                        else if(!(mouse_b & 1))
                        {
                            player[1].shoot = false;
                        } 
                        
                        if (mouse_b & 2 && keyhold[1] != TRUE)
                        {
                            keyhold[1] = TRUE;
                            if (player[1].bomb_num > 0)
                            {
                                player[1].bomb_num --;
                                blowup[animation_counter].setup(player[1].x+player[1].c/2,player[1].y+player[1].d/2,3);
                                animation_counter++;
                            }
                        }
                        if(!(mouse_b & 2))
                        {
                            keyhold[1] = FALSE;
                        }
                    }
                    //ZHAO HUAN enemy
                    if(current_enemy+15 >= 1000)
                    {
                        current_enemy = 0;
                    }
                   
                    if(enemy_counter == 0)
                    {
                        conjuration_counter++;
                        if(conjuration_num == 5)
                        {
                            if(conjuration_counter > 12)
                            {
                                conjuration_num = rand()%6;
                                conjuration_counter = 0;
                            }
                        }
                        else if(conjuration_num != 5)
                            conjuration_num = rand()%6;
                            
                        if(spawn_minion == true)
                            conjuration_num = 6;
                        if(level_counter >= level_length && spawn_minion == false)
                            conjuration_num = 100;
                        if(conjuration_num == 0)
                        {
                            epos = rand()%700+200;
                            if (epos <= 450)
                            {
                                for(int i = 0; i < 4; i++)
                                {
                                     enemy[current_enemy].setup(4+level*3,epos+i*50, -50-i*50,0,1.5,5);
                                     current_enemy++;
                                }
                            }
                            else
                            {
                                for(int i = 0; i < 4; i++)
                                {
                                     enemy[current_enemy].setup(4+level*3,epos-i*50, -50-i*50,0,1.5,5);
                                     current_enemy++;
                                }
                            }
                            
                        }
                        else if(conjuration_num == 1)
                        {
                            int random_num = rand()%4+4;
                            for(int i = 0; i < random_num; i++)
                            {
                                 enemy[current_enemy].setup(4+level*3,rand()%750+225, -50,0,1.5,rand()%6);
                                 current_enemy++;
                            }
                        }
                        else if(conjuration_num == 2)
                        {
                             epos = rand()%650+250;
                            for(int i = 0; i < 4; i++)
                            {
                                 enemy[current_enemy].setup(4+level*3,epos+i*50,-50,0,1.5,4);
                                 current_enemy++;
                            }
                        }
                        else if(conjuration_num == 3)
                        {
                             epos = rand()%650+250;
                            for(int i = 0; i < 4; i++)
                            {
                                 enemy[current_enemy].setup(4+level*3,epos+i*50, -50,0,1.5,rand()%6);
                                 current_enemy++;
                            }
                        }
                        else if(conjuration_num == 4)
                        {
                            epos = rand()%500+200;
                            enemy[current_enemy].setup(2+level*3,epos, -50,0,1.5,5);
                            current_enemy++;
                            for(int i = 1; i < 4; i++)
                            {
                                 enemy[current_enemy].setup(2+level*3,epos-50*i, -50-i*60,0,1.5,5);
                                 enemy[current_enemy+1].setup(2+level*3,epos+50*i, -50-i*60,0,1.5,5);
                                 current_enemy+=2;
                            }
                        }
                        else if(conjuration_num == 5)
                        {
                              enemy[current_enemy].setup(4+level*3,rand()%750+225, -50,0,1.5,rand()%6);
                              current_enemy++;
                        }
                        else if(conjuration_num == 6)
                        {
                            for(int i = 0; i < 6; i++)
                            {
                                 enemy[current_enemy].setup(4+level*3,rand()%750+225, -50,0,1.5,6);
                                 current_enemy++;
                            }
                        }
                        else
                        {
                        }    
                    }
                    else if(level_counter >= level_length && conjur_boss == TRUE)
                    {
                         for(int i = 0;i<1000;i++)
                         {
                             if(!enemy[i].dead())
                             {
                                 enemy[i].insta_death();
                                 blowup[animation_counter].setup(enemy[i].x(),enemy[i].y(),0);
                                 animation_counter++;
                             }
                         }
                         conjur_boss = FALSE;
                         boss_alive = TRUE;
                         spawn_minion = true;
                            enemy[current_enemy].setup(300+level*50,rand()%500, -499,0,1.5,7);
                              current_enemy++;
                         
                    }
                    enemy_counter++;
                    
                    if(player[0].life != 0 && player[1].life != 0)
                    {
                        if(player[0].x < player[1].x)
                            map_scroll = (player[0].x+player[1].c+player[1].x)/2 - screen_w/2;
                        else
                             map_scroll = (player[0].x+player[0].c+player[1].x)/2 - screen_w/2;
                    }
                    
                    else if(player[0].life == 0) 
                        map_scroll = player[1].x+player[1].c/2-screen_w/2;
                    else if(player[1].life == 0)
                        map_scroll = player[0].x+player[0].c/2-screen_w/2;
                    
                    
                    if(map_scroll < 225)
                        map_scroll = 225;
                    else if(map_scroll+screen_w >= 975)
                    {
                        map_scroll = 975 - screen_w;
                    }
                    
                    if(player[0].x+player[0].c > map_scroll+screen_w)
                    {
                          player[0].x=map_scroll+screen_w-player[0].c;
                    }
                    if(player[1].x+player[1].c > map_scroll+screen_w)
                    {
                          player[1].x=map_scroll+screen_w-player[1].c;
                    }
                    //Plauyer muzzle flare
                    player[0].shoot_x = player[0].x + 8;
                    player[0].shoot_y = player[0].y - 3;
                    
                    player[1].shoot_x = player[1].x + 8;
                    player[1].shoot_y = player[1].y - 3;    
                    //Conjuration
                    int difficulty = level*15;
                    int difficulty2 = level*25;
                    if (difficulty >= 70)
                       difficulty = 69;
                    if (difficulty2 >= 200)
                       difficulty2 = 199;
                    if (conjuration_num == 5)
                    {
                        if(enemy_counter > 80-difficulty)
                        {
                            enemy_counter = 0;
                        }
                    }
                    else
                    {
                        if(enemy_counter > 210-difficulty2)
                        {
                            enemy_counter = 0;
                        }
                    }    
                    //Shooting
                    for (int i = 0; i < maxbullets; i++)
                    {
                        for(int j = 0;j<2;j++)
                        {
                            if (bullet[j][i].ready())
                                bullet[j][i].movebullets(map_scroll);
                        }
                    }
                    if(current_clip >= maxbullets)
                        current_clip = 0;
                    if (player[0].frame_counterSHOT  >2)
                    {
                       canshoot = 1;
                    }
                    if (player[1].frame_counterSHOT >2)
                    {
                       canshoot2 = 1;
                    }
                    if(animation_counter > 900)
                       animation_counter = 0;
                    //Countners
                    speed_counter--;
                    bosshurt_counter++;
                    bool_frame++;
                    if(bool_frame>6)
                    {
                        bool_frame = 0;
                    }
                    
                    impact_num ++;
                    if(impact_num > 1)
                        impact_num -= 4;
                         
                    for(int i = 0; i < 1000; i++)
                    {
                        blowup[i].counter++;
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
                        player[j].life_counter ++;
                        player[j].tgm_counter ++;
                        FFframe_counterP[j]++;
                    }             
                }
                player[0].frameLR(player[0].frame_counterLR,KEY_A, KEY_D);
                player[1].frameLR(player[1].frame_counterLR, KEY_LEFT, KEY_RIGHT);
                for(int j = 0; j<2;j++)
                {
                    player[j].frame(player[j].frame_counter);
                    player[j].frameSHOT(player[j].frame_counterSHOT);
                    player[j].wave_frame(player[j].wave_counter,damage[j]);
                }

                //Player one
                if (!player[0].dead())
                {
                    if(player[0].tgm() && bool_frame/3 == 1)
                    {
                    }
                    else
                        masked_stretch_blit(p1, buffer,player[0].a,player[0].b,player[0].c, player[0].d, player[0].x - map_scroll, player[0].y, player[0].c, 43);
                }
                if (player[0].shoot == true)
                    masked_blit(shoot, buffer, player[0].blit_shoot_x, 0, player[0].shoot_x  - map_scroll - player[0].shotX, player[0].shoot_y, 23 ,17);
                //Player two
                if (!player[1].dead())
                {
                    if(player[1].tgm() && bool_frame/3 == 1)
                    {
                    }
                    else
                        masked_stretch_blit(p2, buffer,player[1].a,player[1].b,player[1].c, player[1].d, player[1].x  - map_scroll, player[1].y, player[1].c,43);
                }
                if (player[1].shoot == true)
                    masked_blit(shoot, buffer, player[1].blit_shoot_x, 0, player[1].shoot_x  - map_scroll - player[1].shotX, player[1].shoot_y, 23 ,17);
        
                for (int i = 0; i < maxbullets; i++)
                {
                    for(int j = 0; j<2; j++)
                    {
                        if (bullet[j][i].ready())
                        {   
                            if(damage[j] == 0)
                                draw_sprite(buffer, laser[0], bullet[j][i].x()-map_scroll,bullet[j][i].y());
                            else if(damage[j] == 1)
                                draw_sprite(buffer, laser[1], bullet[j][i].x()-map_scroll,bullet[j][i].y());
                            else if(damage[j] == 2)
                                draw_sprite(buffer, laser[2], bullet[j][i].x()-map_scroll,bullet[j][i].y());
                            else if(damage[j] == 3)
                                draw_sprite(buffer, laser[3], bullet[j][i].x()-map_scroll,bullet[j][i].y());
                        }
                    }
                }

        //WARNING-----INEFFICIENT CODING BELOW-----------------------------------------------------------------------------------------------------------
                for(int i = 0; i < 1000; i++)
                {
                    //Bullet Impact
                    for(int j = 0;j<2;j++)
                    {
                        if(bullet[j][i].impact == true)
                        {
                             if(impact_num < 1)
                                 draw_sprite(buffer,impact,bullet[j][i].x()-map_scroll-37+8, bullet[j][i].y());
                             if(impact_num >= 1)
                                 bullet[j][i].impact = false;
                        }
                    }
                    //Animations
                    if(blowup[i].show == true)
                    {
                         if(blowup[i].type == 0)
                         {
                             if(blowup[i].counter < 4)
                                 draw_sprite(buffer, ex[0],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 4 && blowup[i].counter <8)
                                 draw_sprite(buffer, ex[1],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 8 && blowup[i].counter < 12)
                                 draw_sprite(buffer, ex[2],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 12 && blowup[i].counter < 16)
                                 draw_sprite(buffer, ex[3],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 16 && blowup[i].counter < 20)
                                 draw_sprite(buffer, ex[4],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 20 && blowup[i].counter < 24)
                                 draw_sprite(buffer, ex[5],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 24 && blowup[i].counter < 28)
                                 draw_sprite(buffer, ex[6],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter >= 28 && blowup[i].counter < 32)
                                 draw_sprite(buffer, ex[7],blowup[i].x-map_scroll-17, blowup[i].y+32-23);
                             if(blowup[i].counter > 31)
                                 blowup[i].show = false;
                         }
                         else if(blowup[i]. type == 1)
                         {
                             draw_sprite(buffer, enemyp[7],blowup[i].x-map_scroll, blowup[i].y);
                             if(blowup[i].counter == 2 || blowup[i].counter == 33 || blowup[i].counter == 64 || blowup[i].counter ==  95)
                             {
                                 for(int j = 0; j < 5; j++)
                                 {      
                                     blowup[animation_counter].setup(blowup[i].x+rand()%600,blowup[i].y+rand()%400,0);
                                     animation_counter++;
                                 }
                             }
                             
                             if(blowup[i].counter > 130)
                             {
                                 blowup[i].show = false;
                                 blowup[animation_counter].setup(0,0,2);
                                 animation_counter++;
                             }
                         }
                         else if(blowup[i].type == 2)
                         {
                             
                             if(blowup[i].counter > 120)
                             {
                                 blowup[i].show = false;
                                 boss_alive = false;
                             }
                             else
                                 draw_sprite(buffer, level_completed,0,0);
                         }
                         else if(blowup[i].type == 3)
                         {
                             blowup[i].bombw+=blowup[i].counter;
                             blowup[i].bombh+=blowup[i].counter;
                             switch(blowup[i].counter)
                             {
                                 case 5:  map_scroll+=2;
                                          break;
                                 case 40: map_scroll -=2;
                                          break;
                                 case 10:
                                 case 20:
                                 case 30: map_scroll -=4;
                                          break;
                                 case 15:
                                 case 25:
                                 case 35: map_scroll +=4;
                                          break;
                              }
                             if(blowup[i].counter > 40)
                             {
                                 blowup[i].show = false;
                                 blowup[i].bombh = bomb->h;
                                 blowup[i].bombw = bomb->w;
                                 for(int i = 0;i<1000;i++)
                                 {
                                     enemy[i].NOTshoot();        
                                     if(!enemy[i].dead())
                                     {
                                         if(enemy[i].type !=7)
                                         {
                                             enemy[i].insta_death();
                                             blowup[animation_counter].setup(enemy[i].x(),enemy[i].y(),0);
                                             animation_counter++;
                                         }
                                     }
                                 }
                             }
                             masked_stretch_blit(bomb, buffer,0,0,bomb->w,bomb->h,blowup[i].x-map_scroll-blowup[i].bombw/2, blowup[i].y-blowup[i].bombh/2, blowup[i].bombw, blowup[i].bombh);
                         }
                         else if(blowup[i].type == 4)
                         {
                             if(blowup[i].counter > 480)
                                 MODE = 0;
                             else
                             {
                                 draw_sprite(buffer, game_over,0,0);
                                 textprintf_ex(buffer, font, 204,372, makecol(255,255,255), -1, "%d waves", level+1);
                                 if(number_of_players == 1)
                                     textprintf_ex(buffer, font, 204,584, makecol(255,255,255), -1, "%d waves", highscore1);
                                 else if(number_of_players == 2)
                                     textprintf_ex(buffer, font, 204,584, makecol(255,255,255), -1, "%d waves", highscore2);
                             }
                         }
                     }
                    //enemy gun
                    EN_shoot = rand()%500;
                    if(EN_shoot == 0)
                    {
                        if(enemy[i].y()>=20)
                        {
                            if(!enemy[i].dead())
                            {
                                if (!enemy[i].ready())
                                {
                                    n = rand()%2;
                                    if (player[0].dead())
                                        n = 1;
                                    else if(player[1].dead())
                                        n = 0;
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
                    if (enemy[i].pj_x > 1100+50 || enemy[i].pj_x < -50 || enemy[i].pj_y > 750+50 || enemy[i].pj_y < -50)
                        enemy[i].NOTshoot();
                    if (enemy[i].x()>1100+300 || enemy[i].x() < -300 || enemy[i].y() > 750+50 || enemy[i].y() < -500)
                        enemy[i].insta_death();
                    //enemy shooting
                    if (enemy[i].ready())
                    {
                        enemy[i].pj_x += enemy[i].pjx_speed;
                        enemy[i].pj_y += enemy[i].pjy_speed;
                        draw_sprite(buffer, pj, enemy[i].pj_x - map_scroll, enemy[i].pj_y);
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
                                        blowup[animation_counter].setup(player[j].x,player[j].y,0);
                                        animation_counter++;
                                        life[j]--;
                                        player[j].life_counter = 0;
                                    }
                                }
                            }
                        }
                    }
                    if(!enemy[i].dead())
                    {
                        //enemy initialization
                        if (enemy[i].type == 7)
                        {
                            if(enemy[i].y() >=20 && enemy[i].y_speed!=0)
                            {
                                enemy[i].y_speed=0;
                                 enemy[i].x_speed=-0.15;
                                 
                            }
                            else if(enemy[i].y() <20)
                                enemy[i].y_speed=0.25;
                            if(enemy[i].y_speed == 0)
                            {
                                if (enemy[i].x() <= 20)
                                    enemy[i].x_speed=0.15;
                                else if (enemy[i].x() >= 550)
                                    enemy[i].x_speed=-0.15;
                            }
                        }
                        else if(enemy[i].type == 6)
                        {
                            int LR;
                            LR = rand()%2;     
                            if(LR == 1)
                               LR = -3;
                            else
                               LR = 3;     
                            enemy[i].x_speed = LR;
                            enemy[i].y_speed = 0.5;
                        }
                        else if (enemy[i].type == 5)
                        {
                            enemy[i].x_speed = 0;
                            enemy[i].y_speed = 0.75;
                        }
                        else if (enemy[i].type == 4)
                            enemy[i].y_speed-=0.00325;
                        else if (enemy[i].type == 3)
                        {
                            enemy[i].x_speed+=0.001;
                            enemy[i].y_speed=0.75;
                        }
                        else if (enemy[i].type == 2)
                        {
                            enemy[i].x_speed-=0.001;
                            enemy[i].y_speed=0.75;
                        }
                        else if (enemy[i].type == 1)
                        {
                            enemy[i].x_speed = 0.2;
                            enemy[i].y_speed -= 0.00325;
                        }
                        else if (enemy[i].type == 0)
                        {
                            enemy[i].x_speed = -0.2;
                            enemy[i].y_speed -= 0.00325;
                        }
                        enemy[i].updatepos();
                        if(enemy[i].type != 7)
                            draw_sprite(buffer, enemyp[enemy[i].type], enemy[i].x() - map_scroll, enemy[i].y());
                        else
                        {
                            if(bosshurt == false)
                                draw_sprite(buffer, enemyp[enemy[i].type], enemy[i].x() - map_scroll, enemy[i].y());
                            if(bosshurt == true)
                            {
                                if(bosshurt_counter < 1)
                                    draw_sprite(buffer, enemyp[8], enemy[i].x() - map_scroll, enemy[i].y());
                                else
                                    bosshurt = false;
                            }
                        }
                        
                        for (int j = 0; j < maxbullets; j++)
                        {
                            for(int k = 0;k<2;k++)
                            {
                                if (bullet[k][j].ready())
                                {
                                    if(bulletcollide(bullet[k][j].x()-map_scroll, bullet[k][j].y(), 16, 20, enemy[i].x()-map_scroll, enemy[i].y(), enemyp[enemy[i].type]->w,enemyp[enemy[i].type]->h,enemyp[enemy[i].type]))
                                     {
                                         bullet[k][j].NOTshoot();
                                         enemy[i].hurt();
                                         if(damage[k] == 1)
                                             enemy[i].hurt();
                                             
                                         bullet[k][j].impact = true;
                                         if(enemy[i].type == 7)
                                         {
                                             bullet[k][j].impact = false;
                                             bosshurt = true;
                                             bosshurt_counter = -2;
                                         }
                                             
                                         if(enemy[i].dead())
                                         {
                                             if(enemy[i].type == 7)
                                             {
                                                 blowup[animation_counter].setup(enemy[i].x(),enemy[i].y(),1);
                                                 animation_counter++;
                                                 spawn_minion = false;
                                                 for(int i = 0;i<1000;i++)
                                                 {
                                                     if(!enemy[i].dead())
                                                     {
                                                         enemy[i].insta_death();
                                                         blowup[animation_counter].setup(enemy[i].x(),enemy[i].y(),0);
                                                         animation_counter++;
                                                     }
                                                 }
                                             }
                                             else
                                                 blowup[animation_counter].setup(enemy[i].x(),enemy[i].y(),0);
                                                 animation_counter++;
                                         }
                                     }
                                 }
                             }
                        }
                        
                        for(int j = 0; j<2;j++)
                        {
                            if(!player[j].dead())
                            {
                                //Player Collision
                                if(enemy[i].type == 7)
                                {
                                }
                                else
                                {
                                    if(collide(player[j].x - map_scroll, player[j].y, player[j].c, player[j].d, enemy[i].x()-map_scroll, enemy[i].y(), enemyp[enemy[i].type]->w,enemyp[enemy[i].type]->h) && !player[j].tgm())
                                    {
                                        enemy[i].insta_death();
                                        player[j].insta_death();
                                        blowup[animation_counter].setup(enemy[i].x(),enemy[i].y(),0);
                                        animation_counter ++;
                                        if (player[j].dead())
                                        {
                                            blowup[animation_counter].setup(player[j].x,player[j].y,0);
                                            life[j] --;
                                            player[j].life_counter = 0;
                                            animation_counter ++;
                                        }
                                    }
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
                           if (FFframe_counterP[i] < 3)
                               masked_stretch_blit(FF[0], buffer,0,0,53,49,player[i].x-map_scroll+player[i].c/2-35, player[i].y+player[i].d/2-35,70,70);
                           if (FFframe_counterP[i] >= 3 && FFframe_counterP[i] < 6)
                               masked_stretch_blit(FF[1], buffer,0,0,53,49,player[i].x-map_scroll+player[i].c/2-35, player[i].y+player[i].d/2-35,70,70);
                           if (FFframe_counterP[i] >= 6)
                               masked_stretch_blit(FF[2], buffer,0,0,53,49,player[i].x-map_scroll+player[i].c/2-35, player[i].y+player[i].d/2-35,70,70);
                           if (FFframe_counterP[i] > 8)
                              player[i].FFshow = false;
                        }
                    }
                    if(player[i].life > 0 && player[i].dead())
                    {
                        if(player[0].life > 0 && player[0].dead())
                            masked_stretch_blit(p1, buffer,player[0].a,player[0].b,player[0].c, player[0].d, player[0].x - map_scroll, 1100-player[0].life_counter*2, player[0].c, 43);
                        if(player[1].life > 0 && player[1].dead())
                            masked_stretch_blit(p2, buffer,player[1].a,player[1].b,player[1].c, player[1].d, player[1].x - map_scroll, 1100-player[1].life_counter*2, player[1].c, 43);
                        if (1100-player[i].life_counter*2 <= 650)
                        {
                            player[i].HP = shield[i];
                            player[i].y = 650;
                            player[i].tgm_counter = 0;
                            boom[i]=3;
                        }
                    }                       
                }
                
                if(player[0].life==0 && player[1].life == 0 )
                {
                    if (number_of_players == 1)
                    {
                        fptr = fopen("highscore1P.txt", "r");
                        fscanf(fptr, "%d", &highscore1);
                        fclose(fptr);
                        if (level+1 > highscore1)
                        {
                            highscore1 = level+1;
                            fptr = fopen("highscore1P.txt", "w");
                            fprintf(fptr, "%d", highscore1);
                            fclose(fptr);
                        }
                    }
                    else if(number_of_players == 2)
                    {
                        fptr = fopen("highscore2P.txt", "r");
                        fscanf(fptr, "%d", &highscore2);
                        fclose(fptr);
                        if (level+1 > highscore2)
                        {
                            highscore2 = level+1;
                            fptr = fopen("highscore2P.txt", "w");
                            fprintf(fptr, "%d", highscore2);
                            fclose(fptr);
                        }
                    }
                    blowup[animation_counter].setup(0,0,4);
                    animation_counter++;
                }
                
                if(initialize != TRUE)
                    blit(buffer, screen, 0, 0, 0, 0, screen_w, screen_h);
                else
                    blit(buffer,Pause_screen,0,0,0,0,screen_w,screen_h);
                clear_bitmap(buffer);
            }
    }			
    //destroy_midi(sound);
	destroy_bitmap(buffer);

    return(0);
}END_OF_MAIN();

/***********************************************************************************************/
//Functions
//Menu
void DisplayMenu(int &Num,int &Num_Player)
{
     SAMPLE *MenuSong = load_wav("music/Coven - Wake You Up.wav"); //the title music
     BITMAP *buffer2 = create_bitmap(screen_w,screen_h); //the buffer
     BITMAP *Scroll = load_bitmap("sprites/Star Background.bmp", NULL);
     BITMAP *Scroll2 = load_bitmap("sprites/Star Background2.bmp", NULL);
     BITMAP *jetani = load_bitmap("sprites/Menu_ani.bmp", NULL);
     BITMAP *Menu[3];
     Menu[0]= load_bitmap("sprites/Menu.bmp", NULL);
     Menu[1]= load_bitmap("sprites/Menu 2.bmp", NULL);
     Menu[2]= load_bitmap("sprites/Menu 3.bmp", NULL);
     BITMAP *Select = load_bitmap("sprites/cursor.bmp", NULL);
     BITMAP *Instruction_Pic = load_bitmap("Instruction Menu.bmp", NULL);    
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
             if(key[KEY_UP] && keyhold == FALSE|| key[KEY_W] && keyhold == FALSE)
             {
                  select--;   
                  keyhold = TRUE;
             } 
             else if(key[KEY_DOWN] && keyhold == FALSE|| key[KEY_S] && keyhold == FALSE)
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
                     if(select == 1)
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
             
             if(!key[KEY_ENTER] && !key[KEY_DOWN] && !key[KEY_UP] && !key[KEY_W] && !key[KEY_S])
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
             
             background_pos[0]+=1;
             background_pos[1]+=1;
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
             if(ani_counter >4)
                 ani_counter = 0;
         }                 
         blit(Scroll, buffer2, 0, 0, 0, (int)background_pos[0], 500,1500);
         blit(Scroll2, buffer2, 0, 0, 0, (int)background_pos[1], 500,1500);
         masked_blit(Menu[mode], buffer2, 0, 0, 0, 0, screen_w ,screen_h);
         masked_blit(Select, buffer2, 0, 0, cursor.x, cursor.y, 61 ,43);
         if(ani_counter >= 2)
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
             if(key[KEY_UP] && keyhold == FALSE|| key[KEY_W] && keyhold == FALSE)
             {
                  select--;
                  keyhold = TRUE;
             }
             else if(key[KEY_DOWN] && keyhold == FALSE|| key[KEY_S] && keyhold == FALSE)
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

             if(!key[KEY_ENTER] && !key[KEY_DOWN] && !key[KEY_UP] && !key[KEY_W] && !key[KEY_S])
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
void CloseScreen(BITMAP *background)
{
     static BITMAP *buffer2 = create_bitmap(screen_w,screen_h); //the buffer
     static BITMAP *Gate[2];
     Gate[0]= load_bitmap("sprites/door_left.bmp", NULL);
     Gate[1]= load_bitmap("sprites/door_right.bmp", NULL);
     int Loop = 0;
     float x[2] = {-250,501};
     while(Loop==0){
         while(speed_counter > 0)
         {
             if(x[0] >= 0)
             {
                 Loop = 1;
             }
             x[0]+=3;
             x[1]-=3;
             speed_counter--;
         }
         blit(background, buffer2, 0,0,0,0,screen_w,screen_h);
         masked_blit(Gate[0], buffer2, 0, 0, x[0], 0, 250 ,screen_h);
         masked_blit(Gate[1], buffer2, 0, 0, x[1], 0, 250 ,screen_h);
         blit(buffer2,screen,0,0,0,0,screen_w,screen_h);
         clear_bitmap(buffer2);
     }
}

void OpenScreen(BITMAP* background)
{
     static BITMAP *buffer2 = create_bitmap(screen_w,screen_h); //the buffer
     static BITMAP *Gate[2];
     Gate[0]= load_bitmap("sprites/door_left.bmp", NULL);
     Gate[1]= load_bitmap("sprites/door_right.bmp", NULL);
     int Loop = 0;
     float x[2] = {0,251};
     while(Loop==0){
         while(speed_counter > 0)
         {
             if(x[1] > 500)
             {
                 Loop = 1;
             }
             x[0]-=3;
             x[1]+=3;
             speed_counter--;
         }
         blit(background, buffer2, 0,0,0,0,screen_w,screen_h);
         masked_blit(Gate[0], buffer2, 0, 0, x[0], 0, 250 ,screen_h);
         masked_blit(Gate[1], buffer2, 0, 0, x[1], 0, 250 ,screen_h);
         blit(buffer2,screen,0,0,0,0,screen_w,screen_h);
         clear_bitmap(buffer2);
     }
}

void UpgradeScreen(int number_of_players,int damage[],int speed[],int life[], int bomb[], int shield[], BITMAP *background)
{
     static BITMAP *buffer2 = create_bitmap(screen_w,screen_h); //the buffer
     static BITMAP *Menu[2];
     Menu[0]= load_bitmap("sprites/upgrade.bmp", NULL);
     Menu[1]= load_bitmap("sprites/upgrade2.bmp", NULL);
     static BITMAP *Select = load_bitmap("sprites/cursor2.bmp", NULL);
     int mode[2] = {0};
     int select[2] ;
     int keyhold[2] = {FALSE};
     bool press[2] = {FALSE};
     bool ready[2];
     int Loop = 0;
     int i;
     pos cursor[2];
     if (number_of_players != 1)
     {
         ready[0] = false;
         ready[1] = false;
     }
     else
     {
         ready[0] = false;
         ready[1] = true;
     }
     while(Loop==0){
         while(speed_counter > 0)
         {
             if(key[KEY_W] && keyhold[0] == FALSE)
             {
                  select[0]--;
                  keyhold[0] = TRUE;
             }
             else if(key[KEY_S] && keyhold[0] == FALSE)
             {

                  select[0]++;
                  keyhold[0] = TRUE;
             }
             if(key[KEY_SPACE] && keyhold[0] == FALSE)
             {
                 press[0] = true;
                 keyhold[0] = TRUE;
             }
             if(number_of_players != 1)
             {
                 if(key[KEY_UP] && keyhold[1] == FALSE)
                 {
                      select[1]--;
                      keyhold[1] = TRUE;
                 }
                 else if(key[KEY_DOWN] && keyhold[1] == FALSE)
                 {
                      select[1]++;
                      keyhold[1] = TRUE;
                 }
                 if(key[KEY_ENTER] && keyhold[1] == FALSE)
                 {
                     press[1] = true;
                     keyhold[1] = TRUE;
                 }
             }
             if(!key[KEY_ENTER] && !key[KEY_DOWN] && !key[KEY_UP])
             {
                 keyhold[1] = FALSE;
             }
             if(!key[KEY_SPACE] && !key[KEY_S] && !key[KEY_W])
             {
                 keyhold[0] = FALSE;
             }
             for(int i = 0; i<2;i++)
             {
                 if(mode[i] == 0)
                 {
                     if(select[i] < 0)
                     {
                         select[i] = 1;
                     }
                     else if(select[i] > 1)
                     {
                         select[i] = 0;
                     }
                 }
                 else if(mode[i] == 1)
                 {
                     if(select[i] < 0)
                     {
                         select[i] = 2;
                     }
                     else if(select[i] > 2) 
                     {
                         select[i] = 0;
                     }
                 }
                 else if(mode[i] == 2)
                      select[i] = 0;
                 if(press[i] == true)
                 {
                     press[i]=false;
                     if(mode[i] == 0)
                     { 
                         if(select[i] == 0 && damage[i] <= 2)
                         {
                             mode[i]=1;
                             damage[i]++;
                         }
                         else if(select[i] == 1 && speed[i] <= 4)
                         {
                             mode[i]=1;
                             speed[i]++;
                         }
                     }
                     else if(mode[i] == 1)
                     {
                         if(select[i] == 0)
                         {
                             mode[i]=2;
                             life[i]++;
                         }
                         else if(select[i] == 1)
                         {
                             mode[i]=2;
                             bomb[i]+=2;
                         }
                         else if(select[i] == 2)
                         {
                              mode[i]=2;
                              shield[i]+=2;
                         }
                     }
                     else if(mode[i] == 2)
                     {
                          ready[i] = true;
                     }
                 }
                                  
                 if(mode[i] == 0)
                 {
                     if(select[i] == 0)
                     {
                         cursor[i].x = 100;
                         cursor[i].y = 125 + i*375;
                     }
                     else if(select[i] == 1)
                     {
                         cursor[i].x = 115;
                         cursor[i].y = 185 + i*375;
                     }
                 }
                 if(mode[i] == 1)
                 {
                     if(select[i] == 0)
                     {
                         cursor[i].x = 267;
                         cursor[i].y = 113 + i*375; 
                     }
                     else if(select[i] == 1)
                     {
                         cursor[i].x = 267;
                         cursor[i].y = 154 + i*375;
                     }
                     else if(select[i] == 2)
                     {
                         cursor[i].x = 267;
                         cursor[i].y = 199 + i*375;
                     }
                 }
                 else if(mode[i] == 2)
                 {
                         cursor[i].x = 183;
                         cursor[i].y = 262 + i*375;
                 }
             }
             if(ready[0] == true && ready[1] == true)
             {
                 Loop = 1;
             }
             speed_counter--;
         }
         blit(background, buffer2, 0,0,0,0,screen_w,screen_h);
         for(int i = 0; i<2;i++)
         {
             if (ready[i]!= true)
             {
                 masked_blit(Menu[i], buffer2,0,0,0,i*375,screen_w,screen_h/2);
                 masked_blit(Select, buffer2, 0, 0, cursor[i].x, cursor[i].y, 29 ,48);
                 textprintf_ex(buffer2, font, 224,153 + i*375, makecol(255,255,255), -1, "%d", damage[i]);
                 textprintf_ex(buffer2, font, 224,204 + i*375, makecol(255,255,255), -1, "%d", speed[i]);
                 textprintf_ex(buffer2, font, 388,138 + i*375, makecol(255,255,255), -1, "%d", life[i]-1);
                 textprintf_ex(buffer2, font, 388,172 + i*375, makecol(255,255,255), -1, "%d", bomb[i]);
                 textprintf_ex(buffer2, font, 388,224 + i*375, makecol(255,255,255), -1, "%d", shield[i]);
             }
         }
         blit(buffer2,screen,0,0,0,0,screen_w,screen_h);
         clear_bitmap(buffer2);
     }
}

int bulletcollide(float p1_bb_left,float p1_bb_top,float p1_width, float p1_height, float p2_bb_left,float p2_bb_top, float p2_width, float p2_height, BITMAP *enemy){
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
    if(getpixel(enemy,p1_bb_left - p2_bb_left,p1_bb_top - p2_bb_top) != PURPLE && getpixel(enemy,p1_bb_right - p2_bb_left,p1_bb_top - p2_bb_top) != PURPLE)
    {
        return true;
    }
    else
    {
        return false;
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
     if (frame_counterSHOT > 3)
         frame_counterSHOT = 0;
}

void jet::frameSHOT(int num)
{
     if(shoot == true){
         if(num<=1)
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
    if(num < 2)
    	b = 0;
    else if(num >= 2 && num < 4)
    	b = 43;
    else if(num >= 4 && num < 6)
    	b = 86;
    else if(num >= 6 && num < 8)
         b = 129;
}
void jet::frame_reset()
{
    if(frame_counter > 8)
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
    if(key[L] && key[R])
    {
        a = 114;
        c = 34;
        turn = 0;
    }
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
        else if (num >= 20 && num < 25)
        {
             a = 0;
             c = 18;
        }
    }
    if(keyrel(L))
        turn = 1;
    if(a != 144 && Left == false && Right == false && turn == 1)
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
        else if(num >= 20 && num < 25)
        {
            a = 114;
            c = 34;
        }
        if(num > 24 && a == 114)
            turn = 0;
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
        else if (num >= 20 && num < 25)
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
        else if(num >= 20 && num < 25)
        {     
            a = 114;
            c = 34;
        }
        if(num > 24 && a == 114)
            turn = 0;
    }        
}


void jet::wave_reset()
{
    if (wave_counter > 9)
       wave_counter = 0;
}
void jet::wave_frame(int num,int damage)
{
    
    //Big laser
    /*
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
    */
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
        case 4: i = 4;
                break;
        case 2:
        case 3: i = -1;
                break;
        case 6:
        case 9: i = 14;
                break;
        case 7:
        case 8:i = 19;
                break;
    }
    */
    
    switch(num)
    {
        case 0:
        case 5: i = 2;
                break;
        case 1:
        case 4: i = -3;
                break;
        case 2:
        case 3: i = -8;
                break;
        case 6:
        case 9: i = 7;
                break;
        case 7:
        case 8:i = 12;
                break;
    }
    
    
    //Default
    if(damage < 3)
    {
        if (c == 18)
            i = 2;
        else
            i = 9;
    }
}
//Bullet Functions

Bullet::Bullet(){
    bx = 0;
    by = 0;
    
    bs = 16;
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

void moveable::setup(int hp, int x,int y,float xs,float ys,int n)
{
    HP = hp;
    global_x = x;
    global_y = y;
    x_speed = xs;
    y_speed = ys;
    type = n;
}

//Detect Keyrelease
int keyrel(int k)
{
    static int initialized = 0;
    static int keyp[KEY_MAX] = {1};
 
    if(!initialized)
    {
        int i;
        for(i = 0; i < KEY_MAX; i++) keyp[i] = 0;
        initialized = 1;
    }
 
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
//animation

void animation::initialize()
{
    x = 0;
    y = 0;
    counter = 0;
    type = 0;
    show = false;
    bombw = 256;
    bombh = 250;
}

void animation::setup(int x_pos, int y_pos,int ani_type)
{
    x = x_pos;
    y = y_pos;
    counter = 0;
    type = ani_type;
    show = true;
}
