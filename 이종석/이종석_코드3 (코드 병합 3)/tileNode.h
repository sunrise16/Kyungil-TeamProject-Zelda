#pragma once

// 게임 화면 타일
#define TILESIZE 64
#define MAP_TILEX 50
#define MAP_TILEY 50
#define TILESIZEX (MAP_TILEX * TILESIZE)
#define TILESIZEY (MAP_TILEY * TILESIZE)

#define TILEMAPSIZEX 11
#define TILEMAPSIZEY 4

#define CAMERA_TILEX WINSIZEX/TILESIZE
#define CAMERA_TILEY WINSIZEY/TILESIZE

// 이미지 타일 (화면 우측 상단에 붙여 놓을 샘플 타일)
//X SIZE 고정 6 (384pixel)

#define SAMPLE_TILEX 47
#define SAMPLE_TILEY 24

// 맵툴에서 사용할 컨트롤들
enum CTRL
{
	CTRL_SAVE, CTRL_LOAD,
	CTRL_TERRAIN, CTRL_OBJECT, CTRL_ERASER,
	CTRL_MAP1, CTRL_MAP2, CTRL_MAP3, CTRL_END
};

// 지형
enum TERRAIN
{
	TR_WALL, TR_GROUND, TR_GRASS, TR_WATER, TR_END
};

// 오브젝트
enum OBJECT
{
	//BLOCK 종류 1 = 아래 일부 지나가는거 불가 / 2 = 절반 불가 / 3 
	OBJ_CARPET, OBJ_WALL,
	OBJ_BLOCK1, OBJ_BLOCK2, OBJ_BLOCK3, OBJ_BLOCKS,
	OBJ_LINK, OBJ_BOSS, OBJ_MOB1, OBJ_MOB2,
	OBJ_NONE
};

enum MAPTOOL_MENU
{
	MENU_HOUSE,
	MENU_TOWN,
	MENU_BOSS,
	MENU_AUTO,
	MENU_CHAR
};

// 타일 구조체
struct tagTile
{
	TERRAIN terrain;
	OBJECT obj;
	RECT rc;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        

	int x, y;
	int terrainFrameX;
	int terrainFrameY;
	int objFrameX;
	int objFrameY;
};

// 이미지 타일 구조체
struct tagSampleTile
{
	RECT rc;
	int terrainFrameX;
	int terrainFrameY;
	int indexX, indexY;
};

// 현재 타일 구조체
struct tagCurrentTile
{
	int x, y;
};