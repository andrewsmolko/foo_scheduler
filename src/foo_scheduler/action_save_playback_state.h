#pragma once

#include "action.h"

class ActionSavePlaybackState : public IAction
{
public:
    class ExecSession : public IActionExecSession
    {
    public:
        explicit ExecSession(const ActionSavePlaybackState& action);

        virtual void Init(IActionListExecSessionFuncs& alesFuncs);
        virtual void Run(const AsyncCall::CallbackPtr& completionCall);
        virtual const IAction* GetParentAction() const;
        virtual bool GetCurrentStateDescription(std::wstring& descr) const;

    private:
        const ActionSavePlaybackState& m_action;
        IActionListExecSessionFuncs* m_alesFuncs = nullptr;
    };

    static const char* GetPlaylistKey();
    static const char* GetTrackKey();
	static const char* GetPositionKey();

public: // IAction
    virtual GUID GetPrototypeGUID() const;
    virtual int GetPriority() const;
    virtual std::wstring GetName() const;
    virtual IAction* Clone() const;

    virtual std::wstring GetDescription() const;
    virtual bool HasConfigDialog() const;
    virtual bool ShowConfigDialog(CWindow parent);
    virtual ActionExecSessionPtr CreateExecSession() const;

    virtual void LoadFromS11nBlock(const ActionS11nBlock& block);
    virtual void SaveToS11nBlock(ActionS11nBlock& block) const;
};