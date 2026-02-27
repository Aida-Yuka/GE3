#include "Sprite.h"
#include "SpriteBase.h"

void Sprite::Initialize(SpriteBase* spriteBase)
{
	//引数で受け取ってメンバ変数に記録する
	this->spriteBase = spriteBase;
}
