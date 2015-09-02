#pragma once

#include "async_call.h"
#include "action_exec_session.h"
#include "action_s11n_block.h"

//------------------------------------------------------------------------------
// IAction
//------------------------------------------------------------------------------

class IAction
{
public:
	virtual ~IAction() {}

	virtual GUID GetPrototypeGUID() const = 0;
	virtual int GetPriority() const = 0;
	virtual std::wstring GetName() const = 0;
	virtual IAction* Clone() const = 0;

	virtual std::wstring GetDescription() const = 0;
	virtual bool HasConfigDialog() const = 0;
	virtual bool ShowConfigDialog(CWindow parent) = 0;
	virtual ActionExecSessionPtr CreateExecSession() const = 0;

	virtual void LoadFromS11nBlock(const ActionS11nBlock& block) = 0;
	virtual void SaveToS11nBlock(ActionS11nBlock& block) const = 0;
};

// For boost::ptr_vector.
inline IAction* new_clone(const IAction& a)
{
	return a.Clone();
}

