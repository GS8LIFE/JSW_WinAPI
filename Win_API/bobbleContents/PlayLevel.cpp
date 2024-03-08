#include "PlayLevel.h"
#include "Dragon.h"
#include "DragonTool.h"
#include "Dragon2.h"
#include "Dragon2Tool.h"
#include "Ceil.h"
#include "shooter.h"
#include "arrow.h"
#include "Bobble.h"
#include "Bobblefire.h"
#include <EngineCore\EngineCore.h>
PlayLevel::PlayLevel() 
{

}

PlayLevel::~PlayLevel() 
{
}

void PlayLevel::BeginPlay()
{
	ULevel::BeginPlay();


	FVector WindowScale = GEngine->MainWindow.GetWindowScale();
	AActor* Back = SpawnActor<AActor>();
	UImageRenderer* Renderer = Back->CreateImageRenderer(RenderOrder::Map);
	Renderer->SetImage("map.png");
	Renderer->SetPosition(WindowScale.Half2D());
	Renderer->SetScale(WindowScale);

	AActor* borderline = SpawnActor<AActor>();
	UImageRenderer* Renderer1 = borderline->CreateImageRenderer(RenderOrder::Map);
	Renderer1->SetImage("border.png");
	Renderer1->SetPosition({ 317,286 });
	Renderer1->SetScale({ 320,548 });

	AActor* bar = SpawnActor<AActor>();
	UImageRenderer* Renderer3 = bar->CreateImageRenderer(RenderOrder::Map);
	Renderer3->SetAlpha(100);
	Renderer3->SetImage("bar.png");
	Renderer3->SetTransform({ {325,400}, {272, 60} });

	Arrow = SpawnActor<arrow>();
	shooter* Shooter = SpawnActor<shooter>();
	Ceil* ceiling = SpawnActor<Ceil>();
	Dragon* Player = SpawnActor<Dragon>();
	Dragon2* Player2 = SpawnActor<Dragon2>();
	Dragon2Tool* Dragon2T = SpawnActor<Dragon2Tool>();
	DragonTool* Dragon1T = SpawnActor<DragonTool>();

	helper maphelper;

	//버블 배열
	map[0] = { '.', '.', '.', '.', 'G', '.', '.', 'G' };
	map[1] = { '.', '.', '.', 'G', '.', '.', '.', '/' };
	map[2] = { '.', '.', '.', '.', '.', '.', '.', '.' };
	map[3] = { '.', '.', '.', '.', '.', '.', '.', '/' };
	map[4] = { '.', '.', '.', '.', '.', '.', '.', '.'};
	map[5] = { '.', '.', '.', '.', '.', '.', '.', '/' };
	map[6] = { '.', '.', '.', '.', '.', '.', '.', '.' };
	map[7] = { '.', '.', '.', '.', '.', '.', '.', '/' };
	map[8] = { '.', '.', '.', '.', '.', '.', '.', '.' };
	map[9] = { '.', '.', '.', '.', '.', '.', '.', '/' };
	map[10] = { '.', '.', '.', '.', '.', '.', '.', '.' };
	map[11] = { '.', '.', '.', '.', '.', '.', '.', '/' };

	maphelper.setnowmap(getnowmap());

	for (int row_idx = 0; row_idx < map.size(); ++row_idx)
	{
		const auto& row = map[row_idx];

		// . 혹은 / 이면 다음으로 넘김 (색이 없으므로)
		for (int col_idx = 0; col_idx < row.size(); ++col_idx)
		{
			char col = row[col_idx];
			if (col == '.' || col == '/')
			{
				continue;
			}
			
			if (row_idx % 2 == 1)
			{
				Bobble* NewB = SpawnActor<Bobble>();
				NewB->get_bubble(col);
				NewB->SetActorLocation({ 221 + (32 * col_idx), 65 + (32 * row_idx) });
				NewB->setmap(getmap(),row_idx,col_idx);
				NewB->getnowmap(maphelper.nowmap);
				nowbobble[{col_idx, row_idx}].push_back(NewB);
			}
			else
			{
				Bobble* NewB = SpawnActor<Bobble>();	
				NewB->get_bubble(col);
				NewB->SetActorLocation({ 205 + (32 * col_idx), 65 + (32 * row_idx) });
				NewB->setmap(getmap(), row_idx, col_idx);
				NewB->getnowmap(maphelper.nowmap);
				nowbobble[{col_idx, row_idx}].push_back(NewB);
				}
		}
	}
	fire_bobble();
}


int PlayLevel::check_x(int _x,int _y)
{
	if (_x > 0) 
	{
		if (y % 2 == 1)
		{	
			_x = (collideLocate.X - 237) / 32 + 1;
		}
		else
		{
		_x = (collideLocate.X - 221) / 32 + 1;
		}
	}
	else
	return _x;
}

std::map<int, std::vector<char>> PlayLevel::getmap()
{
	return map;
}
void PlayLevel::set_map_vector()
{
	y = (collideLocate.Y - 65) / 16;
	x = (collideLocate.X - 205) / 16;
	if (y > 0)
	{
		y = (collideLocate.Y - 81) / 32 + 1;
		x = check_x(x,y);
	}
	else
	{
		x = check_x(x,y);
	}
}


void PlayLevel::fire_bobble()
{
	firebobble = SpawnActor<Bobblefire>();
	now = firebobble->get_bubble(map);
	firebobble->get_bubble(now);
	firebobble->SetActorLocation({ 328,410 });
	nextbobble = SpawnActor<Bobble>();
	next = firebobble->get_bubble(map);
	nextbobble->get_bubble(next);
	nextbobble->SetActorLocation({ 264,448 });
	cur_bobble = true;
}

void PlayLevel::remove_bobble(int _row, int _col, char _color)
{
	visited.clear();
	visit(_row, _col, _color);
	if (visited.size() >= 3)
	{
		remove_visited_bubbles();
	//	remove_hanging_bubbles();
	}

}
void PlayLevel::visit(int _row, int _col, char _color)
{
	char col;
	std::pair<int, int> p;
	p = std::make_pair(_row, _col);
	if (_row < 0 or _row >= 8  or _col < 0 or _col >= 11)
	{
		return;
	}
	if (_col % 2 && _row == 8)
	{
		col = map[_col][7];
	}
	else
	{
		col = map[_col][_row];
	}
	if (_color != col)
	{
		return;
	}
	if (map[_col][_row] == '/' || map[_col][_row] == '.')
	{
		return;
	}
	if (std::find(visited.begin(), visited.end(), p) != visited.end()) {
		return; // 이미 방문한 곳이면 함수를 빠져나감
	}

	// 방문 처리: (row_idx, col_idx)  visited 벡터에 추가
	visited.push_back(std::make_pair(_row, _col));

	int rows[] = { 0, -1, -1, 0, 1, 1 };
	int cols[] = { -1, -1, 0, 1, 0, -1 };

	if (_row % 2 == 1) {
		int rows[] = { 0, -1, -1, 0, 1, 1 };
		int cols[] = { -1, 0, 1, 1, 1, 0 };
	}	

	for (int i = 0; i < 6; i++) {
		visit(_row + rows[i], _col + cols[i], _color);
	}

}
void PlayLevel::remove_visited_bubbles()
{
	std::pair<int, int> p;
	
	for (int col = 0; col < 11; col++)
	{
		for (int row = 0; row < 8; row++)
		{
			p = std::make_pair(row, col);
			if (std::find(visited.begin(), visited.end(), p) != visited.end())
			{
				map[col][row] = '.';

				auto& bobbles = nowbobble[{row, col}];
				for (auto bobble : bobbles) {
					bobble->Destroy();
				}

			}
		}
	}
	int a = 0;
}

void PlayLevel::fired_bobble()
{
	nextbobble->Destroy();
	firebobble = SpawnActor<Bobblefire>();
	firebobble->get_bubble(next);
	now = next;
	firebobble->SetActorLocation({ 328,410 });
	nextbobble = SpawnActor<Bobble>();
	next = firebobble->get_bubble(map);
	nextbobble->get_bubble(next);
	nextbobble->SetActorLocation({ 264,448 });
	cur_bobble = true;
}
void PlayLevel::Tick(float _DeltaTime) {

	ULevel::Tick(_DeltaTime);	
	if (cur_bobble == true && true == UEngineInput::IsDown(VK_SPACE))
	{
		firebobble->setfireAng(Arrow->getangle());
		cur_bobble = false;
		firing = true;
	}
	else if (firing == true) 
	{
		if (firebobble->IsDestroy())
		{
 			set_map_vector();
			if (map[y][x] == '/' && y % 2 == 1   ) // 짝수는 8번째 자리가 없기에 8번째에 놓아졌을 때 7번째로 옮기는 작업 
			{

				Bobble* NewB = SpawnActor<Bobble>();
				NewB->get_bubble(now);
				NewB->SetActorLocation({ 221 + (32 * (x-1)), 65 + (32 * y) }); 
				NewB->setmap(getmap(),x,y);
				map[y][x] = now;
				nowbobble[{ x, y}].push_back(NewB);
			}
			else {
				if (y % 2 == 1)
				{
					Bobble* NewB = SpawnActor<Bobble>();
					NewB->get_bubble(now);
					NewB->SetActorLocation({ 221 + (32 * x), 65 + (32 * y) });
					NewB->setmap(getmap(),x,y);
					map[y][x] = now;
					nowbobble[{x, y}].push_back(NewB);
				} 
				else
				{
					Bobble* NewB = SpawnActor<Bobble>();
					NewB->get_bubble(now);
					NewB->SetActorLocation({ 205 + (32 * x), 65 + (32 * y) });
					NewB->setmap(getmap(),x,y);
					map[y][x] = now;
					nowbobble[{x, y}].push_back(NewB);
				}
			}
			remove_bobble(x, y, now);
			nowmap = getmap();
			CellCount++ ;
			firing = false;
		}
		else if (firebobble->IsDestroy() == false)
		{
			set_collide_locate(firebobble->get_collide_locate());
		}
	}
	else if(cur_bobble == false )
	{
		fired_bobble();
	}	

	int a = map.size();
}
