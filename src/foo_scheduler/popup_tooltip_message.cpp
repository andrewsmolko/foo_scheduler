#include "pch.h"
#include "popup_tooltip_message.h"

namespace PopupTooltipMessageHelpers
{
	RootParentMsgInterceptor::RootParentMsgInterceptor(const boost::function<void ()>& removeTooltip) : m_removeTooltip(removeTooltip)
	{
	}

	void RootParentMsgInterceptor::Init(CWindow control, CWindow tooltip)
	{
		if (m_hWnd != NULL)
			UnsubclassWindow();

		SubclassWindow(::GetAncestor(control, GA_ROOT));
		m_tooltip = tooltip;
	}

	void RootParentMsgInterceptor::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
	{
		SetMsgHandled(false);
		m_removeTooltip();
	}

	void RootParentMsgInterceptor::OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
	{
		SetMsgHandled(false);

		if (nState == WA_INACTIVE && wndOther != m_tooltip.m_hWnd)
			m_removeTooltip();
	}

	ControlMsgInterceptor::ControlMsgInterceptor(const boost::function<void ()>& removeTooltip) : m_removeTooltip(removeTooltip)
	{

	}

	void ControlMsgInterceptor::Init(CWindow control, CWindow /*tooltip*/)
	{
		if (m_hWnd != NULL)
			UnsubclassWindow();

		SubclassWindow(control);
	}

	void ControlMsgInterceptor::OnDestroy()
	{
		SetMsgHandled(false);
		m_removeTooltip();
	}

} // namespace PopupTooltipMessageHelpers

PopupTooltipMessage::PopupTooltipMessage() :
	m_toolinfo(),
	m_rootParentInterceptor(boost::bind(&PopupTooltipMessage::CleanUp, this)),
	m_controlInterceptor(boost::bind(&PopupTooltipMessage::CleanUp, this))
{

}

void PopupTooltipMessage::Show(const TCHAR* message, CWindow wndControl)
{
	if (message == NULL && m_tooltip.m_hWnd == NULL)
		return;

	if (m_tooltip.m_hWnd == NULL)
	{
		m_tooltip.Create(NULL, NULL, NULL, TTS_BALLOON | TTS_NOPREFIX | WS_POPUP);

		m_rootParentInterceptor.Init(wndControl, m_tooltip);
		m_controlInterceptor.Init(wndControl, m_tooltip);
	}

	if (m_tooltip.m_hWnd == NULL)
		return;

	if (m_tooltip.GetToolCount() > 0)
	{
		m_tooltip.TrackActivate(&m_toolinfo, FALSE);
		m_tooltip.DelTool(&m_toolinfo);
	}

	if (message != NULL)
	{
		m_toolinfo.cbSize = sizeof(m_toolinfo);
		m_toolinfo.uFlags = TTF_TRACK | TTF_IDISHWND | TTF_ABSOLUTE | TTF_TRANSPARENT | TTF_CENTERTIP;
		m_toolinfo.hwnd = wndControl;
		m_toolinfo.uId = 0;
		m_toolinfo.lpszText = const_cast<TCHAR*>(message);
		m_toolinfo.hinst = core_api::get_my_instance();

		if (m_tooltip.AddTool(&m_toolinfo))
		{
			CRect rect;
			wndControl.GetWindowRect(rect);
			m_tooltip.TrackPosition(rect.CenterPoint().x, rect.bottom);
			m_tooltip.TrackActivate(&m_toolinfo, TRUE);
		}
	}
}

void PopupTooltipMessage::CleanUp()
{
	if (m_rootParentInterceptor.m_hWnd != NULL)
		m_rootParentInterceptor.UnsubclassWindow();

	if (m_controlInterceptor.m_hWnd != NULL)
		m_controlInterceptor.UnsubclassWindow();

	if (m_tooltip.m_hWnd != NULL)
		m_tooltip.DestroyWindow();
}