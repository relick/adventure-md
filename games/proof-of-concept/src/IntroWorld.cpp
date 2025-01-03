#include "Worlds.hpp"
#include "Game.hpp"

#include <genesis.h>
#include "res_fonts.h"

namespace Game
{

void IntroWorld::Init
(
	Game& io_game
)
{
	PAL_setPalette(PAL0, intro_font_pal.data, TransferMethod::DMA);
	VDP_loadFont(&intro_font, TransferMethod::DMA);
	VDP_setTextPalette(PAL0);

	VDP_drawText("kasha.dev", 15, 0);
}

void IntroWorld::Shutdown
(
	Game& io_game
)
{
	VDP_setVerticalScroll(VDP_getTextPlane(), 0);
	VDP_clearPlane(VDP_getTextPlane(), true);
}

void IntroWorld::Run
(
	Game& io_game
)
{
	if (y < 56)
	{
		VDP_setVerticalScroll(VDP_getTextPlane(), -2*(y++));
	}
	else if (y < 150)
	{
		++y;
	}
	else
	{
		io_game.RequestNextWorld(std::make_unique<GameWorld>());
	}
}

}