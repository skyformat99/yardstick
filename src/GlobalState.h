/* Copyright (C) 2016 Sean Middleditch, all rights reserverd. */

#pragma once

#include <yardstick/yardstick.h>

#include "Atomics.h"
#include "Spinlock.h"
#include "Signal.h"
#include "WebsocketSink.h"

#include <cstring>
#include <thread>

namespace _ys_ {

class ThreadState;

class GlobalState
{
	Signal _signal;
	AlignedAtomic<bool> _active;

	Spinlock _stateLock;
	std::thread _backgroundThread;
	ysAllocator _allocator;

	Spinlock _threadsLock;
	ThreadState* _threads = nullptr;

	WebsocketSink _websocketSink;

	void ThreadMain();
	ysResult ProcessThread(ThreadState* thread);
	ysResult Flush();

public:
	GlobalState() : _active(false) {}
	GlobalState(GlobalState const&) = delete;
	GlobalState& operator=(GlobalState const&) = delete;

	inline static GlobalState& instance();

	ysResult Initialize(ysAllocator allocator);
	bool IsActive() const { return _active.load(std::memory_order_relaxed); }
	ysResult Shutdown();

	ysResult ListenWebsocket(unsigned short port);

	void RegisterThread(ThreadState* thread);
	void DeregisterThread(ThreadState* thread);

	void PostThreadBuffer() { _signal.Post(); }

	ysResult Tick();
};

GlobalState& GlobalState::instance()
{
	static GlobalState state;
	return state;
}

} // namespace _ys_
