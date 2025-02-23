#include "VoteMode.hpp"

#include "Game.hpp"
#include "TileOps.hpp"
#include "Version.hpp"
#include "VNWorld.hpp"

#include "res_voting.h"

#include <cstdlib>
#include <ranges>

namespace Game
{

inline constexpr u16 c_influencePerMash = 4;
inline constexpr s16 c_maxVote = 128;
inline constexpr s16 c_minVote = -128;

inline constexpr u16 c_framesForLeftTitle = 60;
inline constexpr u16 c_framesForRightTitle = 100;
inline constexpr u16 c_framesForEnd = 240;

inline constexpr u16 SetVFlip(u16 i_attr) {
	return (i_attr & (~TILE_ATTR_VFLIP_MASK)) | TILE_ATTR_VFLIP_MASK;
}
inline constexpr u16 SetHFlip(u16 i_attr) {
	return (i_attr & (~TILE_ATTR_HFLIP_MASK)) | TILE_ATTR_HFLIP_MASK;
}

inline constexpr std::array<TileSet const*, 11> c_nums = {
	&voting_time_0,
	&voting_time_1,
	&voting_time_2,
	&voting_time_3,
	&voting_time_4,
	&voting_time_5,
	&voting_time_6,
	&voting_time_7,
	&voting_time_8,
	&voting_time_9,
	&voting_time_10,
};

template<size_t t_Amount>
consteval std::array<u16, t_Amount> GenerateFrameNums(u16 i_framesPerSecond)
{
	std::array<u16, t_Amount> ret;
	for (u16 i = 0; i < t_Amount; ++i)
	{
		ret[i] = (i + 1) * i_framesPerSecond;
	}
	return ret;
}

inline constexpr auto c_pal_framesForNums = GenerateFrameNums<11>(50);
inline constexpr auto c_ntsc_framesForNums = GenerateFrameNums<11>(60);

//------------------------------------------------------------------------------
VoteMode::~VoteMode
(
)
{
	if (m_graphicsReady)
	{
		m_game->Sprites().RemoveSprite(m_num[0]);
		m_game->Sprites().RemoveSprite(m_num[1]);
		m_game->Sprites().RemoveSprite(m_cursor);
	}
}

//------------------------------------------------------------------------------
void VoteMode::Start
(
	VoteModeParams i_params
)
{
	// Cache params
	m_params = std::move(i_params);

	// Set up data
	m_framesLeft = fix16Mul(std::max<f16>(m_params.m_votingTime, FIX16(5)), FramesPerSecond()); // Min 5-seconds
	m_remainingInfluence = m_params.m_startingPlayerInfluence;
	m_voteWon = false;

	GenerateAttackEvents();
	SetupGraphics();

	m_votingStarted = true;
}

//------------------------------------------------------------------------------
void VoteMode::Update
(
)
{
	if (!m_votingStarted || !m_graphicsReady || m_votingComplete)
	{
		if (m_votingComplete)
		{
			++m_endTimer;
		}
		return;
	}

	u16 const buttons = JOY_readJoypad(JOY_1);
	bool const ABCpressedThisFrame = [this, buttons] {
		if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
		{
			if (!m_ABCpressed)
			{
				m_ABCpressed = true;
				return true;
			}
		}
		else
		{
			m_ABCpressed = false;
		}
		return false;
	}();

	if (ABCpressedThisFrame && m_remainingInfluence >= c_influencePerMash)
	{
		m_votePosition += c_influencePerMash;
		m_remainingInfluence -= c_influencePerMash;
	}

	if (!m_attackEvents.empty())
	{
		if (m_attackEvents.back().m_frame >= m_framesLeft)
		{
			m_votePosition -= static_cast<s16>(m_attackEvents.back().m_size);
			m_attackEvents.pop_back();
		}
	}

	if (m_votePosition >= c_maxVote)
	{
		m_votePosition = c_maxVote;
		m_votingComplete = true;
		m_voteWon = true;
		SetupEndGraphics();
		return;
	}
	else if (m_votePosition <= c_minVote)
	{
		m_votePosition = c_minVote;
		m_votingComplete = true;
		m_voteWon = false;
		SetupEndGraphics();
		return;
	}
	
	if (m_framesLeft > 0)
	{
		--m_framesLeft;
	}
	else
	{
		m_votingComplete = true;
		m_voteWon = m_votePosition >= 0;
		SetupEndGraphics();
		return;
	}

	UpdateGraphics();
}

//------------------------------------------------------------------------------
void VoteMode::GenerateAttackEvents
(
)
{
	auto fnGenerateAttacksWithParams = [this](u16 i_attackSize, f16 i_attackSizeVar, f16 i_attackFramesVar)
	{
		f16 const attackSizeVarHalf = i_attackSizeVar / 2;
		f16 const attackFramesVarHalf = i_attackFramesVar / 2;
		u16 remainingAttack = m_params.m_attackSize;

		u16 const possibleAttacks = m_params.m_attackSize / i_attackSize;

		m_attackEvents.reserve(possibleAttacks);

		for (u16 i = 0; i < possibleAttacks; ++i)
		{
			f16 const r = rand() & FIX16_FRAC_MASK;
			f16 const variation = fix16Mul(r, i_attackSizeVar) - attackSizeVarHalf;
			u16 const attackSize = fix16ToInt(variation) + i_attackSize;

			if (remainingAttack < attackSize)
			{
				m_attackEvents.push_back(AttackEvent{ 0, remainingAttack });
				break;
			}
			remainingAttack -= attackSize;

			if (remainingAttack < (i_attackSize - attackSizeVarHalf))
			{
				m_attackEvents.push_back(AttackEvent{ 0, static_cast<u16>(attackSize + remainingAttack) });
				break;
			}
			else
			{
				m_attackEvents.push_back(AttackEvent{ 0, attackSize });
			}
		}

		u16 const averageFramesPerAttack = m_framesLeft / (m_attackEvents.size() + 1); // Add 1 to avoid attacking immediately at the end
		u16 const earliestAllowedAttack = m_framesLeft - fix16ToInt(attackFramesVarHalf);

		u16 regularFrames = averageFramesPerAttack;
		for (u16 i = 0; i < m_attackEvents.size(); ++i)
		{
			f16 const r = rand() & FIX16_FRAC_MASK;
			f16 const variation = fix16Mul(r, i_attackFramesVar) - attackFramesVarHalf;
			u16 const attackTime = std::min<u16>(fix16ToInt(variation < 0 ? attackFramesVarHalf : variation) + regularFrames, earliestAllowedAttack);

			m_attackEvents[i].m_frame = attackTime;

			regularFrames += averageFramesPerAttack;
		}

	};

	switch (m_params.m_attackPattern)
	{
	case AttackPattern::SlowChunky:
	{
		constexpr u16 c_attackSize = 20;
		constexpr f16 c_attackSizeVariation = FIX16(5);
		constexpr f16 c_attackFramesVariation = FIX16(40);
		fnGenerateAttacksWithParams(c_attackSize, c_attackSizeVariation, c_attackFramesVariation);
		break;
	}
	case AttackPattern::FastBitty:
	{
		constexpr u16 c_attackSize = 5;
		constexpr f16 c_attackSizeVariation = FIX16(2);
		constexpr f16 c_attackFramesVariation = FIX16(20);
		fnGenerateAttacksWithParams(c_attackSize, c_attackSizeVariation, c_attackFramesVariation);
		break;
	}
	case AttackPattern::Variable:
	{
		constexpr u16 c_attackSize = 15;
		constexpr f16 c_attackSizeVariation = FIX16(10);
		constexpr f16 c_attackFramesVariation = FIX16(60);
		fnGenerateAttacksWithParams(c_attackSize, c_attackSizeVariation, c_attackFramesVariation);
		break;
	}
	}
}

//------------------------------------------------------------------------------
std::pair<u16, u16> VoteMode::FindNumTileIndicesForFrameTimer() const
{
	if (System::IsNTSC())
	{
		for (u16 i = 0; i < c_ntsc_framesForNums.size(); ++i)
		{
			if (m_framesLeft < c_ntsc_framesForNums[i])
			{
				return { m_num_tileIndex[i], m_num_tileIndex[i] + 12, };
			}
		}
	}
	else
	{
		for (u16 i = 0; i < c_pal_framesForNums.size(); ++i)
		{
			if (m_framesLeft < c_pal_framesForNums[i])
			{
				return { m_num_tileIndex[i], m_num_tileIndex[i] + 12, };
			}
		}
	}

	return { m_num_tileIndex[0], m_num_tileIndex[0] + 12, };
}

//------------------------------------------------------------------------------
void VoteMode::SetupGraphics()
{
	// Load required tiles for sprites, bar, and large text
	m_game->QueueLambdaTask([this] -> Task {
		PAL_setColors(16 * PAL2, voting_palette.data, 16, DMA_QUEUE); // Borrow portrait palette line

		// Borrow character region
		u16 tileIndex = c_tilesEnd;

		// Time numbers
		for (u16 numI = 0; TileSet const* numTiles : c_nums)
		{
			tileIndex -= numTiles->numTile;
			m_num_tileIndex[numI] = tileIndex;
			++numI;
			auto load = Tiles::LoadTiles_Chunked(numTiles, tileIndex);
			AwaitTask(load);
		}

		// Bars
		{
			tileIndex -= voting_bar_corners.numTile;
			m_barCorners_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_bar_corners, tileIndex);
			AwaitTask(load);
		}
		{
			tileIndex -= voting_bar_mids.numTile;
			m_barMids_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_bar_mids, tileIndex);
			AwaitTask(load);
		}

		// Cursor
		{
			tileIndex -= voting_cursor.numTile;
			m_cursor_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_cursor, tileIndex);
			AwaitTask(load);
		}

		// Giant text
		{
			tileIndex -= voting_passed_vote_set.numTile;
			m_passed_vote_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_passed_vote_set, tileIndex);
			AwaitTask(load);
		}
		{
			tileIndex -= voting_passed_passed_set.numTile;
			m_passed_passed_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_passed_passed_set, tileIndex);
			AwaitTask(load);
		}
		{
			tileIndex -= voting_failed_vote_set.numTile;
			m_failed_vote_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_failed_vote_set, tileIndex);
			AwaitTask(load);
		}
		{
			tileIndex -= voting_failed_failed_set.numTile;
			m_failed_failed_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_failed_failed_set, tileIndex);
			AwaitTask(load);
		}

		// Influence bar
		{
			tileIndex -= voting_influence_bar.numTile;
			m_influenceBar_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_influence_bar, tileIndex);
			AwaitTask(load);
		}
		{
			tileIndex -= voting_influence_bar_text_set.numTile;
			m_influenceBarText_tileIndex = tileIndex;
			auto load = Tiles::LoadTiles_Chunked(&voting_influence_bar_text_set, tileIndex);
			AwaitTask(load);
			auto map = Tiles::SetMap_SubFull(
				VDP_BG_A,
				voting_influence_bar_text_map.tilemap,
				voting_influence_bar_text_map.w,
				voting_influence_bar_text_map.h,
				TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, tileIndex),
				14,
				22
			);
			AwaitTask(map);
		}

		// Text
		{
			auto text = RenderText(tileIndex);
			AwaitTask(text);
		}

		// Add sprites for number
		s8 z = -64;
		{
			std::pair<u16, u16> const tileIndex = FindNumTileIndicesForFrameTimer();
			auto [leftID, left] = m_game->Sprites().AddSprite(SpriteSize::r4c3, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, tileIndex.first));
			auto [rightID, right] = m_game->Sprites().AddSprite(SpriteSize::r4c3, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, tileIndex.second));
			m_num[0] = leftID;
			m_num[1] = rightID;

			left.SetX(17 * 8);
			left.SetY(2 * 8);
			left.SetZ(z++);
			right.SetX(20 * 8);
			right.SetY(2 * 8);
			right.SetZ(z++);
		}

		// Add cursor sprite
		{
			// Cursor has to use PAL3 to get shadow mode to work
			auto [id, spr] = m_game->Sprites().AddSprite(SpriteSize::r4c2, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, m_cursor_tileIndex));
			m_cursor = id;

			spr.SetX((c_screenWidthPx / 2) - 8);
			spr.SetY((16 - 1) * 8);
			spr.SetZ(z++);
		}

		// Create bars
		m_updateBarTileMap = UpdateBarTileMap();
		m_updateInfluenceBarMap = UpdateInfluenceBarTileMap();

		m_graphicsReady = true;
	});
}

//------------------------------------------------------------------------------
Task VoteMode::RenderText
(
	u16 i_tileIndex
)
{
	char const* str = m_params.m_voteName;
	if (!str)
	{
		co_return;
	}

	// Render tiles
	u16 x = 0;
	while (*str != '\0')
	{
		char const curChar = *str;
		if (curChar != ' ')
		{
			u8 const curCharWidth = m_fontData->GetVNTextFontCharWidth(curChar);
			if (curCharWidth + x > 256)
			{
				// Can't fit any more characters
				++str;
				continue;
			}

			u32 const* curCharTileRows = m_fontData->GetVNTextFontTile(curChar)->AsRawRows();

			// Whilst we have 8 pixel tall text that aligns with the tiles, a character spans 2 tiles generally. So we'll do left tile, then right tile
			u16 const tileInd = (x >> 3);
			while ((tileInd + 1u) >= m_voteNameTextTiles.size())
			{
				m_voteNameTextTiles.insert(m_voteNameTextTiles.end(), {Tiles::Tile{}, Tiles::Tile{}, Tiles::Tile{}, Tiles::Tile{}});
			}
			u32* const leftTile = m_voteNameTextTiles[tileInd].AsRawRows();

			// So for the left tile, we need to just shift the font data to the right by the amount of pixels m_x is in to the tile
			u8 leftTilePixels = x & 0x7;
			u16 const shift = leftTilePixels << 2;
			for (u8 i = 0; i < 8; ++i)
			{
				leftTile[i] |= curCharTileRows[i] >> shift;
			}

			u16 const used = 8 - leftTilePixels;
			if (used < curCharWidth)
			{
				u32* const rightTile = m_voteNameTextTiles[tileInd + 1].AsRawRows();
				u16 antiShift = used << 2;
				for (u8 i = 0; i < 8; ++i)
				{
					rightTile[i] |= curCharTileRows[i] << antiShift;
				}
			}

			x += curCharWidth;
		}
		else if (x > 0)
		{
			// Default space size, but only if we're not at the start of a line
			x += 4;
		}
		++str;
	}

	u16 const textTileIndex = i_tileIndex - m_voteNameTextTiles.size();

	while (!DMA_queueDmaFast(
		DMA_VRAM,
		m_voteNameTextTiles.data(),
		textTileIndex * 32,
		m_voteNameTextTiles.size() * (sizeof(Tiles::Tile) >> 1),
		2
	))
	{
		co_yield{};
	}

	co_yield{};

	// Add sprites
	s8 z = -32;
	u16 spriteX = (c_screenWidthPx - x) / 2;
	for (u16 i = 0; i < m_voteNameTextTiles.size(); i += 4)
	{
		auto [id, spr] = m_game->Sprites().AddSprite(SpriteSize::r1c4, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, textTileIndex + i));
		m_voteNameSprites.push_back(id);
		spr.SetX(spriteX + (i * 8));
		spr.SetY(10 * 8);
		spr.SetZ(z++);
	}

	co_return;
}

//------------------------------------------------------------------------------
void VoteMode::UpdateGraphics()
{
	m_updateBarTileMap();
	m_updateInfluenceBarMap();

	auto cursor = m_game->Sprites().EditSpriteData(m_cursor);
	cursor.SetX((c_screenWidthPx / 2) - 8 + m_votePosition);

	auto numLeft = m_game->Sprites().EditSpriteData(m_num[0]);
	auto numRight = m_game->Sprites().EditSpriteData(m_num[1]);
	auto const [numTileLeft, numTileRight] = FindNumTileIndicesForFrameTimer();
	numLeft.SetFirstTileIndex(numTileLeft);
	numRight.SetFirstTileIndex(numTileRight);
}

//------------------------------------------------------------------------------
Task VoteMode::UpdateBarTileMap
(
)
{
	while (!m_votingComplete)
	{
		m_barTileMap[0] = GetBarCornerAttr(false);
		m_barTileMap[31] = GetBarCornerAttr(true);

		for (u16 i = 1; i < 31; ++i)
		{
			m_barTileMap[i] = GetBarMidAttr(i);
		}

		for (u16 i = 0, j = 32; i < 32; ++i, ++j)
		{
			m_barTileMap[j] = SetVFlip(m_barTileMap[i]);
		}

		// Draw bar
		{
			auto map = Tiles::SetMap_SubFull(
				VDP_BG_A,
				m_barTileMap.data(),
				32,
				2,
				0,
				4,
				16
			);
			AwaitTask(map);
		}

		co_yield{};
	}

	co_return;
}
//------------------------------------------------------------------------------
Task VoteMode::UpdateInfluenceBarTileMap
(
)
{
	while (!m_votingComplete)
	{
		u16 const influenceSmall = m_remainingInfluence / 2;
		u16 const tile = influenceSmall / 20;
		for (u16 i = 0; i < tile; ++i)
		{
			m_influenceBarTileMap[i + 20] = TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_influenceBar_tileIndex + 8);
		}
		u16 const offset = std::min<u16>(16, influenceSmall - (tile * 16)) / 2;
		m_influenceBarTileMap[tile + 20] = TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_influenceBar_tileIndex + offset);
		for (u16 i = tile + 1; i < 20; ++i)
		{
			m_influenceBarTileMap[i + 20] = TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_influenceBar_tileIndex);
		}

		for (u16 i = 0; i < 20; ++i)
		{
			m_influenceBarTileMap[i] = SetHFlip(m_influenceBarTileMap[39 - i]);
		}

		// Draw bar
		{
			auto map = Tiles::SetMap_SubFull(
				VDP_BG_A,
				m_influenceBarTileMap.data(),
				40,
				1,
				0,
				0,
				24
			);
			AwaitTask(map);
		}

		co_yield{};
	}

	co_return;
}

//------------------------------------------------------------------------------
u16 VoteMode::GetBarCornerAttr
(
	bool i_hflip
) const
{
	if (!i_hflip)
	{
		// Top-left
		if (m_votePosition >= (c_minVote + 8))
		{
			return TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_barCorners_tileIndex + 6);
		}

		// Filling with green from the left
		u16 const index = static_cast<u16>(std::max<s16>(m_votePosition - (c_minVote + 2), 0));
		return TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_barCorners_tileIndex + index);
	}
	else
	{
		// Top-right
		if (m_votePosition <= (c_maxVote - 8))
		{
			return TILE_ATTR_FULL(PAL2, TRUE, FALSE, TRUE, m_barCorners_tileIndex);
		}

		// Filling with green from the left, after flipping means decrementing from right side of tiles
		u16 const negIndex = static_cast<u16>(std::min<s16>(m_votePosition - (c_maxVote - 7), 5));
		return TILE_ATTR_FULL(PAL2, TRUE, FALSE, TRUE, m_barCorners_tileIndex + voting_bar_corners.numTile - negIndex - 1);
	}
}

//------------------------------------------------------------------------------
u16 VoteMode::GetBarMidAttr
(
	u16 i_tileI
) const
{
	// Green fills from left, so hflip only needed when playing in reverse
	u16 const index = std::min<u16>(static_cast<u16>(std::max<s16>(m_votePosition - c_minVote - (i_tileI * 8), 0)), 8);
	return TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_barMids_tileIndex + index);
}

//------------------------------------------------------------------------------
void VoteMode::SetupEndGraphics()
{
	std::ranges::for_each(m_num, [this](SpriteID id) { m_game->Sprites().RemoveSprite(id); });
	m_game->Sprites().RemoveSprite(m_cursor);
	std::ranges::for_each(m_voteNameSprites, [this](SpriteID id) { m_game->Sprites().RemoveSprite(id); });

	m_vnWorld->WhiteBG(*m_game, true);
	m_vnWorld->HideCharacterVisual(*m_game, true);
	m_vnWorld->BlackBG(*m_game, false);

	m_game->QueueFunctionTask(UpdateEndGraphics());
}

//------------------------------------------------------------------------------
Task VoteMode::UpdateEndGraphics()
{
	while (m_endTimer < c_framesForLeftTitle)
	{
		co_yield{};
	}
	if (m_voteWon)
	{
		auto map = Tiles::SetMap_SubFull(
			VDP_BG_A,
			voting_passed_vote_map.tilemap,
			voting_passed_vote_map.w,
			voting_passed_vote_map.h,
			TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_passed_vote_tileIndex),
			1,
			12
		);
		AwaitTask(map);
	}
	else
	{
		auto map = Tiles::SetMap_SubFull(
			VDP_BG_A,
			voting_failed_vote_map.tilemap,
			voting_failed_vote_map.w,
			voting_failed_vote_map.h,
			TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_failed_vote_tileIndex),
			2,
			12
		);
		AwaitTask(map);
	}

	while (m_endTimer < c_framesForRightTitle)
	{
		co_yield{};
	}
	if (m_voteWon)
	{
		auto map = Tiles::SetMap_SubFull(
			VDP_BG_A,
			voting_passed_passed_map.tilemap,
			voting_passed_passed_map.w,
			voting_passed_passed_map.h,
			TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_passed_passed_tileIndex),
			18,
			12
		);
		AwaitTask(map);
	}
	else
	{
		auto map = Tiles::SetMap_SubFull(
			VDP_BG_A,
			voting_failed_failed_map.tilemap,
			voting_failed_failed_map.w,
			voting_failed_failed_map.h,
			TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, m_failed_failed_tileIndex),
			18,
			12
		);
		AwaitTask(map);
	}

	while (m_endTimer < c_framesForEnd)
	{
		co_yield{};
	}
	m_graphicsDone = true;

	co_return;
}

}