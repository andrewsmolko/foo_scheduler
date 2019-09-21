#pragma once

class ScopeExitFunction : private boost::noncopyable
{
public:
	explicit ScopeExitFunction(const boost::function<void ()>& func) : m_func(func)
	{
	}

	~ScopeExitFunction()
	{
        if (m_func)
		    m_func();
	}

    void Clear()
    {
        m_func.clear();
    }

private:
	boost::function<void ()> m_func;
};