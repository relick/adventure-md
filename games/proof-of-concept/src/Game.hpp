#pragma once

#include "Declare.hpp"
#include "SpriteManager.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace Game
{

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;

	SpriteManager m_sprites;

	static inline u8 s_callbackID = 0;
	static inline std::vector<std::pair<u8, std::function<void()>>> s_vBlankCallbacks;

public:
	Game();

	void RequestNextWorld(std::unique_ptr<World> &&i_nextWorld);

	void Run();

	u8 AddVBlankCallback(std::function<void()>&& i_callback);
	void RemoveVBlankCallback(u8 i_callbackID);

	SpriteManager& Sprites() { return m_sprites; }

private:
	static void VBlankCallback();
};

}