#include "stdafx.h"
#include "mapToolScene.h"

// 초기화 (init)
HRESULT mapToolScene::init()
{
	// 배경 맵
	IMAGEMANAGER->addImage("Bg", "Source/bkBlack.bmp", 64, 64);

	// 맵툴 UI 배경
	IMAGEMANAGER->addImage("maptool_UI_House","Source/Scroll_UI_House.bmp", 904, 400, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("maptool_UI_Town", "Source/Scroll_UI_Town.bmp", 904, 400, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("maptool_UI_Boss", "Source/Scroll_UI_Boss.bmp", 904, 400, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("maptool_UI_Auto", "Source/Scroll_UI_Auto.bmp", 904, 400, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("maptool_UI_Char", "Source/Scroll_UI_Char.bmp", 904, 400, true, RGB(255,0,255));

	// 맵툴 UI 선택 아이콘 (scrollX)
	IMAGEMANAGER->addFrameImage("maptool_UI_SCROLLS", "Source/Scrolls_UI250x60.bmp", 250, 60, 5, 1);

	//============================= TILE & OBJECT IMAGE =================================//

	// 타일 맵 이미지
	IMAGEMANAGER->addFrameImage("tilemap", "Source/타일맵3008x1536.bmp", 3008, 1536, 47, 24);
	// 상단 아이콘 버튼(세이브, 로드, 지우개, 리셋)
	IMAGEMANAGER->addImage("save_button", "Source/세이브버튼.bmp", 100, 40, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("load_button", "Source/로드버튼.bmp", 100, 40, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("eraser_button", "Source/지우개버튼.bmp", 100, 40, true, RGB(255,0,255));
	IMAGEMANAGER->addImage("reset_button", "Source/리셋버튼.bmp", 100, 40, true, RGB(255,0,255));

	// 화살표 (스크롤하는 화살표)
	IMAGEMANAGER->addImage("arrow_right", "Source/오른쪽으로화살표.bmp", 50, 26, true, RGB(255, 0, 255));
	IMAGEMANAGER->addImage("arrow_left", "Source/왼쪽으로화살표.bmp", 50, 26, true, RGB(255, 0, 255));

	// 선택한 타일 표시하는 이미지
	IMAGEMANAGER->addFrameImage("select_tile", "Source/map_tool_UI_select.bmp", 128, 64, 2, 1);
	// 선택한 타일 드래그시 알파렌더할 이미지 (단색)
	IMAGEMANAGER->addImage("select_rect", "Source/dragRect.bmp", 64, 64);

	IMAGEMANAGER->addImage("saveload_UI", "Source/save_UI.bmp", 150, 190, true, RGB(255, 0, 255));

	IMAGEMANAGER->addFrameImage("obj_button", "Source/obj_UI.bmp", 66, 64, 1, 2);

	// 스크롤 변수 초기화
	scrollX = scrollY = 0;
	
	// 맵툴 세팅
	this->mapToolSetup();

	// 현재 타일 초기화
	_currentTile.x = 0;
	_currentTile.y = 0;

	camX = _rcMCamera.rc.left / TILESIZE;
	camY = _rcMCamera.rc.top / TILESIZE;

	// 지형 그리기 속성으로 시작
	_ctrlSelect = CTRL_TERRAIN;
	// 메뉴 HOUSE로 시작
	_menu = MENU_HOUSE;

	// 맵툴UI팝업변수(true가 되면 렉트가 올라옴)
	upUI = tileSelect = objectOn = eraseOn = saveOn = loadOn = tilerectOn = dragOnSample = false;

	return S_OK;
}

// 해제 (release)
void mapToolScene::release()
{
}

// 업데이트 (update)
void mapToolScene::update()
{
	// 키입력 처리 함수
	this->input();

	if (!saveOn || loadOn)
	{
		if (objectOn)
		{
			_ctrlSelect = CTRL_OBJECT;
		}
		else
		{
			_ctrlSelect = CTRL_TERRAIN;
		}
		if (eraseOn)
		{
			_ctrlSelect = CTRL_ERASER;
		}
	}
	
	// TAB 버튼을 누르면 맵툴 UI가 올라옵니다
	if (INPUT->getKeyDown(VK_TAB))
	{
		if (!upUI)
		{
			upUI = true;
		}
		else
			upUI = false;
	
	}
	
	// LBUTTON으로 타일맵에 세팅
	if (INPUT->getKey(VK_LBUTTON))
	{
		this->setMap();
	}
	setSampleTile(selectTiles.right / TILESIZE, selectTiles.bottom / TILESIZE);


	//====================================== 카 메 라 ============================================//

	// 화면 카메라 스크롤 ( W, A, S, D )
	if (INPUT->getKey('A') && _rcMCamera.x - WINSIZEX/2 > 0)
	{
		_rcMCamera.x -= 64;
		if (camX > 0) camX--;
	}
	if (INPUT->getKey('D') && _rcMCamera.x + WINSIZEX/2 < TILESIZEX)
	{
		_rcMCamera.x += 64;
		if (camX < MAP_TILEX - 20) camX++;
	}
	if (INPUT->getKey('W') && _rcMCamera.y - WINSIZEY/2 > 0)
	{
		_rcMCamera.y -= 64;
		if (camY > 0) camY--;
	}
	if (INPUT->getKey('S') && _rcMCamera.y + WINSIZEY/2 < TILESIZEY)
	{
		_rcMCamera.y += 64;
		if (camY < MAP_TILEY - 15) camY++;
	}
	
	this->mapUiAnimation();
	
	// 계속 초기화 해야할 RECT들 모아둔것
	updateRect();
	saveloadRectSet();
}


//=================================================== 렌 더 ====================================================//

void mapToolScene::render()
{
	//화면 출력(테스트)용 임시 변수
	RECT tmp;
	char str[50];

	for (int i = 0; i < MAP_TILEY * MAP_TILEX; i++)
	{
		if (_tiles[i].rc.left < 0) continue;
		
		IMAGEMANAGER->frameRender("tilemap", getMemDC(), _tiles[i].rc.left, _tiles[i].rc.top,
			_tiles[i].terrainFrameX, _tiles[i].terrainFrameY);

		//sprintf(str, "[ %d, %d ]", _tiles[i].x, _tiles[i].y);
		//TextOut(getMemDC(), _tiles[i].rc.left, _tiles[i].rc.top, str, strlen(str));

		//인게임 맵 오브젝트 렌더
		if (_tiles[i].obj == OBJ_NONE) continue;

		IMAGEMANAGER->frameRender("tilemap", getMemDC(), _tiles[i].rc.left, _tiles[i].rc.top,
				_tiles[i].objFrameX, _tiles[i].objFrameY);
	}

	//맵툴 UI 이미지 렌더
	switch (_menu)
	{
	case MENU_HOUSE:
		IMAGEMANAGER->render("maptool_UI_House", getMemDC(), _rcUIBg.left, _rcUIBg.top);
		break;
	case MENU_TOWN:
		IMAGEMANAGER->render("maptool_UI_Town", getMemDC(),  _rcUIBg.left, _rcUIBg.top);
		break;												 
	case MENU_BOSS:											 
		IMAGEMANAGER->render("maptool_UI_Boss", getMemDC(),  _rcUIBg.left, _rcUIBg.top);
		break;												 
	case MENU_AUTO:											 
		IMAGEMANAGER->render("maptool_UI_Auto", getMemDC(),  _rcUIBg.left, _rcUIBg.top);
		break;												 
	case MENU_CHAR:											 
		IMAGEMANAGER->render("maptool_UI_Char", getMemDC(),  _rcUIBg.left, _rcUIBg.top);
		break;
	}

	//메뉴 변경 아이콘(스크롤)
	IMAGEMANAGER->render("maptool_UI_SCROLLS", getMemDC(), _rcUIBg.left + 100, _rcUIBg.top - 30);
	IMAGEMANAGER->render("tilemap", getMemDC(), _rcMapSpace.left, _rcMapSpace.top, scrollX * 64, scrollY * 64, 704, MAP_HEIGHT);



	// 왼쪽 게임 화면 및 오른쪽 샘플 타일 RECT 출력
	if (INPUT->getToggleKey(VK_F1))
	{
		
		for (int i = 0; i < MAP_TILEY; i++)
		{
			for (int j = 0; j < MAP_TILEX; j++)
			{
				if (_tiles[i * MAP_TILEX + j].obj == OBJ_CARPET || _tiles[i * MAP_TILEX + j].terrain == TR_GROUND)
				{
					FrameRect(getMemDC(), _tiles[i * MAP_TILEX + j].rc, RGB(255, 255, 0));
				}
				else if (_tiles[i * MAP_TILEX + j].terrain == TR_GRASS)
				{
					FrameRect(getMemDC(), _tiles[i * MAP_TILEX + j].rc, RGB(0, 255, 0));
				}
				else if (_tiles[i * MAP_TILEX + j].terrain == TR_WATER)
				{
					FrameRect(getMemDC(), _tiles[i * MAP_TILEX + j].rc, RGB(0, 0, 255));
				}
				else if (_tiles[i * MAP_TILEX + j].obj == OBJ_BLOCKS || _tiles[i * MAP_TILEX + j].terrain == TR_WALL)
				{
					FrameRect(getMemDC(), _tiles[i * MAP_TILEX + j].rc, RGB(255, 0, 0));
				}
		
				//sprintf(str, "[ %d, %d ]", _tiles[i * MAP_TILEX + j].x, _tiles[i * MAP_TILEX + j].y);
				//TextOut(getMemDC(), _tiles[i * MAP_TILEX + j].rc.left, _tiles[i * MAP_TILEX + j].rc.top, str, strlen(str));
			
			}
		}

	}

	//아이콘 RECT(고쳐야함)
	for (int i = 0; i < 5; i++)
	{
		FrameRect(getMemDC(), icon[i], RGB(255, 255, 0));
	}

	//타일 선택하면 그 타일 표시해주는 RECT
	if(upUI) FrameRect(getMemDC(), select_tile, RGB(0, 255, 255));
	 
	//드래그 관련
	if (tilerectOn) FrameRect(getMemDC(), mouseRect, RGB(0, 0, 255));

	// UI 렌더(항상 가장 아래에)
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(getMemDC(), &_rcSaveLoadBg, brush);

	//FrameRect(getMemDC(), _rcUIBg, RGB(255, 0, 0));
	//FrameRect(getMemDC(), _rcSaveBg, RGB(255, 0, 0));


	//UI 버튼 이미지 렌더
	IMAGEMANAGER->findImage("save_button")->render(getMemDC(), _rcSave.left, _rcSave.top);
	IMAGEMANAGER->findImage("load_button")->render(getMemDC(), _rcLoad.left, _rcLoad.top);
	IMAGEMANAGER->findImage("eraser_button")->render(getMemDC(), _rcEraser.left, _rcEraser.top);
	IMAGEMANAGER->findImage("reset_button")->render(getMemDC(), _rcReset.left, _rcReset.top);

	IMAGEMANAGER->findImage("arrow_left")->render(getMemDC(), _rcArrowLeft.left, _rcArrowLeft.top);
	IMAGEMANAGER->findImage("arrow_right")->render(getMemDC(), _rcArrowRight.left, _rcArrowRight.top);

	//OBJ 버튼 (frameImage)
	if(!objectOn) 	IMAGEMANAGER->findImage("obj_button")->frameRender(getMemDC(), _rcObject.left, _rcObject.top, 0, 1);
	else 	IMAGEMANAGER->findImage("obj_button")->frameRender(getMemDC(), _rcObject.left, _rcObject.top, 0, 0);

	//RECT 표시용
	//FrameRect(getMemDC(), IMAGEMANAGER->findImage("maptool_UI_SCROLLS")->boundingBox(), RGB(255, 0, 0));

	//SAVE랑 LOAD누르면 각각 창 나오게
	if(saveOn) IMAGEMANAGER->findImage("saveload_UI")->render(getMemDC(), _rcSaveBg.left, _rcSaveBg.top);
	if(loadOn) IMAGEMANAGER->findImage("saveload_UI")->render(getMemDC(), _rcLoadBg.left, _rcLoadBg.top);

	//SAVE TEXT
	textOut(getMemDC(), _rcSave1.left + 42, _rcSave1.top + 20, "SAVE 1", RGB(255, 255, 0));
	textOut(getMemDC(), _rcSave2.left + 42, _rcSave2.top + 20, "SAVE 2", RGB(255, 255, 0));
	textOut(getMemDC(), _rcSave3.left + 42, _rcSave3.top + 20, "SAVE 3", RGB(255, 255, 0));

	//LOAD TEXT
	textOut(getMemDC(), _rcLoad1.left + 42, _rcLoad1.top + 20, "LOAD 1", RGB(255, 255, 0));
	textOut(getMemDC(), _rcLoad2.left + 42, _rcLoad2.top + 20, "LOAD 2", RGB(255, 255, 0));
	textOut(getMemDC(), _rcLoad3.left + 42, _rcLoad3.top + 20, "LOAD 3", RGB(255, 255, 0));

	//미니맵 관련
	FrameRect(getMemDC(), _rcMinimap, RGB(255, 255, 0));
	FrameRect(getMemDC(), _rcMinimap_C, RGB(255, 0, 0));

	//드래그 부분 표시
	for (int i = 0; i < SAMPLE_TILEY; i++)
	{
		for (int j = 0; j < SAMPLE_TILEX; j++)
		{
			if (IntersectRect(&tmp, &_sampleTile[i][j].rc, &mouseRect))
			{
				IMAGEMANAGER->alphaRender("select_rect", getMemDC(), _sampleTile[i][j].rc.left, _sampleTile[i][j].rc.top, 50);
				if (!dragOnSample)
				{
					dragOnSample = true;
					_dragIndexStartX = j;
					_dragIndexStartY = i;
				}
				else
				{
					_dragRectWidth = abs(mouseRect.right - mouseRect.left);
					_dragRectHeight = abs(mouseRect.bottom - mouseRect.top);

					_dragIndexEndX = j + _dragRectWidth / TILESIZE -1 ;
					_dragIndexEndY = i + _dragRectHeight / TILESIZE -1;
				}
				
			}
		}
	}

	//if (dragOnSample)
	//{
	//	IMAGEMANAGER->alphaRender("tilemap", getMemDC(), _ptMouse.x, _ptMouse.y,
	//		_sampleTile[_dragIndexStartX][_dragIndexStartY].terrainFrameX, _sampleTile[_dragIndexStartX][_dragIndexStartY].terrainFrameY,
	//		_sampleTile[_dragIndexEndX][_dragIndexEndY].terrainFrameX, _sampleTile[_dragIndexEndX][_dragIndexEndY].terrainFrameY, 100);
	//}

	//표시용 TEXT
	SetTextColor(getMemDC(), RGB(255, 0, 0));
	sprintf_s(str, "startX: %d, startY: %d", _dragIndexStartX, _dragIndexStartY);
	TextOut(getMemDC(), 0, 20, str, strlen(str));
	sprintf_s(str, "endX: %d, endY: %d", _dragIndexEndX, _dragIndexEndY);
	TextOut(getMemDC(), 0, 40, str, strlen(str));
	sprintf_s(str, "Width: %d, Height: %d", _dragRectWidth, _dragRectHeight);
	TextOut(getMemDC(), 0, 60, str, strlen(str));


}

// 맵툴 세팅
void mapToolScene::mapToolSetup()
{

	//인게임 화면 RECT 초기화
	for (int i = 0; i < MAP_TILEY; i++)
	{
		for (int j = 0; j < MAP_TILEX; j++)
		{
			//_tiles[i * MAP_TILEX + j].rc = RectMake(j * TILESIZE, i * TILESIZE, TILESIZE, TILESIZE);
			_tiles[i * MAP_TILEX + j].x = j;
			_tiles[i * MAP_TILEX + j].y = i;
			_tiles[i * MAP_TILEX + j].terrainFrameX = 0;
			_tiles[i * MAP_TILEX + j].terrainFrameY = 0;
			_tiles[i * MAP_TILEX + j].objFrameX = 0;
			_tiles[i * MAP_TILEX + j].objFrameY = 0;
			_tiles[i * MAP_TILEX + j].terrain = terrainSelect(_tiles[i * MAP_TILEX + j].terrainFrameX, _tiles[i * MAP_TILEX + j].terrainFrameY);
			_tiles[i * MAP_TILEX + j].obj = OBJ_NONE;
		}
	}

	//==================================== 맵툴 RECT 초기화 ========================================//

	_BG = RectMake(0, 0, TILESIZEX, TILESIZEY);

	//배경 카메라 RECT 초기화(전체맵 정중앙)
	_rcMCamera.x = TILESIZEX / 2;
	_rcMCamera.y = TILESIZEY / 2;
	_rcMCamera.rc = RectMakeCenter(_rcMCamera.x, _rcMCamera.y, WINSIZEX, WINSIZEY);

	// 맵툴 UI의 RECT 초기화
	_rcUIBg = RectMakeCenter(WINSIZEX / 2, WINSIZEY + 150, 904, 400);
	_rcSaveLoadBg = RectMake(0, 0, WINSIZEX, 50);

	//하우스 타일맵 Rect 초기화
	_rcMapSpace = RectMake(_rcUIBg.left + 100, _rcUIBg.top + 75, 704, MAP_HEIGHT);

	//오브젝트 맵 켜는 버튼
	_rcObject = RectMake(_rcUIBg.right - 45, _rcUIBg.top + 50, 60, 30);
	
	//화살표 RECT
	_rcArrowLeft = RectMake(_rcUIBg.left + 90, _rcUIBg.bottom - 65, 40, 20);
	_rcArrowRight = RectMake(_rcUIBg.right - 130, _rcUIBg.bottom - 65, 40, 20);

	//미니맵 렉트
	_rcMinimap = RectMake(20, 20, TILESIZEX / 20, TILESIZEY / 20);
	_rcMinimap_C = RectMake(_rcMCamera.rc.left / 20, _rcMCamera.rc.top / 20, WINSIZEX / 20, WINSIZEY / 20);

	//아이콘 렉트 초기화
	for (int i = 0; i < 5; i++)
	{
		icon[i] = RectMakeCenter(125 + _rcUIBg.left + (50 * i), _rcUIBg.top, 50, 50);
	}

	// 컨트롤 버튼 RECT 위치 초기화
	_rcSave = RectMake(WINSIZEX - 210 , 5, 100, 40);
	_rcLoad = RectMake(WINSIZEX - 105,  5, 100, 40);
	_rcEraser = RectMake(WINSIZEX - 315, 5, 100, 40);
	_rcReset = RectMake(WINSIZEX - 420, 5, 100, 40);

	if (tilerectOn)
	{
		sprintf_s(str, "[%d, %d]", mouseT.x, mouseT.y);
		TextOut(getMemDC(), _ptMouse.x, _ptMouse.y, str, strlen(str));
		
		TextOut(getMemDC(), 50, 50, "ON", RGB(0, 255, 0));
	}
	else
	{
		sprintf_s(str, "[%d, %d]", mouseT.x, mouseT.y);
		TextOut(getMemDC(), _ptMouse.x, _ptMouse.y, str, strlen(str));

		TextOut(getMemDC(), 50, 50, "OFF", RGB(255, 0, 0));
	}
	
}

void mapToolScene::setMap()
{
	//이것은 샘플타일의 맵선택
	for (int i = 0; i < SAMPLE_TILEY; i++)
	{
		for (int j = 0; j < SAMPLE_TILEX; j++)
		{
			if (PtInRect(&_sampleTile[i][j].rc, _ptMouse))
			{
				if (eraseOn || saveOn || loadOn)
				{
					eraseOn = false;  saveOn = false; loadOn = false;
				}

				_currentTile.x = _sampleTile[i][j].terrainFrameX;
				_currentTile.y = _sampleTile[i][j].terrainFrameY;
				currentCtrl = _ctrlSelect;
				select_tile = RectMake(_sampleTile[i][j].rc.left, _sampleTile[i][j].rc.top, 64, 64);
				break;
			}
		}
	}
	
	// 인게임 화면과 RECT 간의 충돌 처리
	for (int i = 0; i < MAP_TILEY * MAP_TILEX; i++)
	{
		if (!(PtInRect(&_rcUIBg, _ptMouse) || PtInRect(&_rcSaveLoadBg, _ptMouse)
			|| PtInRect(&IMAGEMANAGER->findImage("maptool_UI_SCROLLS")->boundingBox(), _ptMouse)
			|| PtInRect(&_rcSaveBg, _ptMouse) || PtInRect(&_rcLoadBg, _ptMouse)))
		{
			if (PtInRect(&_tiles[i].rc, _ptMouse))
			{
				if (_ctrlSelect == CTRL_ERASER)
				{
					_tiles[i].objFrameX = 0;
					_tiles[i].objFrameY = 0;
					_tiles[i].obj = OBJ_NONE;
				}
				else
				{
					// 현재 버튼에 따라 타일 생성
					if (currentCtrl == CTRL_TERRAIN)
					{
						_tiles[i].terrainFrameX = _currentTile.x;
						_tiles[i].terrainFrameY = _currentTile.y;
						_tiles[i].terrain = terrainSelect(_currentTile.x, _currentTile.y);
					}

					if (currentCtrl == CTRL_OBJECT)
					{
						_tiles[i].objFrameX = _currentTile.x;
						_tiles[i].objFrameY = _currentTile.y;
						_tiles[i].obj = objectSelect(_currentTile.x, _currentTile.y);
					}
				}
			}
		}
	}
}
void mapToolScene::save()
{
	HANDLE file;
	DWORD write;

	switch (_ctrlSelect)
	{
	case CTRL_MAP1:
		// 파일 세이브
		file = CreateFile("save1.map", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(file, _tiles, sizeof(tagTile) * MAP_TILEX * MAP_TILEY, &write, NULL);
		CloseHandle(file);

		TextOut(getMemDC(), WINSIZEX/2, 100, "SAVE COMPLETE", RGB(0, 255, 0));
		break;
	case CTRL_MAP2:
		// 파일 세이브
		file = CreateFile("save2.map", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(file, _tiles, sizeof(tagTile) * MAP_TILEX * MAP_TILEY, &write, NULL);
		CloseHandle(file);

		TextOut(getMemDC(), WINSIZEX / 2, 100, "SAVE COMPLETE", RGB(0, 255, 0));
		break;
	case CTRL_MAP3:
		// 파일 세이브
		file = CreateFile("save3.map", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(file, _tiles, sizeof(tagTile) * MAP_TILEX * MAP_TILEY, &write, NULL);
		CloseHandle(file);

		TextOut(getMemDC(), WINSIZEX / 2, 100, "SAVE COMPLETE", RGB(0, 255, 0));
		break;
	}
	
}

void mapToolScene::load()
{
	HANDLE file;
	DWORD read;

	switch (_ctrlSelect)
	{
	case CTRL_MAP1:
		// 파일 로드
		file = CreateFile("save1.map", GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		ReadFile(file, _tiles, sizeof(tagTile) * MAP_TILEX * MAP_TILEY, &read, NULL);
		CloseHandle(file);

		TextOut(getMemDC(), WINSIZEX / 2, 100, "LOAD COMPLETE", RGB(255, 0, 0));
		break;
	case CTRL_MAP2:
		// 파일 로드
		file = CreateFile("save2.map", GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		ReadFile(file, _tiles, sizeof(tagTile) * MAP_TILEX * MAP_TILEY, &read, NULL);
		CloseHandle(file);

		TextOut(getMemDC(), WINSIZEX / 2, 100, "LOAD COMPLETE", RGB(255, 0, 0));
		break;
	case CTRL_MAP3:
		// 파일 로드
		file = CreateFile("save3.map", GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		ReadFile(file, _tiles, sizeof(tagTile) * MAP_TILEX * MAP_TILEY, &read, NULL);
		CloseHandle(file);

		TextOut(getMemDC(), WINSIZEX / 2, 100, "LOAD COMPLETE", RGB(255, 0, 0));
		break;
	}
}

//====================================== 업 뎃 렉 트 =============================================//

void mapToolScene::updateRect()
{
	//타일 렉트 업뎃
	for (int i = 0; i < MAP_TILEY; i++)
	{
		for (int j = 0; j < MAP_TILEX ; j++)
		{ // 32, 35
			if ((i + camY) * MAP_TILEX + (j + camX) >= MAP_TILEX * MAP_TILEY) break;
			_tiles[(i + camY) * MAP_TILEX + (j + camX)].rc = RectMake(j * TILESIZE, i * TILESIZE, TILESIZE, TILESIZE);
			
		}
	}


	// 아이콘 렉트 초기화
	for (int i = 0; i < 5; i++)
	{
		icon[i] = RectMakeCenter(125 + _rcUIBg.left + (50 * i), _rcUIBg.top, 50, 50);
	}

	IMAGEMANAGER->findImage("maptool_UI_SCROLLS")->setX(icon[0].left);
	IMAGEMANAGER->findImage("maptool_UI_SCROLLS")->setY(icon[0].top);

	_rcMCamera.rc = RectMakeCenter(_rcMCamera.x, _rcMCamera.y, WINSIZEX, WINSIZEY);
	_rcMinimap_C = RectMake( 20 + _rcMCamera.rc.left / 20, 20 + _rcMCamera.rc.top / 20, WINSIZEX / 20, WINSIZEY / 20);

	_rcMapSpace = RectMake(_rcUIBg.left + 100, _rcUIBg.top + 75, 704, MAP_HEIGHT);
	_rcArrowLeft = RectMake(_rcUIBg.left + 90, _rcUIBg.bottom - 65, 40, 20);
	_rcArrowRight = RectMake(_rcUIBg.right - 130, _rcUIBg.bottom - 65, 40, 20);
	_rcObject = RectMake(_rcUIBg.right - 45, _rcUIBg.top + 50, 60, 30);

}

void mapToolScene::reset()
{
	// 왼쪽 인게임 화면 모두 잔디가 기본 타일이 되도록 세팅
	for (int i = 0; i < MAP_TILEY; i++)
	{
		for (int j = 0; j < MAP_TILEX; j++)
		{
			_tiles[i * MAP_TILEX + j].terrainFrameX = 0;
			_tiles[i * MAP_TILEX + j].terrainFrameY = 0;
			_tiles[i * MAP_TILEX + j].objFrameX = 0;
			_tiles[i * MAP_TILEX + j].objFrameY = 0;
			_tiles[i * MAP_TILEX + j].terrain = terrainSelect(_tiles[i * MAP_TILEX + j].terrainFrameX, _tiles[i * MAP_TILEX + j].terrainFrameY);
			_tiles[i * MAP_TILEX + j].obj = OBJ_NONE;
		}
	}
}


// 지형 세팅 ( 아 직 안 함 )..
TERRAIN mapToolScene::terrainSelect(int frameX, int frameY)
{
	// 벽(집안)
	if (frameX >= 3 && frameY < 4)
	{
		return TR_WALL;
	}
	// 잔디
	if ((frameX < 20 && frameX >= 5 && frameY < 8 && frameY >= 4) || 
		(frameX < 5 && frameY == 7) || (frameX < 5 && frameY < 12 && frameY >= 8))
	{
		return TR_GRASS;
	}
	// 물
	if ((frameX >= 27 && frameY < 8 && frameY >= 4) || 
		(frameX >= 22 && frameX < 24 && frameY >= 4 && frameY < 6) ||
		(frameX >= 24 && frameX < 27 && frameY >= 4 && frameY < 7))
	{
		return TR_WATER;
	}

	// 그 이외일 경우 땅으로 기본값 설정
	return TR_GROUND;
}
// 오브젝트 세팅
OBJECT mapToolScene::objectSelect(int frameX, int frameY)
{
	//// 시멘트
	if (frameX <= 28 && frameY >= 12 && frameY < 16)
	{
		return OBJ_CARPET;
	}
	return OBJ_BLOCKS;
}

//input(feat. ptInRect) 모음집 ==> 그냥 update에 ptInRect 추가하면 다른거 작동 안됩니다...
void mapToolScene::input()
{
	// 오브젝트 지우기(ERASE 버튼 클릭해도 됨)
	if (INPUT->getKeyDown('E'))
	{
		eraseOn = true;
	}
	// 타일 지우기
	if (INPUT->getKeyDown('R'))
	{
		_currentTile.x = 0;
		_currentTile.y = 0;
		currentCtrl = CTRL_TERRAIN;
	}
	//LBUTTON 입력 관련
	if (INPUT->getKeyDown(VK_LBUTTON))
	{
		if (PtInRect(&_rcObject, _ptMouse))
		{
			if (!objectOn)
			{
				objectOn = true;
				switch (_menu)
				{
				case MENU_HOUSE:
					scrollY = 12;
					break;
				case MENU_TOWN:
					scrollY = 16;
					break;
				case MENU_BOSS:
					scrollY = 20;
					break;
				case MENU_AUTO:
					break;
				case MENU_CHAR:
					break;
				}
			}
			else
			{
				objectOn = false;
				switch (_menu)
				{
				case MENU_HOUSE:
					scrollX = 0;
					scrollY = 0;
					break;
				case MENU_TOWN:
					scrollX = 0;
					scrollY = 4;
					break;
				case MENU_BOSS:
					scrollX = 0;
					scrollY = 8;
					break;
				case MENU_AUTO:
					break;
				case MENU_CHAR:
					break;
				}
			}
		}

		// 스크롤 X, 스크롤 Y (아직 제대로 예외처리 안함)
		if (PtInRect(&_rcArrowLeft, _ptMouse))
		{
			if (scrollX > 0)
			{
				scrollX--;
			}
		}
		else if (PtInRect(&_rcArrowRight, _ptMouse))
		{
			if (scrollY <= 11)
			{
				if (scrollX < 21)
					scrollX++;
			}
			else
			{
				if (scrollY < 16)
				{
					if (scrollX < 25)
						scrollX++;
				}
				else
				{
					if (scrollX < 36)
						scrollX++;
				}
			}
		}
		 
		// 위쪽 UI 입력 처리
		if (PtInRect(&_rcSave, _ptMouse))
		{
			if (!saveOn) saveOn = true;
			else saveOn = false;
		}
		else if (PtInRect(&_rcLoad, _ptMouse))
		{
			if (!loadOn) loadOn = true;
			else loadOn = false;
		}
		else if (PtInRect(&_rcEraser, _ptMouse))
		{
			eraseOn = true;
		}
		else if (PtInRect(&_rcReset, _ptMouse))
		{
			this->reset();
		}

		if (saveOn || loadOn)
		{
			//SAVE / LOAD 
			if (PtInRect(&_rcSave1, _ptMouse) || PtInRect(&_rcLoad1, _ptMouse))
			{
				_ctrlSelect = CTRL_MAP1;
			}
			else if (PtInRect(&_rcSave2, _ptMouse) || PtInRect(&_rcLoad2, _ptMouse))
			{
				_ctrlSelect = CTRL_MAP2;
			}
			else if (PtInRect(&_rcSave3, _ptMouse) || PtInRect(&_rcLoad3, _ptMouse))
			{
				_ctrlSelect = CTRL_MAP3;
			}

			if(saveOn) this->save();
			else this->load();
		}

		if (PtInRect(&icon[0], _ptMouse))
		{
			scrollX = 0;
			scrollY = 0;
			_menu = MENU_HOUSE;
			if (!upUI)
				upUI = true;
			if(objectOn)
				objectOn = false;
		}
		else if (PtInRect(&icon[1], _ptMouse))
		{
			scrollX = 0;
			scrollY = 4;
			_menu = MENU_TOWN;
			if (!upUI)
				upUI = true;
			if (objectOn)
				objectOn = false;
		}
		else if (PtInRect(&icon[2], _ptMouse))
		{
			scrollX = 0;
			scrollY = 8;
			_menu = MENU_BOSS;
			if (!upUI)
				upUI = true;
			if (objectOn)
				objectOn = false;
		}
		else if (PtInRect(&icon[3], _ptMouse))
		{
			scrollX = 0;
			_menu = MENU_AUTO;
			if (!upUI)
				upUI = true;
			if (objectOn)
				objectOn = false;
		}
		else if (PtInRect(&icon[4], _ptMouse))
		{
			scrollX = 0;
			_menu = MENU_CHAR;
			if (!upUI)
				upUI = true;
			if (objectOn)
				objectOn = false;
		}
		
	}

	//드래그
	if (INPUT->getKey(VK_RBUTTON))
	{
		mouseRect = RectMake(0, 0, 0, 0);
		if (!tilerectOn)
		{
			tilerectOn = true;
			mouseT.x = _ptMouse.x;
			mouseT.y = _ptMouse.y;
		}
		else if (tilerectOn)
		{
			if (_ptMouse.y > mouseT.y)
			{
				mouseRect = RectMake(mouseT.x, mouseT.y, _ptMouse.x - mouseT.x, _ptMouse.y - mouseT.y);
			}
			else
			{
				mouseRect = RectMake(_ptMouse.x, _ptMouse.y, mouseT.x - _ptMouse.x,mouseT.y - _ptMouse.y);
			}
		}
	}
	if (INPUT->getKeyUp(VK_RBUTTON))
	{
		tilerectOn = false;
		
	}

	if (INPUT->getKeyDown(VK_RETURN))
	{
		SCENEMANAGER->loadScene("inGameScene");
	}
}

// SAMPLETILE SETUP
void mapToolScene::setSampleTile(int indexX, int indexY)
{
	//샘플 타일 RECT 초기화
	for (int i = 0; i < TILEMAPSIZEY; i++)
	{
		for (int j = 0; j < TILEMAPSIZEX; j++)
		{
			_sampleTile[i][j].rc = RectMake(_rcMapSpace.left + j * TILESIZE, _rcMapSpace.top + i * TILESIZE, TILESIZE, TILESIZE);

			_sampleTile[i][j].terrainFrameX = j + scrollX;
			_sampleTile[i][j].terrainFrameY = i + scrollY;
			_sampleTile[i][j].indexX = indexX;
			_sampleTile[i][j].indexY = indexY;
		}
	}
}

void mapToolScene::saveloadRectSet()
{
	//세이브 창 켜기
	if (saveOn)
	{
		if (loadOn) loadOn = false;
		_rcSaveBg = RectMake(_rcSave.left - 25, _rcSaveLoadBg.bottom + 10, 150, 190);
		_rcSave1 = RectMake(_rcSaveBg.left + 10, _rcSaveBg.top + 10, 130, 55);
		_rcSave2 = RectMake(_rcSaveBg.left + 10, _rcSave1.bottom + 3, 130, 55);
		_rcSave3 = RectMake(_rcSaveBg.left + 10, _rcSave2.bottom + 3, 130, 55);
	}
	else
	{
		_rcSaveBg = RectMake(_rcSave.left - 25, -200, 150, 190);
		_rcSave1 = RectMake(_rcSaveBg.left + 10, _rcSaveBg.top + 10, 130, 55);
		_rcSave2 = RectMake(_rcSaveBg.left + 10, _rcSave1.bottom + 3, 130, 55);
		_rcSave3 = RectMake(_rcSaveBg.left + 10, _rcSave2.bottom + 3, 130, 55);
	}
	//세이브 창 끄기
	if (loadOn)
	{
		if (saveOn) saveOn = false;
		_rcLoadBg = RectMake(_rcLoad.left - 50, _rcSaveLoadBg.bottom + 10, 150, 190);
		_rcLoad1 = RectMake(_rcLoadBg.left + 10, _rcLoadBg.top + 10, 130, 55);
		_rcLoad2 = RectMake(_rcLoadBg.left + 10, _rcLoad1.bottom + 3, 130, 55);
		_rcLoad3 = RectMake(_rcLoadBg.left + 10, _rcLoad2.bottom + 3, 130, 55);
	}
	else
	{
		_rcLoadBg = RectMake(_rcLoad.left - 50, -200, 150, 190);
		_rcLoad1 = RectMake(_rcLoadBg.left + 10, _rcLoadBg.top + 10, 130, 55);
		_rcLoad2 = RectMake(_rcLoadBg.left + 10, _rcLoad1.bottom + 3, 130, 55);
		_rcLoad3 = RectMake(_rcLoadBg.left + 10, _rcLoad2.bottom + 3, 130, 55);
	}
}

//맵툴 UI 팝업 애니메이션
void mapToolScene::mapUiAnimation()
{
	if (upUI)
	{
		if (_rcUIBg.top > WINSIZEY / 2 + 80)
		{
			_rcUIBg.top -= 10;
			tileSelect = true;
		}
		_rcUIBg.bottom = _rcUIBg.top + 400;
	}
	else
	{
		if (_rcUIBg.top < WINSIZEY - 50)
		{
			_rcUIBg.top += 10;
			tileSelect = false;
			_rcUIBg.bottom = _rcUIBg.top + 906;
		}
	}


}
