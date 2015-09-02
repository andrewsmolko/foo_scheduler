#pragma once

#include "async_call.h"

//------------------------------------------------------------------------------
// IActionExecSession
//------------------------------------------------------------------------------

class IAction;

struct IActionExecSession
{
	virtual ~IActionExecSession() {}

	// updateALESDescriptionFunc is used to update the parent action list exec session description.
	virtual void Init(const boost::function<void ()>& updateALESDescriptionFunc) = 0;

	// completionCall is called after an action has been completed.
	virtual void Run(const AsyncCall::CallbackPtr& completionCall) = 0;

	// Returns an action that has created this session.
	virtual const IAction* GetParentAction() const = 0;

	// Returns true if there's a description of the current state.
	virtual bool GetCurrentStateDescription(std::wstring& descr) const = 0;
};

typedef boost::shared_ptr<IActionExecSession> ActionExecSessionPtr;