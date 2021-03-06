#include "stdafx.h"
#include "mushroom.h"
#include "player.h"

HRESULT mushroom::init()
{
	//버섯 이미지 삽입
	_room = IMAGEMANAGER->addFrameImage("mushroom", "Images/Enemy/Mushroom/버섯괴물 2048 + 128 _ X16 Y1.bmp", 2048, 128, 16, 1, true, RGB(255, 0, 255));
	_destroy4 = IMAGEMANAGER->addFrameImage("destroy4", "Images/Enemy/Mushroom/버섯파괴 1280 + 64 _ X10 Y1.bmp", 1280, 64, 10, 1, true, RGB(255, 0, 255));
	_getAtk = IMAGEMANAGER->addFrameImage("roomGetAtk", "Images/Enemy/Mushroom/버섯피격 384 + 512 _ X3 Y4.bmp", 384, 512, 3, 4, true, RGB(255, 0, 255));

	//버섯 구조체 초기화
	//for (int i = 0; i < _octopus.size(); i++)
	//{
	//	//문어렉트 위치 랜덤 생성
	//	_octopus[i].x = RANDOM->Range(0, WINSIZEX - 80);
	//	_octopus[i].y = RANDOM->Range(0, WINSIZEY - 64);
	//	_octopus[i]._hp = MINION_HP;
	//	_octopus[i]._angle = PI_2;
	//	_octopus[i]._speed = 3.0f;
	//	_octopus[i].frameX = 0;
	//	_octopus[i].frameY = RANDOM->Range(octo_DOWN,octo_UP);
	//	_octopus[i]._state = RANDOM->Range(0, 3);
	//	_octopus[i].destroyX = _octopus[i].destroyY = 0;
	//}

	_missile = new missile;
	_missile->init(30, 500);

	return S_OK;
}

void mushroom::release()
{
	_missile->release();
	SAFE_DELETE(_missile);
}

void mushroom::update()
{
	_missile->update();

	if (INPUT->getKeyDown(VK_RBUTTON))
	{
		tagMushroom mushroom;
		mushroom.x = RANDOM->Range(0, WINSIZEX - 128);
		mushroom.y = RANDOM->Range(0, WINSIZEY - 128);
		mushroom._rc = RectMakeCenter(mushroom.x, mushroom.y + 30, 60, 60);		// 이미지 여백이 꽤 커서 렉트를 줄임;
		mushroom._hp = MUSHROOM_HP;
		mushroom._angle = PI_2;
		mushroom._speed = 1.0f;
		mushroom.frameX = mushroom.frameY = 0;
		mushroom.hitFrameX = mushroom.hitFrameY = 0;
		mushroom._state = RANDOM->Range(0, 3);
		mushroom._direction = RANDOM->Range(0, 1);
		mushroom.destroyX = mushroom.destroyY = 0;
		mushroom._count = 0;
		mushroom._dead = 0;
		_mushroom.push_back(mushroom);
	}

	
	for (int i = 0; i < _mushroom.size(); i++)
	{
		//_mushroom[i]._hp -= 1;															//죽었을때 모션 확인
		
		if (_mushroom[i].x - 64 < 0)
		{	
			_mushroom[i].x = 64;
		}	
		if (_mushroom[i].x + 64 > WINSIZEX)
		{	
			_mushroom[i].x = WINSIZEX - 64;
		}	
		if (_mushroom[i].y - 64 < 0)
		{	
			_mushroom[i].y = 64;
		}	
		if (_mushroom[i].y + 64 > WINSIZEY)
		{	
			_mushroom[i].y = WINSIZEY - 64;
		}

		if (_mushroom[i].frameX < 8 || _mushroom[i].frameX > 13)
		{
			_mushroom[i].x += cosf(_mushroom[i]._angle) * _mushroom[i]._speed;
			_mushroom[i].y += -sinf(_mushroom[i]._angle) * _mushroom[i]._speed;
		}
		_mushroom[i]._count++;


		//버섯이 살아있을 때
		if (_mushroom[i]._hp > 0)
		{
			_mushroom[i]._rc = RectMakeCenter(_mushroom[i].x, _mushroom[i].y + 30, 60, 60);	//렉트 위치 이미지기준 업데이트
			//버섯방향 (아래쪽)
			if (_mushroom[i]._state == MUSH_DOWN)
			{
				if (!_mushroom[i]._direction)			//버섯 이동각도
				{
					_mushroom[i]._angle += 0.03f;
				}
				else if (_mushroom[i]._direction)
				{
					_mushroom[i]._angle -= 0.03f;
				}
				if (_mushroom[i]._count % 20 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].frameX++;
				}
				if (_mushroom[i].frameX > 15)			//버섯 방향 초기화
				{
					_mushroom[i].frameX = 0;
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);
					if (_mushroom[i]._state == MUSH_DOWN)
					{
						_mushroom[i]._angle = -(PI_2);
					}
					else if (_mushroom[i]._state == MUSH_LEFT)
					{
						_mushroom[i]._angle = (PI);
					}
					else if (_mushroom[i]._state == MUSH_RIGHT)
					{
						_mushroom[i]._angle = (PI2);
					}
					else if (_mushroom[i]._state == MUSH_UP)
					{
						_mushroom[i]._angle = (PI_2);
					}
					_mushroom[i]._count = RANDOM->Range(0, 30);
					_mushroom[i]._direction = RANDOM->Range(0, 1);
				}
			}
			//버섯방향 (왼쪽)
			if (_mushroom[i]._state == MUSH_LEFT)
			{
				if (!_mushroom[i]._direction)			//버섯 이동각도
				{
					_mushroom[i]._angle += 0.03f;
				}
				else if (_mushroom[i]._direction)
				{
					_mushroom[i]._angle -= 0.03f;
				}
				if (_mushroom[i]._count % 20 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].frameX++;
				}
				if (_mushroom[i].frameX > 15)			//버섯 방향 초기화
				{
					_mushroom[i].frameX = 0;
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);
					if (_mushroom[i]._state == MUSH_DOWN)
					{
						_mushroom[i]._angle = -(PI_2);
					}
					else if (_mushroom[i]._state == MUSH_LEFT)
					{
						_mushroom[i]._angle = (PI);
					}
					else if (_mushroom[i]._state == MUSH_RIGHT)
					{
						_mushroom[i]._angle = (PI2);
					}
					else if (_mushroom[i]._state == MUSH_UP)
					{
						_mushroom[i]._angle = (PI_2);
					}
					_mushroom[i]._count = RANDOM->Range(0, 30);
					_mushroom[i]._direction = RANDOM->Range(0, 1);
				}
			}
			//버섯방향 (오른쪽)
			if (_mushroom[i]._state == MUSH_RIGHT)
			{
				if (!_mushroom[i]._direction)			//버섯 이동각도
				{
					_mushroom[i]._angle += 0.03f;
				}
				else if (_mushroom[i]._direction)
				{
					_mushroom[i]._angle -= 0.03f;
				}
				if (_mushroom[i]._count % 20 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].frameX++;
				}
				if (_mushroom[i].frameX > 15)			//버섯 방향 초기화
				{
					_mushroom[i].frameX = 0;
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);
					if (_mushroom[i]._state == MUSH_DOWN)
					{
						_mushroom[i]._angle = -(PI_2);
					}
					else if (_mushroom[i]._state == MUSH_LEFT)
					{
						_mushroom[i]._angle = (PI);
					}
					else if (_mushroom[i]._state == MUSH_RIGHT)
					{
						_mushroom[i]._angle = (PI2);
					}
					else if (_mushroom[i]._state == MUSH_UP)
					{
						_mushroom[i]._angle = (PI_2);
					}
					_mushroom[i]._count = RANDOM->Range(0, 30);
					_mushroom[i]._direction = RANDOM->Range(0, 1);
				}
			}
			//버섯방향 (위쪽)
			if (_mushroom[i]._state == MUSH_UP)
			{
				if (!_mushroom[i]._direction)			//버섯 이동각도
				{
					_mushroom[i]._angle += 0.03f;
				}
				else if (_mushroom[i]._direction)
				{
					_mushroom[i]._angle -= 0.03f;
				}
				if (_mushroom[i]._count % 20 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].frameX++;
				}
				if (_mushroom[i].frameX > 15)			//버섯 방향 초기화
				{
					_mushroom[i].frameX = 0;
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);
					if (_mushroom[i]._state == MUSH_DOWN)
					{
						_mushroom[i]._angle = -(PI_2);
					}
					else if (_mushroom[i]._state == MUSH_LEFT)
					{
						_mushroom[i]._angle = (PI);
					}
					else if (_mushroom[i]._state == MUSH_RIGHT)
					{
						_mushroom[i]._angle = (PI2);
					}
					else if (_mushroom[i]._state == MUSH_UP)
					{
						_mushroom[i]._angle = (PI_2);
					}
					_mushroom[i]._count = RANDOM->Range(0, 30);
					_mushroom[i]._direction = RANDOM->Range(0, 1);
				}
			}
			//버섯 피격
			// 위에서 타격
			if (_mushroom[i]._state == MUSH_ATKDOWN)
			{
				_mushroom[i].hitFrameY = 0;
				if (_mushroom[i]._count % 10 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].hitFrameX++;
				}
				if (_mushroom[i].hitFrameX > 2)			//버섯 상태 초기화
				{
					_mushroom[i].hitFrameX = 0;
					_mushroom[i]._count = RANDOM->Range(0, 30);					// 움직임 카
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);	// 방향
					_mushroom[i]._direction = RANDOM->Range(0, 1);				// 방향
					_mushroom[i].frameX = 0;									// 동작
				}
			}
			// 왼쪽에서 타격
			else if (_mushroom[i]._state == MUSH_ATKLEFT)
			{
				_mushroom[i].hitFrameY = 1;
				if (_mushroom[i]._count % 10 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].hitFrameX++;
				}
				if (_mushroom[i].hitFrameX > 2)			//버섯 상태 초기화
				{
					_mushroom[i].hitFrameX = 0;
					_mushroom[i]._count = RANDOM->Range(0, 30);					// 움직임 카운트
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);	// 방향
					_mushroom[i]._direction = RANDOM->Range(0, 1);				// 방향
					_mushroom[i].frameX = 0;									// 동작
				}
			}
			// 오른쪽에서 타격
			else if (_mushroom[i]._state == MUSH_ATKRIGHT)
			{
				_mushroom[i].hitFrameY = 2;
				if (_mushroom[i]._count % 10 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].hitFrameX++;
				}
				if (_mushroom[i].hitFrameX > 2)			//버섯 상태 초기화
				{
					_mushroom[i].hitFrameX = 0;
					_mushroom[i]._count = RANDOM->Range(0, 30);					// 움직임 카
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);	// 방향
					_mushroom[i]._direction = RANDOM->Range(0, 1);				// 방향
					_mushroom[i].frameX = 0;									// 동작
				}
			}
			// 아래쪽에서 타격
			else if (_mushroom[i]._state == MUSH_ATKUP)
			{
				_mushroom[i].hitFrameY = 3;
				if (_mushroom[i]._count % 10 == 0)		//버섯 프레임카운트
				{
					_mushroom[i].hitFrameX++;
				}
				if (_mushroom[i].hitFrameX > 2)			//버섯 상태 초기화
				{
					_mushroom[i].hitFrameX = 0;
					_mushroom[i]._count = RANDOM->Range(0, 30);					// 움직임 카
					_mushroom[i]._state = RANDOM->Range(MUSH_DOWN, MUSH_UP);	// 방향
					_mushroom[i]._direction = RANDOM->Range(0, 1);				// 방향
					_mushroom[i].frameX = 0;									// 동작
				}
			}

			//플레이어 공격과 충돌
			RECT temp;
			if (IntersectRect(&temp, &_playerAdress->getAttack(), &_mushroom[i]._rc))
			{
				if (_playerAdress->getAttack().right < _mushroom[i].x && _playerAdress->getAttack().bottom > _mushroom[i]._rc.top&& _playerAdress->getAttack().top < _mushroom[i]._rc.bottom)
				{
					_mushroom[i].x += 30;
					_mushroom[i]._state = MUSH_ATKLEFT;
				}
				else if (_playerAdress->getAttack().left > _mushroom[i].x&& _playerAdress->getAttack().bottom > _mushroom[i]._rc.top&& _playerAdress->getAttack().top < _mushroom[i]._rc.bottom)
				{
					_mushroom[i].x -= 30;
					_mushroom[i]._state = MUSH_ATKRIGHT;
				}
				else if (_playerAdress->getAttack().top < _mushroom[i].y && _playerAdress->getAttack().right > _mushroom[i]._rc.left&& _playerAdress->getAttack().left < _mushroom[i]._rc.right)
				{
					_mushroom[i].y += 30;
					_mushroom[i]._state = MUSH_ATKUP;
				}
				else if (_playerAdress->getAttack().bottom > _mushroom[i].y&& _playerAdress->getAttack().right > _mushroom[i]._rc.left&& _playerAdress->getAttack().left < _mushroom[i]._rc.right)
				{
					_mushroom[i].y -= 30;
					_mushroom[i]._state = MUSH_ATKDOWN;
				}
				_mushroom[i]._hp -= 100;			//플레이어 공격력이어야함
			}
		}
	}

	for (int i = 0; i < _mushroom.size(); i++)
	{
		if (_mushroom[i]._hp <= 0)
		{
			_mushroom[i]._hp = 0;
			_mushroom[i]._dead++;
			_mushroom[i]._rc = RectMakeCenter(_mushroom[i].x, _mushroom[i].y, 0, 0);
			if (_mushroom[i]._dead % 10 == 0)
			{
				_mushroom[i].destroyX++;
			}
			if (_mushroom[i].destroyX > 8)
			{
				//_mushroom[i]._hp = 30;
				_mushroom.erase(_mushroom.begin() + i);
			}
		}
	}
}

void mushroom::render()
{
	_missile->render();
	for (int i = 0; i < _mushroom.size(); i++)
	{
		Rectangle(getMemDC(), _mushroom[i]._rc);		//충돌판정 렉트위치 확인용

		if (_mushroom[i]._hp > 0)
		{
			if (_mushroom[i]._state >= MUSH_DOWN && _mushroom[i]._state <= MUSH_UP)
			{
				_room->frameRender(getMemDC(), _mushroom[i].x - 64, _mushroom[i].y - 64, _mushroom[i].frameX, 0);
			}
			else if (_mushroom[i]._state == MUSH_ATKDOWN)
			{
				_getAtk->frameRender(getMemDC(), _mushroom[i].x - 64, _mushroom[i].y - 64, _mushroom[i].hitFrameX, _mushroom[i].hitFrameY);
			}
			else if (_mushroom[i]._state == MUSH_ATKLEFT)
			{
				_getAtk->frameRender(getMemDC(), _mushroom[i].x - 64, _mushroom[i].y - 64, _mushroom[i].hitFrameX, _mushroom[i].hitFrameY);
			}
			else if (_mushroom[i]._state == MUSH_ATKRIGHT)
			{
				_getAtk->frameRender(getMemDC(), _mushroom[i].x - 64, _mushroom[i].y - 64, _mushroom[i].hitFrameX, _mushroom[i].hitFrameY);
			}
			else if (_mushroom[i]._state == MUSH_ATKUP)
			{
				_getAtk->frameRender(getMemDC(), _mushroom[i].x - 64, _mushroom[i].y - 64, _mushroom[i].hitFrameX, _mushroom[i].hitFrameY);
			}
		}
		else
		{
			_destroy4->frameRender(getMemDC(), _mushroom[i].x - 64, _mushroom[i].y - 32, _mushroom[i].destroyX, 0);
		}
		//문어 렉트 확인
		//Rectangle(getMemDC(), _octopus[i]._rc);
	}
}