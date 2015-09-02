#pragma once

namespace PopupTooltipMessageHelpers
{
	// This window intercepts messages of the root parent window of the control.
	class RootParentMsgInterceptor : public CWindowImpl<RootParentMsgInterceptor, CWindow, CNullTraits>
	{
	public:
		explicit RootParentMsgInterceptor(const boost::function<void ()>& removeTooltip);

		void Init(CWindow control, CWindow tooltip);

	private:
		BEGIN_MSG_MAP(RootParentMsgInterceptor)
			MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
			MSG_WM_ACTIVATE(OnActivate)
		END_MSG_MAP()

		void OnWindowPosChanged(LPWINDOWPOS lpWndPos);
		void OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther);

	private:
		boost::function<void ()> m_removeTooltip;
		CWindow m_tooltip;
	};

	// This window intercepts messages of the control.
	// You need this window to track the situation when a child control is dynamically created
	// and the parent window isn't moved or deactivated.
	class ControlMsgInterceptor : public CWindowImpl<ControlMsgInterceptor, CWindow, CNullTraits>
	{
	public:
		explicit ControlMsgInterceptor(const boost::function<void ()>& removeTooltip);

		void Init(CWindow control, CWindow /*tooltip*/);

	private:
		BEGIN_MSG_MAP(ControlMsgInterceptor)
			MSG_WM_DESTROY(OnDestroy)
		END_MSG_MAP()

		void OnDestroy();

	private:
		boost::function<void ()> m_removeTooltip;
	};

} // namespace PopupTooltipMessageHelpers

// MessageBox replacement.
class PopupTooltipMessage
{
public:
	PopupTooltipMessage();

	void Show(const TCHAR* message, CWindow wndControl);
	void CleanUp();

private:
	CToolTipCtrl m_tooltip;
	TOOLINFO m_toolinfo;

	PopupTooltipMessageHelpers::RootParentMsgInterceptor m_rootParentInterceptor;
	PopupTooltipMessageHelpers::ControlMsgInterceptor m_controlInterceptor;
};