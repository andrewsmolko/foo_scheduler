#pragma once

#include "async_call.h"

//------------------------------------------------------------------------------
// IActionListExecSessionDelegate
//------------------------------------------------------------------------------

class IActionListExecSessionFuncs
{
public:
    virtual boost::any GetValue(const std::string &key) const = 0;
    virtual void SetValue(const std::string &key, const boost::any &value) = 0;
    virtual void UpdateDescription() = 0;

protected:
    ~IActionListExecSessionFuncs() {}
};

//------------------------------------------------------------------------------
// IActionExecSession
//------------------------------------------------------------------------------

class IAction;

struct IActionExecSession
{
	virtual ~IActionExecSession() {}

	virtual void Init(IActionListExecSessionFuncs& alesFuncs) = 0;

	// completionCall is called after an action has been completed.
	virtual void Run(const AsyncCall::CallbackPtr& completionCall) = 0;

	// Returns an action that has created this session.
	virtual const IAction* GetParentAction() const = 0;

	// Returns true if there's a description of the current state.
	virtual bool GetCurrentStateDescription(std::wstring& descr) const = 0;
};

typedef boost::shared_ptr<IActionExecSession> ActionExecSessionPtr;