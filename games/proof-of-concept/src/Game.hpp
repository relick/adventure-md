#pragma once

#include "Declare.hpp"
#include "SpriteManager.hpp"
#include "GameRoutines.hpp"

#include <deque>
#include <functional>
#include <memory>
#include <vector>

namespace Game
{

#define PROFILER (0)

template<typename T>
concept NotTaskCoroutine = !std::convertible_to<T, Task>;

struct Erasure
{
	virtual ~Erasure() = default;
};

template<NotTaskCoroutine T_Lambda>
struct LambdaHolder
	: Erasure
{
	LambdaHolder(T_Lambda&& i_lambda) : m_lambda(std::move(i_lambda)) {}
	T_Lambda m_lambda;
};

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;
	WorldRoutine m_currentWorldRoutine;

	SpriteManager m_sprites;

	std::deque<std::pair<Task, std::unique_ptr<Erasure>>> m_tasks;

	static inline VBlankCallbackID s_callbackID = 0;
	static inline std::vector<std::pair<VBlankCallbackID, std::function<void()>>> s_vBlankCallbacks;

public:
	Game();

	void RequestNextWorld(std::unique_ptr<World> &&i_nextWorld);

	void Run();

#if PROFILER
	static u32 GetVCount();
#endif

	VBlankCallbackID AddVBlankCallback(std::function<void()>&& i_callback);
	void RemoveVBlankCallback(VBlankCallbackID i_callbackID);

	SpriteManager& Sprites() { return m_sprites; }

	void QueueFunctionTask(Task&& i_task);
	template<NotTaskCoroutine T_Lambda, typename... T_Args>
	void QueueLambdaTask(T_Lambda&& i_lambda, T_Args&&... i_args);
	bool TasksInProgress() const;

private:
	static void VIntCallback();
	static void VBlankCallback();

	void PostWorldFrame();
	void PreWorldInit();
};

#if PROFILER

struct AutoProfileScope
{
	char const* m_fmt;
	u32 m_before;
	AutoProfileScope(char const* i_fmt) : m_fmt(i_fmt), m_before(Game::GetVCount()) {}
	~AutoProfileScope() {
		u32 after = Game::GetVCount();
		kprintf(m_fmt, after - m_before);
	}
};

#else

struct AutoProfileScope
{
	AutoProfileScope(char const* i_fmt) {}
};

#endif

template<NotTaskCoroutine T_Lambda, typename... T_Args>
void Game::QueueLambdaTask
(
	T_Lambda&& i_lambda,
	T_Args&&... i_args
)
{
	LambdaHolder<T_Lambda>* holder = new LambdaHolder<T_Lambda>{ std::move(i_lambda) };
	m_tasks.push_back({
		holder->m_lambda(std::forward<T_Args>(i_args)...),
		std::unique_ptr<Erasure>(holder)
	});
}

}