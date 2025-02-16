#include "TitleWorld.hpp"

#include "Debug.hpp"
#include "Game.hpp"
#include "VNWorld.hpp"

namespace Game
{

//------------------------------------------------------------------------------
TitleWorld::TitleWorld
(
	std::unique_ptr<Script> i_script
)
	: m_script{ std::move(i_script) }
{
	if (!m_script) { Error("Must provide a script to TitleWorld"); }
}

//------------------------------------------------------------------------------
WorldRoutine TitleWorld::Init
(
	Game& io_game
)
{
	io_game.RequestNextWorld(std::make_unique<VNWorld>(std::move(m_script)));
	co_return;
}

//------------------------------------------------------------------------------
WorldRoutine TitleWorld::Shutdown
(
	Game& io_game
)
{
	co_return;
}

//------------------------------------------------------------------------------
void TitleWorld::Run
(
	Game& io_game
)
{
}

}