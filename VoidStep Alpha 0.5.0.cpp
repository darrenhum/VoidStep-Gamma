#include<allegro.h>
#include<stdio.h>
#include<iostream>
#include<math.h>
#define maxbullets 1000 

const float ENb_speed = 2;
const int screen_h = 750;
const int screen_w = 500;

using namespace std;

BITMAP *laser;
BITMAP *buffer;
BITMAP *buffer2;

//Timers
volatile long speed_counter = 0;
void increment_speed_counter(){
	speed_counter++;
}END_OF_FUNCTION(increment_speed_counter); 

//Collsion
int collide(float p1_bb_left,float p1_bb_top,float p1_width, float p1_height, float p2_bb_left,float p2_bb_top, float p2_width, float p2_height);
//Detect Keyrelease
int keyrel(int k);
//Player stay in one spot
int stayR(int num);
int stayL(int num, int width);
//Player one laser
int frame_counterSHOT;

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
        int x_change;
        int y_change;
        int type;
        void hurt(){HP -= 1;}
    private:
        float global_x;
        float global_y;
        int shoot_counter;
        int HP;
        int ee;
};

class Bullet{
    public:
        Bullet();
        void shoot();
        void NOTshoot();
        void movebullets(int map_Scroll);
        int x(){return bx;}
        void setx(int n){bx=n;}
        int y(){return by;}
        void sety(int n){by=n;}
        bool ready(){return be != 0;}
        int type;
    private:
        int bx;
        int by;
        int bs;
        int be;
};

/*****************************************************************/
//Player 1+2 Structure
struct jet{
    int x;
    int y;
    int a;   //Blit width
    int b;   //Blit Height
    int c;   //Width
    int d;   //Height
    int Left;
    int Right;
    int w;
    int turn;
    int act;
    int shot;
    int shoot;
    int shoot_x;//Muzzle flare x
    int shoot_y;//Muzzle flare y
    int shotX;
    int dead;
    int blit_shoot_x;
    int frame_counter;
    int frame_counterLR;
    int frame_resetLR (int num);
    void frameLR(int num,int L, int R);
    int frame_counterSHOT;
    void frame(int num);
    int frame_reset(int num);
    void frame_resetSHOT(void);
    void frameSHOT(int num);
    
    int wave_reset(int num);
    int wave_counter;
    int wave_frame(int num);
    int i;
};


//Player 2 Structure
struct laser{
       int x;
       int y;
       bool exist;
      int bb_left;
      int bb_top;
      int bb_right;// = (p2_bb_left + c2);
      int bb_bottom;// = (p2_bb_top + d2); 
};

// End of Player 2
/******************************************************************************************************************************/
int main(int argc, char *argv[]){
    
    allegro_init();
    install_keyboard();
    install_mouse(); 
    install_timer();
    set_color_depth(desktop_color_depth());
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 500, 750, 0, 0);
    LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(200));
    srand(time(NULL));
    
    jet player[2];
    moveable enemy[1000];
    Bullet bullet[maxbullets];
    for(int i = 0; i <= 1000; i ++)
    {
        enemy[i].NOTshoot();
    }
    
    float bgy=0;
    //buffer2 = create_bitmap(screen_w, screen_h);

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

    //Load le picz
    BITMAP *p1 = NULL;
    p1 = load_bitmap("sprites/P1.bmp", NULL);
    BITMAP *p2 = NULL;
    p2 = load_bitmap("sprites/P2.bmp", NULL);
    BITMAP *shoot = NULL;
    shoot = load_bitmap("sprites/shoot.bmp", NULL);
    BITMAP *laser = NULL; 
    laser = load_bitmap("sprites/laser2.bmp", NULL);
    BITMAP *pj = NULL;
    pj = load_bitmap("sprites/enPJ.bmp", NULL); 
    BITMAP *explosion = NULL;
    explosion = load_bitmap("sprites/explosion.bmp", NULL);
    BITMAP *impact = NULL;
    impact = load_bitmap("sprites/impact.bmp", NULL);
    BITMAP *bg = NULL;
    bg = load_bitmap("sprites/bg.bmp", NULL);
    
    BITMAP *enemyp[5] = {NULL};
    enemyp[0] = load_bitmap("sprites/enemy1.bmp", NULL);
    enemyp[1] = load_bitmap("sprites/enemy2.bmp", NULL);
    enemyp[2] = load_bitmap("sprites/enemy3.bmp", NULL);
    enemyp[3] = load_bitmap("sprites/enemy4.bmp", NULL);
    enemyp[4] = load_bitmap("sprites/enemy5.bmp", NULL);
    
    
    BITMAP *buffer = NULL;
    buffer = create_bitmap(1200, 1920);
    
    int x, y = 0;
    //Setting Allegro to text mode
    if(buffer == NULL){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not create buffer!");		
		exit(EXIT_FAILURE);
	}
	
	player[0].wave_counter = 0;
	player[1].wave_counter = 0;
	
    //Player 1
    player[0].frame_counter = 0;
    player[0].frame_counterLR = 0;
    player[0].y = 640;
    player[0].x = 530; 
    player[0].c = 34;
    player[0].d = 44;
    player[0].a = 114;
    player[0].w = 34;
    player[1].frame_counter = 0;
    player[1].frame_counterLR = 0;
    player[1].y = 640;
    player[1].x = 660; 
    player[1].d = 44;
    player[1].a = 114;
    player[1].c = 34;
    player[1].w = 34;
    player[0].i = 0;
    player[1].i = 0;
    player[0].dead = false;
    player[1].dead = false;
    //blit(title_page, screen, 0,0,0,0,480,360);
    
    while(!key[KEY_ESC])
    { 
    draw_sprite(buffer, bg, 0-map_scroll, -1150+(int)bgy);
    textprintf_ex(buffer, font, 200,300, makecol(255,255,255), -1, "Press Space");
    textprintf_ex(buffer, font, 165,320, makecol(255,255,255), -1, "Welcome to VoidStep");
		while(speed_counter > 0)
        {
            bgy+=0.1;
            player[0].Left = false;
            player[0].Right = false;
            player[0].shot = false;
            player[1].Left = false;
            player[1].Right = false;
            player[1].shot = false;
            //Player one controls
    		if(key[KEY_D] && player[0].x < map_scroll+500-34)
            {
    			player[0].x += 2;
    			player[0].Right = true;
    		} 
    		if(key[KEY_A] && player[0].x > map_scroll)
            { 
    			player[0].x -= 1.5;
    			player[0].Left = true;
    		}
    		if(key[KEY_W] && player[0].y > 0)
            {
    			player[0].y -= 1.5;
    		} 
    		if(key[KEY_S] && player[0].y < 750-player[0].d)
            {
    			player[0].y += 1.5;
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
            
           //Player two controls
    		if(key[KEY_RIGHT] && player[1].x < map_scroll+500-34)
            {
    			player[1].x += 2;
    			player[1].Right = true;
    		} 
    		if(key[KEY_LEFT] && player[1].x > map_scroll)
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
    			player[1].y += 1.5;
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
//ZHAO HUAN enemy
            if(current_enemy+8 >= 1000)
            {
                current_enemy = 0;
            }
            if(enemy_counter == 0)
            {

                conjuration_num = rand()%3;
                conjuration_num = 1;
                if(conjuration_num == 0)
                {
                    for(int i = 0; i < 8; i++)
                    {
                         enemy[current_enemy].setup(rand()%750+225, -50,0,3,rand()%5);
                         current_enemy++;
                    }

                }
                else if(conjuration_num == 1)
                {
                     enemy[current_enemy].setup(rand()%750+225, -50,0,3,rand()%5);
                     current_enemy++;
                }
                else
                {
                }
            }

            map_scroll = (player[0].x+34+player[1].x)/2 - screen_w/2;
            if(map_scroll < 225)
                map_scroll = 225;
            else if(map_scroll+screen_w >= 975)
            {
                map_scroll = 975 - screen_w;
            }

            enemy_counter++;
            //Plauyer muzzle flare
            player[0].shoot_x = player[0].x + 8;
            player[0].shoot_y = player[0].y - 3;
            
            player[1].shoot_x = player[1].x + 8;
            player[1].shoot_y = player[1].y - 3;    
                        
            speed_counter --;
            player[0].wave_counter ++;
            player[1].wave_counter ++;
            player[0].wave_counter = player[0].wave_reset(player[0].wave_counter);
            player[1].wave_counter = player[1].wave_reset(player[1].wave_counter);
            player[0].frame_counterLR++;
            player[1].frame_counterLR++;
            player[0].frame_counterLR = player[0].frame_resetLR(player[0].frame_counterLR);
            player[1].frame_counterLR = player[1].frame_resetLR(player[1].frame_counterLR);
            player[0].frame_counter ++;
            player[1].frame_counter ++;
            player[0].frame_counter = player[0].frame_reset(player[0].frame_counter);
            player[1].frame_counter = player[1].frame_reset(player[1].frame_counter);
            player[0].frame_counterSHOT ++;
            player[1].frame_counterSHOT ++;
            player[0].frame_resetSHOT();
            player[1].frame_resetSHOT();
            player[0].i = player[0].wave_frame(player[0].wave_counter);
            player[1].i = player[1].wave_frame(player[1].wave_counter);
            
            for (int i = 0; i < maxbullets; i++)
            {
                if (bullet[i].ready())
                    bullet[i].movebullets(map_scroll);
            }
            if(current_clip >= maxbullets)
                current_clip = 0;
                
            speed_counter --;
            frame_counterSHOT ++;

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
            if (player[0].frame_counterSHOT  >4)
            {
               canshoot = 1;
            }
            if (player[1].frame_counterSHOT >4)
            {
               canshoot2 = 1;
            }
        }
        
        player[0].frameLR(player[0].frame_counterLR,KEY_A, KEY_D);
        player[1].frameLR(player[1].frame_counterLR, KEY_LEFT, KEY_RIGHT);
        player[0].frame(player[0].frame_counter);
        player[1].frame(player[1].frame_counter);
        player[0].frameSHOT(player[0].frame_counterSHOT);
        player[1].frameSHOT(player[1].frame_counterSHOT);
                
        //Player one
        if (player[0].dead == false)
            masked_stretch_blit(p1, buffer,player[0].a,player[0].b,player[0].c, player[0].d, player[0].x - map_scroll, player[0].y, player[0].w, 43);
        if (player[0].shoot == true)
            masked_blit(shoot, buffer, player[0].blit_shoot_x, 0, player[0].shoot_x  - map_scroll - player[0].shotX, player[0].shoot_y, 23 ,17);
        //Player two
        if (player[1].dead == false)
            masked_stretch_blit(p2, buffer,player[1].a,player[1].b,player[1].c, player[1].d, player[1].x  - map_scroll, player[1].y, player[1].w,43);
        if (player[1].shoot == true)
            masked_blit(shoot, buffer, player[1].blit_shoot_x, 0, player[1].shoot_x  - map_scroll - player[1].shotX, player[1].shoot_y, 23 ,17);

        //DEBUGING PURPOSES IGNORE
        
        for (int i = 0; i < maxbullets; i++)
        {
            if (bullet[i].ready())
            {   
                draw_sprite(buffer, laser, bullet[i].x()-map_scroll,bullet[i].y());
            }
        }
        
        
        /////////////////////////////////////////////////////////////////////////////////////
        for(int i = 0; i < 1000; i++)
        {
            if(!enemy[i].dead())
            {
                if (enemy[i].type == 4) 
                    enemy[i].y_speed-=0.0085;
                if (enemy[i].type == 3)
                {
                    enemy[i].x_speed+=0.005;
                    enemy[i].y_speed=2;
                }
                if (enemy[i].type == 2)
                {
                    enemy[i].x_speed-=0.005;
                    enemy[i].y_speed=2;
                }
                if (enemy[i].type == 1)
                {
                    enemy[i].x_speed = 0.5;
                    enemy[i].y_speed -= 0.0085;
                }
                if (enemy[i].type == 0)
                {
                    enemy[i].x_speed = -0.5;
                    enemy[i].y_speed -= 0.0085;
                }
                enemy[i].updatepos();
                draw_sprite(buffer, enemyp[enemy[i].type], enemy[i].x() - map_scroll, enemy[i].y());
            }
        }
        //Bullet Collision
        for(int i = 0; i < 1000; i++)
        {
            if(!enemy[i].dead())
            {
                for (int j = 0; j < maxbullets; j++)
                {
                    if (bullet[j].ready())
                    {
                        if(collide(bullet[j].x()-map_scroll, bullet[j].y(), 16, 20, enemy[i].x()-map_scroll, enemy[i].y(), enemyp[enemy[i].type]->w,enemyp[enemy[i].type]->h))
                         {
                             bullet[j].NOTshoot();
                             draw_sprite(buffer,impact,bullet[j].x()-map_scroll-37+8, bullet[j].y());
                             enemy[i].hurt();                    
                         }
                     }
                }
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
                    if(collide(player[j].x-map_scroll, player[j].y, player[j].c, player[j].d,enemy[i].pj_x-map_scroll, enemy[i].pj_y, pj->w,pj->h))
                    {
                        enemy[i].NOTshoot();
                        player[j].dead = true;
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
                    if(collide(player[j].x - map_scroll, player[j].y, player[j].c, player[j].d, enemy[i].x()-map_scroll, enemy[i].y(), enemyp[enemy[i].type]->w,enemyp[enemy[i].type]->h))
                    {   
                        player[j].dead = true;           
                    }
                }
            }
        }
  
  
        //masked_blit(enemy1, buffer, 0, 0, 50, 50, 47, 64); 
        blit(buffer, screen, 0, 0, 0, 0, screen_w, screen_h);
        clear_bitmap(buffer);
        
    }			
	destroy_bitmap(buffer);

    return(0);
}END_OF_MAIN();

/***********************************************************************************************/
//Functions
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
int jet::frame_reset(int num)
{
    if(num > 12)
        num = 0;
    return(num);
}
int jet::frame_resetLR(int num)
{
    if (Left != true && Right != true)
    {
        if (num > 25)
            num = 0;
    }
    return(num);
}
void jet::frameLR(int num, int L, int R)
{
    if(Left == true)
    {
        if(num < 5)
        {
            a = 85;
            c = 29;
            w = 29;
        }
        else if(num >= 5 && num < 10)
        {
            a = 59;
            c = 26;
            w = 26;
        }
        else if(num >= 10 && num < 15)
        {
            a = 37;
            c = 22;
            w = 22;
        }
        else if(num >= 15 && num < 20)
        {
             a = 18;
             c = 19;
             w = 19;
        }
        else if (num < 25)
        {
             a = 0;
             c = 18;
             w = 18;
        }
    }
    if(keyrel(L))
        turn = 1;
    if(a != 144&& Left == false && Right == false && turn == 1)
    {
        if(num >= 0 && num < 5)
        {
             a = 18;
             c = 19;
             
             w = 19;
        }
        else if(num >= 5 && num < 10)
        {
            a = 37;
            c = 22;
            w = 22;
        }
        else if(num >= 10 && num < 15)
        {
            a = 59;
            c = 26;
            w = 26;
        }
        else if(num <= 15 && num < 20)
        {
            a = 85;
            c = 29;
            w = 29;
        }	
        else if(num < 25)
        {
            a = 114;
            c = 34;
            w = 34;
            turn = 0;
        }
    }
    if(Right == true)
    {
        if(num < 5)
        {
            a = 148;
            c = 29;
            w = 29;
        }
        else if(num >= 5 && num < 10)
        {
            a = 177;
            c = 26;
            w = 26;
        }
        else if(num >= 10 && num < 15)
        {
            a = 203;
            c = 22;
            w = 22;
        }
        else if(num >= 15 && num < 20)
        {
             a = 225;
             c = 19;
             w = 19;
        }
        else if (num < 25)
        {
             a = 244;
             c = 18;
             w = 18;
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
             w = 19;
        }
        else if(num >= 5 && num < 10)
        {
            a = 203;
            c = 22;
            w = 22;
        }
        else if(num >= 10 && num < 15)
        {
            a = 177;
            c = 26;
            w = 26;
        }
        else if(num <= 15 && num < 20)
        {
            a = 148;
            c = 29;
            w = 29;
        }
        else if(num < 25)
        {     
            a = 114;
            c = 34;
            w = 34;
            turn = 0;
        }
    }
}


int jet::wave_reset(int num)
{
    if (num > 13)
       num = 0;
    return num;
}
int jet::wave_frame(int num)
{
    
    //Big laser
    
   /* switch(num)
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
    }*/
    
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
    return i;
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
    static int initialized = 0;
    static int keyp[KEY_MAX];
 
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
