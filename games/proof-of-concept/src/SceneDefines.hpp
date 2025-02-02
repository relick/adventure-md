#pragma once

#include "Declare.hpp"
#include "Game.hpp"
#include "VNWorld.hpp"
#include "Version.hpp"

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(BG) io_vn.SetBG(io_game, BG)

#define play_music(MUSIC, FADE_IN_SECONDS, LOOP) io_vn.StartMusic(MUSIC, fix16ToInt(fix16Mul(FIX16(FADE_IN_SECONDS), FramesPerSecond())), LOOP)

#define wait_for_tasks() io_vn.WaitForTasks(io_game); wait

#define portrait(CHARA, POSE)
#define hide_portrait()

#define show(CHARA, POSE) io_vn.SetCharacterVisual(io_game, c_ ## CHARA ## _ ## POSE ## _pose)
#define hide() io_vn.HideCharacterVisual(io_game, false)

#define think(TEXT) io_vn.SetText(io_game, nullptr, "(" TEXT ")"); wait

#define say(CHARA, TEXT) io_vn.SetText(io_game, &c_ ## CHARA, TEXT); wait

#define choice(CHOICE_SPAN) io_vn.Choice(io_game, CHOICE_SPAN); wait
#define get_choice_result() *(io_vn.GetChoiceResult())
#define timed_choice(TIME_IN_SECONDS, CHOICE_SPAN) io_vn.TimedChoice(io_game, CHOICE_SPAN, FIX16(TIME_IN_SECONDS)); wait
#define get_timed_choice_result() io_vn.GetChoiceResult()

#define vpunch()