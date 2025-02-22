#pragma once

#include "Declare.hpp"
#include "CharacterData.hpp"

#include "Jam15/res_chara.h"

namespace Jam15
{

inline constexpr u16 c_halfWidthChar = 10;

//------------------------------------------------------------------------------
/// Zanmu
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<5> c_zanmu_blink_anim = { {
	{ &zanmu_blink_1, Game::AnimFrameDuration::Variable(60, 300), },
	{ &zanmu_blink_2, Game::AnimFrameDuration::Fixed(4), },
	{ &zanmu_blink_3, Game::AnimFrameDuration::Fixed(3), },
	{ &zanmu_blink_4, Game::AnimFrameDuration::Fixed(2), },
	{ &zanmu_blink_5, Game::AnimFrameDuration::Fixed(4), },
} };

inline constexpr Game::Pose c_zanmu_neutral_pose = {
	&zanmu_tileset,
	&zanmu_pal,
	c_zanmu_blink_anim
};

inline constexpr Game::Poses<1> c_zanmu_poses = {
	c_zanmu_neutral_pose,
};

inline constexpr Game::Character c_zanmu = {
	"ZANMU", true,
	c_zanmu_poses,
};

//------------------------------------------------------------------------------
/// Hisami
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<5> c_hisami_neutral = { {
	{ &hisami_neutral_1, Game::AnimFrameDuration::Variable(60, 300), c_halfWidthChar, },
	{ &hisami_neutral_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &hisami_neutral_3, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &hisami_neutral_4, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &hisami_neutral_5, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_hisami_neutral_pose = {
	&hisami_tileset,
	&hisami_pal,
	c_hisami_neutral
};

inline constexpr Game::AnimFrames<7> c_hisami_flush = { {
	{ &hisami_flush_1, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ &hisami_flush_2, Game::AnimFrameDuration::Fixed(20), c_halfWidthChar, },
	{ &hisami_flush_3, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ &hisami_flush_4, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &hisami_flush_5, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &hisami_flush_6, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &hisami_flush_7, Game::AnimFrameDuration::Infinite(), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_hisami_flush_pose = {
	&hisami_tileset,
	&hisami_pal,
	c_hisami_flush
};

inline constexpr Game::AnimFrames<3> c_hisami_joy = { {
	{ &hisami_joy_1, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ &hisami_joy_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &hisami_joy_3, Game::AnimFrameDuration::Infinite(), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_hisami_joy_pose = {
	&hisami_tileset,
	&hisami_pal,
	c_hisami_joy
};

inline constexpr Game::Poses<3> c_hisami_poses = {
	c_hisami_neutral_pose,
	c_hisami_flush_pose,
	c_hisami_joy_pose,
};

inline constexpr Game::Character c_hisami = {
	"HISAMI", false,
	c_hisami_poses,
};

//------------------------------------------------------------------------------
/// Suika
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<6> c_suika_neutral = { {
	{ &suika_neutral_1, Game::AnimFrameDuration::Variable(60, 300), c_halfWidthChar, },
	{ &suika_neutral_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &suika_neutral_3, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ &suika_neutral_4, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &suika_neutral_5, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &suika_neutral_6, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_suika_neutral_pose = {
	&suika_tileset,
	&suika_pal,
	c_suika_neutral
};

inline constexpr Game::AnimFrames<3> c_suika_bigjoy = { {
	{ &suika_bigjoy_1, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ &suika_bigjoy_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &suika_bigjoy_3, Game::AnimFrameDuration::Infinite(), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_suika_bigjoy_pose = {
	&suika_tileset,
	&suika_pal,
	c_suika_bigjoy
};

inline constexpr Game::AnimFrames<4> c_suika_pout = { {
	{ &suika_pout_1, Game::AnimFrameDuration::Fixed(5), },
	{ &suika_pout_2, Game::AnimFrameDuration::Fixed(3), },
	{ &suika_pout_3, Game::AnimFrameDuration::Fixed(2), },
	{ &suika_pout_4, Game::AnimFrameDuration::Infinite(), },
} };

inline constexpr Game::Pose c_suika_pout_pose = {
	&suika_tileset,
	&suika_pal,
	c_suika_pout
};

inline constexpr Game::Poses<3> c_suika_poses = {
	c_suika_neutral_pose,
	c_suika_bigjoy_pose,
	c_suika_pout_pose,
};

inline constexpr Game::Character c_suika = {
	"SUIKA", false,
	c_suika_poses,
};

//------------------------------------------------------------------------------
/// Yuugi
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<5> c_yuugi_neutral = { {
	{ &yuugi_neutral_1, Game::AnimFrameDuration::Variable(60, 300), c_halfWidthChar, },
	{ &yuugi_neutral_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuugi_neutral_3, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuugi_neutral_4, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &yuugi_neutral_5, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_yuugi_neutral_pose = {
	&yuugi_tileset,
	&yuugi_pal,
	c_yuugi_neutral
};

inline constexpr Game::AnimFrames<4> c_yuugi_angry = { {
	{ &yuugi_angry_1, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuugi_angry_2, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuugi_angry_3, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &yuugi_angry_4, Game::AnimFrameDuration::Infinite(), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_yuugi_angry_pose = {
	&yuugi_tileset,
	&yuugi_pal,
	c_yuugi_angry
};

inline constexpr Game::AnimFrames<6> c_yuugi_dejected = { {
	{ &yuugi_dejected_1, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &yuugi_dejected_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuugi_dejected_3, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ &yuugi_dejected_4, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuugi_dejected_5, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &yuugi_dejected_6, Game::AnimFrameDuration::Infinite(), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_yuugi_dejected_pose = {
	&yuugi_tileset,
	&yuugi_pal,
	c_yuugi_dejected
};

inline constexpr Game::Poses<3> c_yuugi_poses = {
	c_yuugi_neutral_pose,
	c_yuugi_angry_pose,
	c_yuugi_dejected_pose,
};

inline constexpr Game::Character c_yuugi = {
	"YUUGI", false,
	c_yuugi_poses,
};

//------------------------------------------------------------------------------
/// Yuuma
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<5> c_yuuma_neutral = { {
	{ &yuuma_neutral_1, Game::AnimFrameDuration::Variable(60, 300), c_halfWidthChar, },
	{ &yuuma_neutral_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuuma_neutral_3, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_neutral_4, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &yuuma_neutral_5, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_yuuma_neutral_pose = {
	&yuuma_tileset,
	&yuuma_pal,
	c_yuuma_neutral
};

inline constexpr Game::AnimFrames<8> c_yuuma_double = { {
	{ &yuuma_neutral_1, Game::AnimFrameDuration::Variable(60, 300), c_halfWidthChar, },
	{ &yuuma_neutral_2, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuuma_neutral_3, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuuma_neutral_4, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
	{ &yuuma_neutral_2, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_neutral_3, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_neutral_4, Game::AnimFrameDuration::Fixed(2), c_halfWidthChar, },
	{ &yuuma_neutral_5, Game::AnimFrameDuration::Fixed(4), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_yuuma_double_pose = {
	&yuuma_tileset,
	&yuuma_pal,
	c_yuuma_double
};

inline constexpr Game::AnimFrames<7> c_yuuma_annoy = { {
	{ &yuuma_annoy_1, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_annoy_2, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_annoy_3, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_annoy_4, Game::AnimFrameDuration::Fixed(3), c_halfWidthChar, },
	{ &yuuma_annoy_5, Game::AnimFrameDuration::Fixed(6), c_halfWidthChar, },
	{ &yuuma_annoy_6, Game::AnimFrameDuration::Fixed(5), c_halfWidthChar, },
	{ .m_tilemap = &yuuma_annoy_7, .m_duration = Game::AnimFrameDuration::Fixed(6), .m_xOffset = c_halfWidthChar, .m_nextIndex = 4 },
} };

inline constexpr Game::Pose c_yuuma_annoy_pose = {
	&yuuma_tileset,
	&yuuma_pal,
	c_yuuma_annoy
};

inline constexpr Game::AnimFrames<3> c_yuuma_pleased = { {
	{ &yuuma_pleased_1, Game::AnimFrameDuration::Fixed(6), c_halfWidthChar, },
	{ &yuuma_pleased_2, Game::AnimFrameDuration::Fixed(6), c_halfWidthChar, },
	{ &yuuma_pleased_3, Game::AnimFrameDuration::Infinite(), c_halfWidthChar, },
} };

inline constexpr Game::Pose c_yuuma_pleased_pose = {
	&yuuma_tileset,
	&yuuma_pal,
	c_yuuma_pleased
};

inline constexpr Game::Poses<4> c_yuuma_poses = {
	c_yuuma_neutral_pose,
	c_yuuma_double_pose,
	c_yuuma_annoy_pose,
	c_yuuma_pleased_pose,
};

inline constexpr Game::Character c_yuuma = {
	"YUUMA", false,
	{},
};

//------------------------------------------------------------------------------
/// Various without images
//------------------------------------------------------------------------------
// A generic councillor on the left
inline constexpr Game::Character c_lcouncil = {
	"KISHIN", true,
	{},
};
// A generic councillor on the right
inline constexpr Game::Character c_rcouncil = {
	"KISHIN", false,
	{},
};
// The chair
inline constexpr Game::Character c_speaker = {
	"CHAIR", false,
	{},
};

// The shadowy kishin
inline constexpr Game::Character c_kishin = {
	"HIDDEN KISHIN", false,
	{},
};

}