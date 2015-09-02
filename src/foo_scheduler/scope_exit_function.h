#pragma once

class ScopeExitFunction : private boost::noncopyable
{
public:
	explicit ScopeExitFunction(const boost::function<void ()>& func) : m_func(func)
	{
	}

	~ScopeExitFunction()
	{
		m_func();
	}

private:
	boost::function<void ()> m_func;
};