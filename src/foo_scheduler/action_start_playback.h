#pragma once

#include "resource.h"
#include "action.h"
#include "popup_tooltip_message.h"

class ActionStartPlayback : public IAction
{
public:
	class ExecSession : public IActionExecSession
	{
	public:
		explicit ExecSession(const ActionStartPlayback& action);

		virtual void Init(const boost::function<void ()>& updateALESDescriptionFunc);
		virtual void Run(const AsyncCall::CallbackPtr& completionCall);
		virtual const IAction* GetParentAction() const;
		virtual bool GetCurrentStateDescription(std::wstring& descr) const;

	private:
		const ActionStartPlayback& m_action;
	};

public:
	ActionStartPlayback();

	enum EStartType
	{
		startTypeOrdinary,
		startTypeFromTrack
	};

	EStartType GetStartPlaybackType() const;
	void SetStartPlaybackType(EStartType startType);

	t_uint32 GetStartTrackNumber() const;
	void SetStartTrackNumber(t_uint32 track);

public:
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

private:
	ActionStartPlayback(const ActionStartPlayback& rhs);

private:
	EStartType m_playbackStartType;
	t_uint32 m_startTrackNumber;
};

//------------------------------------------------------------------------------
// ActionStartPlaybackEditor
//------------------------------------------------------------------------------

class ActionStartPlaybackEditor :
	public CDialogImpl<ActionStartPlaybackEditor>,
	public CWinDataExchange<ActionStartPlaybackEditor>
{
public:
	enum { IDD = IDD_ACTION_START_PLAYBACK_CONFIG };

	explicit ActionStartPlaybackEditor(ActionStartPlayback* pAction);

private:
	ActionStartPlayback* m_pAction;

	int m_playbackStartType;

private:
	BEGIN_MSG_MAP_EX(ActionStartPlaybackEditor)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDC_BTN_PICK_CURRENT_TRACK, OnPickCurrentTrack)
		COMMAND_ID_HANDLER_EX(IDC_RADIO_ORDINARY_START, OnStartTypeSelected)
		COMMAND_ID_HANDLER_EX(IDC_RADIO_START_FROM_TRACK, OnStartTypeSelected)

		COMMAND_ID_HANDLER_EX(IDOK,     OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
		END_MSG_MAP()

		BEGIN_DDX_MAP(ActionStartPlaybackEditor)
			DDX_RADIO(IDC_RADIO_ORDINARY_START, m_playbackStartType)
		END_DDX_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void OnPickCurrentTrack(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnStartTypeSelected(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	void UpdateControlsState();

private:
	PopupTooltipMessage m_popupTooltipMsg;
};
