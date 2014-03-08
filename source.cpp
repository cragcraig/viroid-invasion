#include <allegro.h> // Allegro Header file
#include <math.h>
#include "data.h"

// g++ source.cpp -o linux_cell `allegro-config --libs`

    ////////////////////////////////////////////////////////////////////////////
    /////////   VARIABLES   ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
//
// Core Variables
bool exit_key = false;
const int max_bullets = 300;
const int max_enemies = 100;
const int tmax_speed = 8;
const int bullet_sprite[2][3] = {{bantibody,bbantibody,bbbantibody},{antibodya,antibodyb,antibodyc}};
const int enemy_sprite[9] = {bloodcell,flagella,BAC,squirm,bluepill,pinkpill,bloodcell2,bloodcell3,BACrain}; // DONT CHANGE ORDER! Just add new ones on the end.
const int bullet_type_sprite[2] = {powerbarblue,powerbarpink};
float max_speedx = tmax_speed;
float max_speedy = tmax_speed;
int sprite_w = 9; // Bullet sprite offset
int max_hurt = 1000;
int warning_counter = 0;
int pauser = 0;
bool game_over = true;
bool exit_now = false;
bool choose_endgame = false;
bool select_dif = false;
bool highscore = false;
char tbuf[20];
int key_pressed = 0;
int input[7];
bool s_key=false;
int input_counter = 0;
int char_input = 0;
int highscore_place = -1;
int char_pixlength = 0;
int l_highscore[10][8] = {{2,17,0,8,6,-1,-1,200000},{2,17,0,8,6,-1,-1,150000},{2,17,0,8,6,-1,-1,100000},{2,17,0,8,6,-1,-1,80000},{2,17,0,8,6,-1,-1,65000},{2,17,0,8,6,-1,-1,50000},{2,17,0,8,6,-1,-1,30000},{2,17,0,8,6,-1,-1,15000},{2,17,0,8,6,-1,-1,7000},{2,17,0,8,6,-1,-1,3000}};
const char *char_converter[26] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};

// Bullet Structure
struct bullet {
       int type, sprite, attach, bultype;
       float xspeed, yspeed;
       float x, y;
} p_sys[max_bullets];

// Player Structure
struct WBC {
       int sprite, basepairs;
       float xspeed, yspeed, x, y, kickback, frame;
       bool damaged;
} player;

// Enemy Structure
struct cell {
       float type, x, y, xspeed, yspeed, kickback, frame, health, width, height, radius, initial_health;
       int sprite, hoffset, woffset;
} enemy[max_enemies];

// Game Variables
int text_slider = -400;
float diff = 0; // Enemy speed changer
int level = 0;
unsigned int level_counter = 0;
int big_level = 0;
int gun_type = 1;
int gun_lvl = 1;
int screen_x = 0;
int screen_y = 9999999;
int star_x = 0;
int star_y = 0;
int shipsize = 35;
int padding = 45 + shipsize;
int temp = int(0.17*max_hurt);
int enemies_on_screen = 0;
int score = 0;
bool ship_shot = false;

    ////////////////////////////////////////////////////////////////////////////
    /////////   FUNCTIONS   ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////


// Make Bullet
void make_bullet(float xoff, float yoff, float xspeed, float yspeed, int type)
{
         xspeed += (rand()%21-10)*0.05;
         for (int p = 0; p < max_bullets; p++)
          {
          if (p_sys[p].type == 0) {p_sys[p].type = type; p_sys[p].xspeed = xspeed; p_sys[p].yspeed = yspeed; p_sys[p].x = int(player.x + xoff); p_sys[p].y = int(player.y - yoff); p_sys[p].sprite = rand()%3; p_sys[p].attach = -1; p_sys[p].bultype = gun_type-1; p = max_bullets * 2;}
          }
}

// Make Enemy
void make_enemy(int type, float xpos, float ypos, float xspeed, float yspeed, int sprite, float health, int width, int height, int radius, int woff, int hoff)
{
         for (int p = 0; p < max_enemies; p++)
          {
          if (enemy[p].type == 0) {enemy[p].type = type; enemy[p].xspeed = xspeed; enemy[p].yspeed = yspeed; enemy[p].x = xpos + screen_x; enemy[p].y = ypos + screen_y; enemy[p].sprite = sprite; enemy[p].health = health; enemy[p].height = height; enemy[p].width = width; enemy[p].radius = radius; enemy[p].frame = 0; enemy[p].hoffset = hoff; enemy[p].woffset = woff; enemy[p].frame = 0; enemy[p].initial_health = health; p = max_enemies * 2;}
          }
}

// Detect Collision with Enemy
int collision_enemy(float x, float y, int offset)
{
         int collide = 0;
         for (int p = 0; p < max_enemies; p++)
          {
          if (enemy[p].type > 1 && enemy[p].health > 0)
           {
           if (enemy[p].radius && (enemy[p].x - x) * (enemy[p].x - x) + (enemy[p].y - y) * (enemy[p].y - y) <= (enemy[p].radius - offset) * (enemy[p].radius - offset)) {collide = p + 1; p = max_enemies * 2;}
           else if (!enemy[p].radius && abs(int(enemy[p].x - x)) <= enemy[p].width/2 - enemy[p].woffset && abs(int(enemy[p].y - y)) <= enemy[p].height/2 - enemy[p].hoffset) {collide = p + 1; p = max_enemies * 2;}
           }
          }
         if (collide) return collide;
         else return 0;
}

// Control Ship
void ship_control()
{
        // Kickback from bullets
        if (player.y - 500 - screen_y < player.kickback) player.y += 10;
        if (player.y - 500 - screen_y > player.kickback) player.y = player.kickback + 500 + screen_y;
        if (player.kickback > 0) player.kickback -= 1.5;

        // Get player input (left/right)
        if (key[KEY_LEFT] && player.xspeed > -max_speedx) player.xspeed -= 0.4;
        if (key[KEY_RIGHT] && player.xspeed < max_speedx) player.xspeed += 0.4;

        // X-Edges
        if (player.x < padding * 2 && player.xspeed < 0) {max_speedx = tmax_speed * (player.x - padding)/padding; if (player.xspeed < -max_speedx) player.xspeed = -max_speedx;}
        else if (player.x > 800 - (padding * 2) && player.xspeed > 0) {max_speedx = tmax_speed * ((800 - player.x) - padding)/padding; if (player.xspeed > max_speedx) player.xspeed = max_speedx;}
        else max_speedx = tmax_speed;
        if ((player.x < padding && player.xspeed < 0) || ((800 - player.x) < padding && player.xspeed > 0)) max_speedx = 0;

        // Friction
        if (!key[KEY_LEFT] && player.xspeed < 0) {player.xspeed += 0.4; if (player.xspeed > 0) player.xspeed = 0;}
        if (!key[KEY_RIGHT] && player.xspeed > 0) {player.xspeed -= 0.4; if (player.xspeed < 0) player.xspeed = 0;}

        // Move Cell
        player.x += player.xspeed;

        // Animation
        player.frame += 0.1;
        if (player.frame >= 3) player.frame = 0;

        // Enemy Collision
        if (player.kickback <= 0) player.damaged = false;
        if (collision_enemy(player.x,player.y,0) && player.damaged == false) {player.kickback = 250; player.damaged = true; gun_lvl--;}
}

void player_input()
{
        // SWITCH GUNS
        if (key[KEY_1]) gun_lvl = 10;
        if (key[KEY_2]) gun_type = 1;
        if (key[KEY_3]) gun_type = 2;
        if (key[KEY_4]) level = 4;
        if (key[KEY_5]) level = 5;
        if (key[KEY_6]) level = 6;
}

void ship_shoot()
{
        if (gun_lvl > 10) gun_lvl = 10; 
        if (gun_lvl < 1) gun_lvl = 1;
        
        // SHOOT
        if (key[KEY_SPACE] && player.kickback <= 0)
         {
	         if (gun_type == 1)
	         {
	         player.kickback = 30-5*((gun_lvl+1)%2);
	         if (gun_lvl <= 2) {make_bullet(0,0,0,-12,gun_type);}
	         else if (gun_lvl <= 4) {make_bullet(-24,-40,0,-12,gun_type); make_bullet(24,-40,0,-12,gun_type);}
	         else if (gun_lvl <= 6) {make_bullet(-24,-40,-2,-12,gun_type); make_bullet(24,-40,2,-12,gun_type); make_bullet(0,0,0,-12,gun_type);}
				else if (gun_lvl <= 8) {make_bullet(-24,-40,-2,-12,gun_type); make_bullet(24,-40,2,-12,gun_type); make_bullet(-13,-40,-1,-13,gun_type); make_bullet(13,-40,1,-13,gun_type);}
	         else {make_bullet(0,0,0,-13,gun_type); make_bullet(-24,-40,-2,-12,gun_type); make_bullet(24,-40,2,-12,gun_type); make_bullet(-13,-40,-1,-13,gun_type); make_bullet(13,-40,1,-13,gun_type);}
		 }
		 if (gun_type == 2)
		 {
		  	 if (gun_lvl < 4) {make_bullet(0,0,0,-12,gun_type); player.kickback = 30-int(3*gun_lvl);}
		  	 else if (gun_lvl < 6) {make_bullet(-24,-40,0,-12,gun_type); make_bullet(24,-40,0,-12,gun_type); make_bullet(0,0,0,-12,gun_type); player.kickback = 32-int(1.5*gun_lvl);}
		  	 else if (gun_lvl < 8) {make_bullet(-24,-40,0.3,-12,gun_type); make_bullet(24,-40,-0.3,-12,gun_type); make_bullet(-15,-40,0.5,-13,gun_type); make_bullet(15,-40,-0.5,-13,gun_type); player.kickback = 32-int(gun_lvl);}
		    else {make_bullet(-24,-40,0.3,-12,gun_type); make_bullet(24,-40,-0.3,-12,gun_type); make_bullet(-15,-40,0.5,-13,gun_type); make_bullet(15,-40,-0.5,-13,gun_type); make_bullet(-19,-40,-0.5,-15,gun_type); make_bullet(19,-40,0.5,-15,gun_type); player.kickback = 32-int(gun_lvl);}
		 }
		 ship_shot = true;
         }
}

bool e_chance(int val)
{
 	 if (enemies_on_screen<3) {if (diff < 0) val /= 2; else val = int(val/(2+1.5*diff));}
 	 if (val == 0) val=1;
 	 if (val*(1-diff) > val/5) val = int(val*(1-diff)); else val = int(val/5);
	 if (rand()%val == 0 && enemies_on_screen < 3 + 5*diff) {if (!enemies_on_screen) enemies_on_screen = 1; return true;} else return false;
}

// Move Background
void move_background()
{
        if (screen_y >= 0) star_y = (int(screen_y / 800) * 800) - screen_y;
        if (screen_y < 0) star_y = (int(screen_y / 800) * 800) + screen_y;
}

// Timer
volatile long speed_counter = 0;
void increment_speed_counter()
{
	speed_counter++;
}
END_OF_FUNCTION(increment_speed_counter);

// Update Variables at beginning of each game loop
void update_variables()
{
        // SPEED COUNTER
        speed_counter --;
        level_counter++;
        if (level_counter > 1800 || (level_counter > 350 && level == 0)) {level++; level_counter = 0;}
        if (level > 6) {level = 0; diff += 0.1; text_slider = 800; big_level++; score += 500*big_level;}
        // SCREEN POSITION
        screen_y -= 1;

        // Text
        static int stop_slide = 0;
        if (stop_slide < 1 && text_slider == 200) stop_slide = 100;
        if (stop_slide > 0) stop_slide --;
        if (text_slider > -400 && stop_slide == 0) text_slider -= 4;
}

void make_red_cells()
	 {
	    // Red Blood Cells
        static int cell_timer = 0;
        cell_timer++;
		if (cell_timer > 220) cell_timer = 0;
        if (rand()%200 == 0 || cell_timer == 0) {int img = rand()%2*7; if (level <= 1 || level > 5 || ((level == 5 || level == 2) && rand()%2 == 0)) img = 6; make_enemy(1,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%13*0.1 + 0.7,img,100,45,46,25,0,0);}
	 }

void levels()
{
 	 // CONTROL ENEMY CREATION
 	    if (level == 1) {
        if (e_chance(50)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.7 + diff,1,50+25*int(diff*3),50,100,0,12,10);
        if (e_chance(20)) make_enemy(2,100+rand()%600,-75,(rand()%21-10)*0.1,rand()%5*0.1 + 0.2 + diff,2,150+25*int(diff*10),127,150,0,34,25);
        if (e_chance(175)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.4 + diff,3,100,60,100,33,0,0);
		}
		else if (level == 2) {
        if (e_chance(50)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.8 + diff,1,50+25*int(diff*5),50,100,0,12,10);
		}
		else if (level == 3) {
        if (e_chance(75)) make_enemy(2,700-rand()%200,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.8 + diff,1,50+25*int(diff*3.5),50,100,0,12,10);
        if (e_chance(200)) make_enemy(2,100+rand()%600,-75,(rand()%21-10)*0.1,rand()%5*0.1 + 0.2 + diff,2,200+25*int(diff*3),127,150,0,34,25);
		}
		else if (level == 4) {
        if (e_chance(120)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.7 + diff,1,100,50,100,0,12,10);
        if (e_chance(200)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.4 + diff,3,150+25*int(diff*10),60,100,33,0,0);
		}
		else if (level == 5) {
        if (e_chance(150)) make_enemy(2,100+rand()%600,-75,(rand()%21-10)*0.1,rand()%5*0.1 + 0.3 + diff,2,200+25*int(diff*5),127,150,0,34,25);
        if (e_chance(200)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.5 + diff,3,150+25*int(diff*3),60,100,33,0,0);
		}
		else if (level == 6) {
        if (e_chance(150)) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.7 + diff,1,50+25*int(diff*3),50,100,0,12,10);
        if (e_chance(75)) make_enemy(2,300+rand()%100,-50,(rand()%21-10)*0.1,rand()%5*0.1 + 0.5 + diff,3,100+25*int(diff*10),60,100,33,0,0);
		}
}

// Update Enemy
void update_enemy(int x)
{
          // Move and bounce
          enemy[x].x += enemy[x].xspeed;
          enemy[x].y += enemy[x].yspeed;
          if (enemy[x].x < padding - 15 || 800 - enemy[x].x < padding - 15) {enemy[x].xspeed *= -1;}
          // Animate
          if (enemy[x].type > 1)
          {
          // If Health is less than zero, start death sequence
          if (enemy[x].health <= 0 && enemy[x].health > -100000)
		  {
		  enemy[x].frame = 3;
		  enemy[x].health = -100000;
		  score += int(enemy[x].initial_health);
		  // Powerups
		  if (rand()%7 == 0)
		  {
		  int img = rand()%2;
		  make_enemy(-2-img, enemy[x].x - screen_x, enemy[x].y - screen_y, enemy[x].xspeed, enemy[x].yspeed, 4+img, 0, 25, 42, 0, 5, 5);
		  }
		  }
          enemy[x].frame += 0.1;
          // Restart Animation or die/make DNA
          if (enemy[x].frame >= 3 && enemy[x].health > 0) enemy[x].frame = 0;
          if (enemy[x].frame >= 6) enemy[x].type = 0;
          }
          // Pill Collision with Player
		  if (enemy[x].type < -1 && abs(int(player.x-enemy[x].x)) < 30 && abs(int(player.y+40-enemy[x].y)) < 30)
		  {
		  if (enemy[x].type == -2) {gun_lvl++; gun_type = 1;}
		  if (enemy[x].type == -3) {gun_lvl++; gun_type = 2;}
		  score += 100;
		  enemy[x].type = 0;
		  }
          // Delete if off screen
          if (enemy[x].y - enemy[x].height/2 > screen_y + 600) {if (enemy[x].type > 1) temp += int(enemy[x].health); enemy[x].type = 0;}
}

// Update Bullet
void update_bullet(int x)
{
         // Move
         if (p_sys[x].type > 0)
          {
          p_sys[x].x += p_sys[x].xspeed;
          p_sys[x].y += p_sys[x].yspeed;
          }
         // Normal Antibody
         if (p_sys[x].type > 0)
          {
          if (p_sys[x].x < padding - 35 || 800 - p_sys[x].x < padding - 35) {p_sys[x].xspeed = 0; p_sys[x].yspeed = 0; p_sys[x].x += rand()%20 - 10; p_sys[x].type = -1;}
          if (p_sys[x].attach = collision_enemy(p_sys[x].x,p_sys[x].y,5)) {p_sys[x].type = -2; p_sys[x].attach -= 1; enemy[p_sys[x].attach].health -= 50;}
          }
         // Attached Antibody
         if (p_sys[x].type == -2)
          {
          p_sys[x].x += enemy[p_sys[x].attach].xspeed;
          p_sys[x].y += enemy[p_sys[x].attach].yspeed;
          p_sys[x].xspeed = enemy[p_sys[x].attach].xspeed;
          p_sys[x].yspeed = enemy[p_sys[x].attach].yspeed;
          if (enemy[p_sys[x].attach].type == 0 || enemy[p_sys[x].attach].health <= 0) {p_sys[x].type = 1; p_sys[x].xspeed += rand()%200 * 0.01 - 1; p_sys[x].yspeed += rand()%50 * 0.01;}
          }
         // Delete if off screen
         if (p_sys[x].y + sprite_w * 2 < screen_y || p_sys[x].y - sprite_w * 2 > screen_y + 600) p_sys[x].type = 0;
}

void check_ispaused()
	 {
	 if (!key[KEY_P] && pauser == -1) pauser = 0;
	 if (key[KEY_P] && pauser == 0) pauser = 1;
	 if (!key[KEY_P] && pauser == 1) pauser = 2;
	 if ((key[KEY_P] && pauser == 2) || key[KEY_ESC]) pauser = -1;
	 }
	 
void reset_game()
    {
    // CLEAR PARTICLE SYSTEM
    for (int x = 0; x < max_bullets; x++)
      p_sys[x].type = 0;

    // CLEAR ENEMY SYSTEM
    for (int x = 0; x < max_enemies; x++)
      enemy[x].type = 0;
    
    // GLOBALS
	text_slider = -400;
	level = 0;
	level_counter = 200;
	big_level = 0;
	gun_type = 1;
	gun_lvl = 1;
	screen_x = 0;
	screen_y = 9999999;
	star_x = 0;
	star_y = 0;
	shipsize = 35;
	padding = 45 + shipsize;
	temp = int(0.17*max_hurt);
	enemies_on_screen = 0;
	score = 0;
	choose_endgame = false;
	
 	// Set Player to Center
    player.x = 400;
    player.y = 500 + screen_y;
    player.kickback = 0;
    }

    ////////////////////////////////////////////////////////////////////////////
    /////////   MAIN FUNCTION   ////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

int main(void)
{
	allegro_init(); // Initialize Allegro
	install_keyboard(); // Initialize keyboard routines
	install_timer(); // Initialize the timer routines
    install_mouse(); // Initialize the mouse routines
    install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT , 0);

	LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
	install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));

	set_color_depth(32);
   	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 800, 600, 0, 0);
   	set_trans_blender(200,200,200,200);
  	srand(5);
  	int x_c=0;
    
	// MENU VARIABLE
	int menu_s = 0;

	// DATABASE
	DATAFILE *data = load_datafile("data.dat");
	
	// FONT
	FONT *font = (FONT *)data[myfont].dat;
	FONT *sfont = (FONT *)data[smallfont].dat;

	// Bitmaps
	BITMAP *buffer = create_bitmap(800,600);
	
	play_midi((MIDI *)data[s_back].dat, 1);
	    
	reset_game();


	while (!exit_now) // Program Loop
	{
	  	  
    ////////////////////////////////////////////////////////////////////////////
    /////////   MENU LOOP   ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    
    menu_s = 0;
    level = 2;
    select_dif = false;
    
    while (game_over && !exit_now && !highscore)
     {
  	 if (speed_counter > 0)
		{
		speed_counter--;
		highscore = false;
		
		// KEYS
		if (key[KEY_UP] || key[KEY_DOWN] || key[KEY_SPACE] || key[KEY_ENTER]) key_pressed++;
		if ((!key[KEY_UP] && !key[KEY_DOWN] && !key[KEY_SPACE] && !key[KEY_ENTER]) || key_pressed > 40) key_pressed = 0;
		if (key[KEY_UP] && key_pressed == 1) menu_s--;
		if (key[KEY_DOWN] && key_pressed == 1) menu_s++;
		if (menu_s < 0) menu_s = 3;
		if (menu_s > 3) menu_s = 0;
		
		// SELECT OPTION
		if ((key[KEY_SPACE] || key[KEY_ENTER]) && !select_dif && key_pressed == 1) 
		{
		if (menu_s == 0) {select_dif = true; key_pressed = 2; menu_s = 0;}
		if (menu_s == 2) {highscore = true; key_pressed = 2;}
		if (menu_s == 3) exit_now = true;
		}
		
		// SELECT DIFFICULTY
		if ((key[KEY_SPACE] || key[KEY_ENTER]) && select_dif && key_pressed == 1)
		{
		if (menu_s != 3) {reset_game(); game_over = false;}
		if (menu_s == 0) diff = -0.2;
		if (menu_s == 1) diff = 0.1;
		if (menu_s == 2) diff = 0.2;
		if (menu_s == 3) {select_dif = false; key_pressed = 2; menu_s = 0;}
		}
		if (key[KEY_ESC] && select_dif) {select_dif = false; menu_s = 0;}
		
		// MAKE CELLS
		make_red_cells();
		make_red_cells();
		if (enemies_on_screen < 3)
		{
			if (rand()%350 == 0) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%13*0.1 + 1.7,1,50,50,100,0,12,10);
       			if (rand()%900 == 0) make_enemy(2,100+rand()%600,-75,(rand()%21-10)*0.1,rand()%13*0.1 + 0.6,2,150,127,150,0,34,25);
        		if (rand()%700 == 0) make_enemy(2,100+rand()%600,-50,(rand()%21-10)*0.1,rand()%13*0.1 + 1.2,3,100,60,100,33,0,0);
        		// if (rand()%600 == 0) make_enemy(2,100+rand()%600,-75,(rand()%21-10)*0.1,rand()%13*0.1 + 0.8,8,100,100,100,0,10,10); WEIRD RAINBOW ENEMY
		}
		
		// UPDATE ENEMIES (move, draw, stop, and delete)
        enemies_on_screen = 0;
        for (int x = 0; x < max_enemies; x++)
         {
          update_enemy(x);
          // Blit Sprite
          if (enemy[x].type != 0) {masked_blit((BITMAP *)data[enemy_sprite[int(enemy[x].sprite)]].dat, buffer, int(enemy[x].width) * int(enemy[x].frame), 0,int(enemy[x].x - enemy[x].width/2) - screen_x,int(enemy[x].y - enemy[x].height/2) - screen_y,int(enemy[x].width),int(enemy[x].height)); if (enemy[x].type > 1) enemies_on_screen++;}
         }
		
		// DRAW
		masked_blit((BITMAP *)data[viroid].dat,buffer,0,0,250,20,300,100);
		masked_blit((BITMAP *)data[arrow].dat,buffer,0,0,100,200+73*menu_s,50,50);
		if (!select_dif) masked_blit((BITMAP *)data[menulist].dat,buffer,0,0,150,200,170,275);
		if (select_dif) {masked_blit((BITMAP *)data[hardness].dat,buffer,0,0,150,200,300,275); masked_blit((BITMAP *)data[dific].dat,buffer,0,0,295,100,203,42);}
		masked_blit((BITMAP *)data[credits].dat,buffer,0,0,640,500,140,50);
		if (menu_s == 1 && !select_dif) masked_blit((BITMAP *)data[keys].dat,buffer,0,0,350,175,300,400);
		
  		// SCREEN
        acquire_screen();
        blit(buffer, screen, 0,0,0,0,800,600);
        release_screen();
        clear_to_color(buffer, makecol(155,0,0));
		
		} 
     }

    ////////////////////////////////////////////////////////////////////////////
    /////////   GAME LOOP   ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

	// GET READY
	x_c=0;
while (!game_over && x_c<60) {
	if (speed_counter > 0) {
		speed_counter--;
		highscore = true;
		masked_blit((BITMAP *)data[title_list].dat,buffer,0,0,300,260,200,40);
		acquire_screen();
		blit(buffer, screen, 0,0,0,0,800,600);
		release_screen();
		clear_to_color(buffer, makecol(110,0,0));
		x_c++;
	}
}


    while (!game_over)
	 {
     if (speed_counter > 0)
		{
        update_variables();
        make_red_cells();
        
        levels();

        // UPDATE ENEMIES (move, draw, stop, and delete)
        enemies_on_screen = 0;
        for (int x = 0; x < max_enemies; x++)
         {
          update_enemy(x);
          // Blit Sprite
          if (enemy[x].type != 0) {masked_blit((BITMAP *)data[enemy_sprite[int(enemy[x].sprite)]].dat, buffer, int(enemy[x].width) * int(enemy[x].frame), 0,int(enemy[x].x - enemy[x].width/2) - screen_x,int(enemy[x].y - enemy[x].height/2) - screen_y,int(enemy[x].width),int(enemy[x].height)); if (enemy[x].type > 1) enemies_on_screen++;}
         }

        // UPDATE BULLETS (move, draw, stop, and delete)
        for (int x = 0; x < max_bullets; x++)
         {
         update_bullet(x);
         // Blit Bullet Sprite
         if (p_sys[x].type != 0) masked_blit((BITMAP *)data[bullet_sprite[p_sys[x].bultype][int(p_sys[x].sprite)]].dat, buffer, 0, 0,(int(p_sys[x].x) - sprite_w) - screen_x,int(p_sys[x].y) - screen_y,62,62);
         }

        // PLAYER SHIP (draw and control)
        if (player.damaged == true && player.kickback > 200) player.sprite = hurt; else player.sprite = int(player.frame);
        if (player.damaged == true && player.y > screen_y + 600 + shipsize) player.x = 400;
        masked_blit((BITMAP *)data[player.sprite].dat, buffer, 0, 0,int(player.x)-shipsize,int(player.y) - screen_y,62,62);
        ship_control();

        // GUNS (shoot and change)
        ship_shoot();
        if (ship_shot) {play_sample((SAMPLE *)data[s_shoot].dat,127,127,1000,0); ship_shot = false;}
        player_input();

        // Text Slider
        if (text_slider > -400) masked_blit((BITMAP *)data[lvl_up].dat,buffer,0,0,text_slider,200,400,100);
        
        // INFO
        masked_blit((BITMAP *)data[power].dat,buffer,0,0,40,10,100,35);
        masked_blit((BITMAP *)data[bullet_type_sprite[gun_type-1]].dat,buffer,0,0,140,10,gun_lvl*15,35);
		masked_blit((BITMAP *)data[title_list].dat,buffer,0,level*40,40,50,200,40);
		
		// THERMOMETER
		if (temp >= 0.98*max_hurt) {temp = (int)(0.17*max_hurt); gun_lvl -= 3; for (int x=0; x<=600; x+=25) {make_bullet(80-player.x,x,7,rand()%100/100-0.5,gun_type); make_bullet(720-player.x,x,-7,rand()%100/100-0.5,gun_type);} warning_counter = 200;}
		else if (temp < (int)(0.17*max_hurt)) temp = (int)(0.17*max_hurt);
		if (gun_lvl <= 0) {gun_lvl = 1; game_over = true; warning_counter = 400;}
		masked_blit((BITMAP *)data[thermempty].dat,buffer,0,0,800-231,25,151,31);
		masked_blit((BITMAP *)data[thermfull].dat,buffer,151-int((151*(100-100*(max_hurt-temp)/max_hurt))/100),0,800-231+151-int((151*(100-100*(max_hurt-temp)/max_hurt))/100),25,int((151*(100-100*(max_hurt-temp)/max_hurt))/100),31);
		if (key[KEY_ESC]) {choose_endgame = true; warning_counter = 1;}
		check_ispaused();
		
		// SCORE
		textout_centre_ex(buffer, font, "Score", 400, 21, makecol(80,0,0), -1);
		textprintf_centre_ex(buffer, font, 400, 45, makecol(80,0,0), -1, "%d", score);
		
		// LEVEL
		textout_ex(buffer, sfont, "Level", 620, 7, makecol(40,0,0), -1);
		textprintf_right_ex(buffer, sfont, 662, 7, makecol(40,0,0), -1, "%d", big_level+1);
		
		// PAUSE
		while (pauser > 0 || warning_counter)
		{
		if (speed_counter > 0)
		{
		speed_counter --;
		if (!choose_endgame)
		{
		if (warning_counter) {if (game_over) draw_trans_sprite(buffer,(BITMAP *)data[gameover].dat,0,260); else draw_trans_sprite(buffer,(BITMAP *)data[warning].dat,0,260); if (int(warning_counter/10)%2 == 0) masked_blit((BITMAP *)data[thermfull].dat,buffer,0,0,800-231,25,151,31); else masked_blit((BITMAP *)data[thermlit].dat,buffer,0,0,800-231,25,151,31);}
		else masked_blit((BITMAP *)data[paused].dat,buffer,0,0,250,250,300,100);
		}
		else
		{
		masked_blit((BITMAP *)data[endgame].dat,buffer,0,0,250,250,300,100);
		if (key[KEY_Y]) {game_over = true; choose_endgame = false;}
		if (key[KEY_N]) {choose_endgame = false;}
		}
		acquire_screen();
        blit(buffer, screen, 0,0,0,0,800,600);
        release_screen();
        if (warning_counter && !choose_endgame) warning_counter--;
        check_ispaused();
		}
		}


        // SCREEN (blit and clear)
        acquire_screen();
        blit(buffer, screen, 0,0,0,0,800,600);
        release_screen();
        if (key[KEY_S] && !s_key) {save_bmp("screenshot.bmp",buffer,NULL); s_key=true;}
        if (!key[KEY_S]) s_key=false;
        clear_to_color( buffer,makecol( 0, 0, 0) );

        // BACKGROUND (move and draw)
        move_background();
        blit((BITMAP *)data[xback].dat, buffer, 0,0,0,star_y,800,800);
        blit((BITMAP *)data[xback].dat, buffer, 0,0,0,star_y + 800,800,800);
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    /////////   END GAME LOOP   ////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    
    // HIGH SCORE
    if (game_over && !exit_now && highscore)
    {
	// LOAD HIGHSCORES
	if (exists("highscore.mep"))
	{
 	PACKFILE *in;
    in = pack_fopen("highscore.mep", "r");

    for (int z = 0; z < 10; z++)
     for (int y = 0; y < 8; y++)
      l_highscore[z][y] = pack_igetl(in);

    pack_fclose(in);
	}
	// GET VARIABLES SET
	level = 2;
    input_counter = 0;
    clear_keybuf();
    for (int x=0; x<7; x++) input[x] = -1;
    highscore_place = -1;
    for (int x=9; x>=0; x--) if (l_highscore[x][7] < score) highscore_place = x;
    if (highscore_place >= 0)
    {
    for (int x=9; x>highscore_place; x--) for (int y=0; y<7; y++) l_highscore[x][y] = l_highscore[x-1][y];
    l_highscore[highscore_place][7] = score;
	for (int x=0; x<7; x++) l_highscore[highscore_place][x] = -1;
	score = 0;
	}
	}
	
	// HIGHSCORE DISPLAY
    while (game_over && !exit_now && highscore)
     {
  	 if (speed_counter > 0)
		{
		speed_counter--;
		level_counter++;
		if (level_counter > 100) level_counter = 0; 
		
		// KEYS
		if ((key[KEY_ENTER] || key[KEY_ESC]) && key_pressed < 2) {key_pressed = 2; highscore = false;}
		if ((!key[KEY_ENTER] && !key[KEY_ESC]) || key_pressed > 40) key_pressed = 0;
		
		// MAKE CELLS
		make_red_cells();
		make_red_cells();
		
		// UPDATE ENEMIES (move, draw, stop, and delete)
        enemies_on_screen = 0;
        for (int x = 0; x < max_enemies; x++)
         {
          update_enemy(x);
          // Blit Sprite
          if (enemy[x].type != 0) {masked_blit((BITMAP *)data[enemy_sprite[int(enemy[x].sprite)]].dat, buffer, int(enemy[x].width) * int(enemy[x].frame), 0,int(enemy[x].x - enemy[x].width/2) - screen_x,int(enemy[x].y - enemy[x].height/2) - screen_y,int(enemy[x].width),int(enemy[x].height)); enemies_on_screen++;}
         }
		
		// DRAW
		masked_blit((BITMAP *)data[viroid].dat,buffer,0,0,250,20,300,100);
		masked_blit((BITMAP *)data[highsc].dat,buffer,0,0,295,100,203,42);
		if (highscore_place >= 0) textout_ex(buffer, font, ">", 205, 200+30*highscore_place, makecol(30,0,0), -1);
		
		// GET INPUT
		if (highscore_place >= 0)
		{
		while (keypressed() && !key_pressed)
		{
		char_input = (readkey() >> 8);
		if (char_input == KEY_BACKSPACE && input_counter > 0) {input_counter--; input[input_counter] = -1;}
		else if (scancode_to_ascii(char_input)-97 >= 0 && scancode_to_ascii(char_input)-97 <= 26 && input_counter < 7) {input[input_counter] = scancode_to_ascii(char_input)-97; input_counter++;}
		else if (char_input == KEY_SPACE && input_counter != 0 && input_counter < 7 && input[input_counter-1] != -1) {input[input_counter] = -1; input_counter++;}
		}
		
		char_pixlength = 0;
		for (int x=0; x<input_counter; x++) if (input[x] >= 0) {textout_ex(buffer, font, char_converter[input[x]], 250+char_pixlength, 200+30*highscore_place, makecol(30,0,0), -1); char_pixlength+=text_length(font, char_converter[input[x]]);} else char_pixlength+=10;
		if (level_counter < 70) textout_ex(buffer, font, "|", 250+char_pixlength, 200+30*highscore_place, makecol(30,0,0), -1);
		textout_centre_ex(buffer, font, "Hit 'Enter' to save", 400, 515, makecol(60,0,0), -1);
		textout_centre_ex(buffer, font, "High Score!", 410, 200+30*highscore_place, makecol(30,0,0), -1);
		}
		else textout_centre_ex(buffer, font, "Hit 'Enter' to continue", 400, 515, makecol(60,0,0), -1);
		
		// DRAW HIGHSCORES
		for (int y=0; y<10; y++)
		{
		textprintf_centre_ex(buffer, font, 230, 200+30*y, makecol(80,0,0), -1, "%d", y+1);
		char_pixlength = 0;
	    for (int x=0; x<7; x++) if (l_highscore[y][x] >=0 && l_highscore[y][x] <= 26) {textout_ex(buffer, font, char_converter[l_highscore[y][x]], 250+char_pixlength, 200+30*y, makecol(80,0,0), -1); char_pixlength+=text_length(font, char_converter[l_highscore[y][x]]);} else char_pixlength+=10;
		textprintf_right_ex(buffer, font, 550, 200+30*y, makecol(80,0,0), -1, "%d", l_highscore[y][7]);
		}
		
  		// SCREEN
        acquire_screen();
        blit(buffer, screen, 0,0,0,0,800,600);
        release_screen();
        clear_to_color(buffer, makecol(155,0,0));
		
		}
	
	// SAVE HIGHSCORES
    if (game_over && !exit_now && !highscore && highscore_place >= 0)
    {
 	for (int x=0; x<7; x++) l_highscore[highscore_place][x] = input[x];// UPDATE HIGHSCORE LIST
 	
 	PACKFILE *out;
    out = pack_fopen("highscore.mep", "w");

    for (int z = 0; z < 10; z++)
     for (int y = 0; y < 8; y++)
      pack_iputl(l_highscore[z][y], out);

    pack_fclose(out);
	}
    } // END Highscore loop
	

}// END Program loop 

    // Clear Data
    destroy_bitmap(buffer);
    unload_datafile(data);
    //delete [] char_converter;

    return 0;
}
END_OF_MAIN();
