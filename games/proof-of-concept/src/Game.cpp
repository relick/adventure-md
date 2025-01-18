#include "Game.hpp"

#include "Worlds.hpp"

#include <genesis.h>

namespace Game
{

Game::Game
(
)
{
	SYS_setVBlankCallback(Game::VBlankCallback);

	VDP_setScreenWidth320();
	VDP_setScreenHeight224();
	PAL_setColorsDMA(0, palette_black, 64);
}

void Game::RequestNextWorld(std::unique_ptr<World> &&i_nextWorld)
{
	m_nextWorld = std::move(i_nextWorld);
}

void Game::Run()
{
	while (true)
	{
		if (m_nextWorld)
		{
			if (m_curWorld)
			{
				m_curWorld->Shutdown(*this);
			}
			m_nextWorld->Init(*this);
			std::swap(m_curWorld, m_nextWorld);
			m_nextWorld = nullptr;
		}
		m_curWorld->Run(*this);

		m_sprites.Update();
		SYS_doVBlankProcess();
	}
}

u8 Game::AddVBlankCallback(std::function<void()>&& i_callback)
{
	s_vBlankCallbacks.push_back({ s_callbackID, std::move(i_callback), });
	return s_callbackID++;
}

void Game::RemoveVBlankCallback(u8 i_callbackID)
{
	auto cbI = std::find_if(
		s_vBlankCallbacks.begin(),
		s_vBlankCallbacks.end(),
		[i_callbackID](auto const& cb) { return cb.first == i_callbackID; }
	);
	
	if(cbI != s_vBlankCallbacks.end())
	{
		s_vBlankCallbacks.erase(cbI);
	}
}

void Game::VBlankCallback()
{
	for (auto const& [id, f] : s_vBlankCallbacks)
	{
		f();
	}
}

}
