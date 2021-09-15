#include <fileioc.h>
#include <fontlibc.h>
#include "gfx/gfx.h"
#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tice.h>

#define TILE_WIDTH          16
#define TILE_HEIGHT         16

#define TILEMAP_WIDTH       120
#define TILEMAP_HEIGHT      60

#define TILEMAP_DRAW_WIDTH  20
#define TILEMAP_DRAW_HEIGHT 15

#define Y_OFFSET            0
#define X_OFFSET            0

//define color values:
#define BLACK				1
#define GRAY				3
#define GREEN				4
#define RED					5
#define TRANSPARENT	0
#define WHITE				2
#define YELLOW			6

#define MAX_SPRITES 25

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static char* version = "1.0";

sk_key_t key;

static size_t numSprites = 5;

int waitingCount;
int score;
int dead;

int playerX = 137;
int playerY = 97;
int playerW = 46;
int playerH = 46;
int colorCount;
int playerColor = 1;

const int hitboxAdjust = 3;

int objectX[25];
int objectY[25];
int objectW[25];
int objectH[25];
int objectVX[25];
int objectVY[25];
gfx_sprite_t* loadedSprites[25];

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------
    
	loadedSprites[0] = techcrafter;
	loadedSprites[1] = playerBlue;
    loadedSprites[2] = playerGreen;
    loadedSprites[3] = playerRed;
    loadedSprites[4] = playerYellow;
    loadedSprites[5] = meteorBrown_tiny1;
    loadedSprites[6] = meteorGrey_tiny1;
    loadedSprites[7] = meteorBrown_tiny2;
    loadedSprites[8] = meteorGrey_tiny2;
    loadedSprites[9] = meteorBrown_small1;
    loadedSprites[10] = meteorGrey_small1;
    loadedSprites[11] = meteorBrown_small2;
    loadedSprites[12] = meteorGrey_small2;
    loadedSprites[13] = meteorBrown_med1;
    loadedSprites[14] = meteorGrey_med1;
    loadedSprites[15] = meteorBrown_med3;
    loadedSprites[16] = meteorGrey_med2;
    loadedSprites[17] = meteorBrown_big1;
    loadedSprites[18] = meteorGrey_big1;
    loadedSprites[19] = meteorBrown_big3;
    loadedSprites[20] = meteorGrey_big3;
    loadedSprites[21] = meteorBrown_big4;
    loadedSprites[22] = meteorGrey_big2;
    loadedSprites[23] = meteorGrey_big4;
    loadedSprites[24] = meteorBrown_big2;
    
    for (size_t i = 0; i < MAX_SPRITES; i++)
	{
	    objectX[i] = 0;
	    objectY[i] = 0;
	    
	    if(i >= 5 && i <= 8)
	    {
	        objectW[i] = 18;
	        objectH[i] = 18;
	    }
	    else if(i >= 9 && i <= 12)
	    {
	        objectW[i] = 28;
	        objectH[i] = 28;
	    }
	    else if(i >= 13 && i <= 16)
	    {
	        objectW[i] = 43;
	        objectH[i] = 43;
	    }
	    else if(i >= 17)
	    {
	        objectW[i] = 50;
	        objectH[i] = 50;
	    }
	    
	    objectVX[i] = 0;
	    objectVY[i] = 0;
	    while(objectVX[i] == 0 || objectVY[i] == 0 || objectVX[i] > 4 || objectVY[i] > 4)
	    {
	        objectVX[i] = random()/RAND_MAX;
	        objectVX[i] -= 400.0f;
	        objectVX[i] /= 100.0f;
	        objectVY[i] = random()/RAND_MAX;
	        objectVY[i] -= 400.0f;
	        objectVY[i] /= 100.0f;
	    }
	}
}

//---------------------------------------------------------------------------------
static void moveSprites() {
//---------------------------------------------------------------------------------
	gfx_Sprite(bgp1, 0, 0);
	gfx_Sprite(bgp2, 160, 0);
	gfx_Sprite(bgp3, 0, 120);
	gfx_Sprite(bgp4, 160, 120);
	
	for (size_t i = 0; i < numSprites + 1; i++)
	{
		if(i > 0 && i < 5 && i == playerColor)
		{
		    gfx_TransparentSprite(loadedSprites[i], playerX, playerY);
		}
		else if(i >= 5)
		{
		    objectX[i] += objectVX[i];
		    objectY[i] += objectVY[i];
		    gfx_TransparentSprite(loadedSprites[i], objectX[i], objectY[i]);

		    // Check for collision with the screen boundaries
		    if ((objectX[i] < 0 && objectVX[i] < 0.0f) ||
			    (objectX[i] > (SCREEN_WIDTH-objectW[i]) && objectVX[i] > 0.0f))
			    objectVX[i] = -objectVX[i];

		    if ((objectY[i] < 0 && objectVY[i] < 0.0f) ||
			    (objectY[i] > (SCREEN_HEIGHT-objectH[i]) && objectVY[i] > 0.0f))
			    objectVY[i] = -objectVY[i];
		    
		    if(objectX[i] + objectW[i] > playerX + hitboxAdjust && objectX[i] < playerX + playerW - hitboxAdjust && objectY[i] + objectH[i] > playerY + hitboxAdjust && objectY[i] < playerY + playerH - hitboxAdjust)
		    {
		        dead = 1;
		    }
		}
	}
}

int getXBlock(int xBlock)  //get x-block from tilemap
{
	return TILE_WIDTH * TILEMAP_DRAW_WIDTH * xBlock;
}

int getYBlock(int yBlock)  //get y-block from tilemap
{
	return TILE_HEIGHT * TILEMAP_DRAW_HEIGHT * yBlock;
}

int main(void)  //main function
{
	//initialization
	
	gfx_Begin();
	
	gfx_SetPalette(global_palette, sizeof_global_palette, 0);
	gfx_SetTransparentColor(TRANSPARENT);
    gfx_SetColor(BLACK);
	
	gfx_SetDrawBuffer();
	gfx_FillScreen(BLACK);
	gfx_SwapDraw();
	
	srand(rtc_Time());
	initSprites();
	
	gfx_SetMonospaceFont(8);
	gfx_SetTextTransparentColor(TRANSPARENT);
	gfx_SetTextFGColor(WHITE);
    gfx_SetTextBGColor(TRANSPARENT);
	
	gfx_FillScreen(BLACK);
    gfx_TransparentSprite(techcrafter, 100, 60);
    gfx_PrintStringXY("Spodge84", 0, 232);
	gfx_PrintStringXY(version, 296, 232);
	gfx_SwapDraw();
	
	delay(3000);
	
	//---------------------------------------------------------------
	
	//play
	play:
	
	while(1)
	{
	    gfx_FillScreen(BLACK);
	    moveSprites();
	    
	    if(dead == 1)
	    {
	        goto dead;
	    }
	    
	    key = kb_Data[7];
		if(key & kb_Up && playerY > 0)
		{
			playerY -= numSprites - 4;
		}
		else if(key & kb_Down && playerY + playerH < 240)
		{
			playerY += numSprites - 4;
		}
		
	    if(key & kb_Left && playerX > 0)
		{
			playerX -= numSprites - 4;
		}
		else if(key & kb_Right && playerX + playerW < 320)
		{
			playerX += numSprites - 4;
		}
		
		if(playerY < 0)
		{
		    playerY = 0;
		}
		else if(playerY + playerH > 240)
		{
		    playerY = 194;
		}
		
		if(playerX < 0)
		{
		    playerX = 0;
		}
		else if(playerX + playerW > 320)
		{
		    playerX = 274;
		}
	        
	    key = os_GetCSC();
		if(key == sk_Clear)
		{
			goto pause;
		}
	    
	    colorCount++;
	    if(colorCount == 18)
	    {
	        if(playerColor < 4)
            {
                playerColor++;
            }
            else
            {
                playerColor = 1;
            }
            score++;
            colorCount = 0;
	    }
	    
	    waitingCount++;
	    if(waitingCount == 360)
		{
		    if(numSprites < MAX_SPRITES)
		    {
		        numSprites++;
		    }
		    waitingCount = 0;
		}
	    
	    gfx_PrintStringXY("Score:", 0, 232);
	    gfx_SetTextXY(50, 232);
        gfx_PrintInt(score, 1);
	    
	    gfx_SwapDraw();
	}
	
	//---------------------------------------------------------------
	
	//pause
	pause:
	
	gfx_SetColor(GRAY);
	gfx_FillRectangle(0, 70, 320, 105);
	gfx_SetTextScale(2, 2);
	gfx_PrintStringXY("Pause menu", 80, 90);
	gfx_SetTextScale(1, 1);
	gfx_PrintStringXY("Press [clear] to continue...", 55, 125);
	gfx_PrintStringXY("Press [del] to exit...", 62, 145);
	
	gfx_PrintStringXY("Score:", 0, 232);
    gfx_SetTextXY(50, 232);
    gfx_PrintInt(score, 1);
	
	gfx_SwapDraw();
	
	while(1)
	{
		key = os_GetCSC();
		if(key == sk_Clear)
		{
			goto play;
		}
		else if(key == sk_Del)
		{
			goto quit;
		}
	}
	
	//---------------------------------------------------------------
	
	//dead
	dead:
	
	gfx_PrintStringXY("GAME OVER!", 245, 232);
	gfx_PrintStringXY("Score:", 0, 232);
    gfx_SetTextXY(50, 232);
    gfx_PrintInt(score, 1);
	gfx_SwapDraw();
	
	delay(2000);
	
	dead = 0;
	waitingCount = 0;
	score = 0;
	playerX = 137;
    playerY = 97;
    numSprites = 5;
    colorCount = 0;
    playerColor = 2;
    
    for (size_t i = 0; i < MAX_SPRITES; i++)
	{
	    objectX[i] = 0;
	    objectY[i] = 0;
	    
	    if(i >= 5 && i <= 8)
	    {
	        objectW[i] = 18;
	        objectH[i] = 18;
	    }
	    else if(i >= 9 && i <= 12)
	    {
	        objectW[i] = 28;
	        objectH[i] = 28;
	    }
	    else if(i >= 13 && i <= 16)
	    {
	        objectW[i] = 43;
	        objectH[i] = 43;
	    }
	    else if(i >= 17)
	    {
	        objectW[i] = 50;
	        objectH[i] = 50;
	    }
	    
	    objectVX[i] = 0;
	    objectVY[i] = 0;
	    while(objectVX[i] == 0 || objectVY[i] == 0 || objectVX[i] > 4 || objectVY[i] > 4)
	    {
	        objectVX[i] = random()/RAND_MAX;
	        objectVX[i] -= 400.0f;
	        objectVX[i] /= 100.0f;
	        objectVY[i] = random()/RAND_MAX;
	        objectVY[i] -= 400.0f;
	        objectVY[i] /= 100.0f;
	    }
	}
	
	goto play;
  
    //---------------------------------------------------------------
	
	//quit
	quit:
	
	gfx_End();
	return 0;
}
