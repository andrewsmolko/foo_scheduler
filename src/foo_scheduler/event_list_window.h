#pragma once

#include "event.h"

class PrefPageModel;

class EventListWindow :
	public CCheckListViewCtrlImpl<EventListWindow>,
	public boost::signals2::trackable
{
public:
	EventListWindow();

	void InitWindow(PrefPageModel* pModel);

private:
	void OnNewEventAdded(Event* pNewEvent);
	void OnEventUpdated(Event* pEvent);
	void OnEventRemoved(Event* pEvent);

	void OnModelReset();

	void InitColumns();

	// Returns index of the item that has been added.
	int AddNewEvent(Event* pNewEvent);
	void InsertEventAtPos(int pos, const Event* pEvent);

	int FindItemByEvent(const Event* pEvent);
	void MoveEventItem(const Event* pEvent, bool up);
	void EditEvent(Event* pEvent);

	void ShowEventContextMenu(int item, const CPoint& point);

	void AppendActionListsItems(CMenu& menuPopup, Event* pEvent);
	void AppendEventItems(CMenu& menuPopup, const Event* pEvent);

private:
	BEGIN_MSG_MAP(EventListWindow)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGED, OnItemChanged)
		
		// Not using REFLECTED_ because our header is our child window and a child window
		// sends notification to the parent window first. Only then the parent window can reflect
		// it to the child.
		NOTIFY_CODE_HANDLER_EX(HDN_ITEMCHANGED, OnHeaderItemChanged)

		MSG_WM_CONTEXTMENU(OnContextMenu)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)

		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnItemChanged(LPNMHDR pnmh);
	LRESULT OnHeaderItemChanged(LPNMHDR pnmh);
	void OnContextMenu(CWindow wnd, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	enum EPopupMenuItems
	{
		menuItemEdit = 1000,
		menuItemRemove,
		menuItemMoveUp,
		menuItemMoveDown
	};

	PrefPageModel* m_pModel;

	bool m_columnsBeingInited;
};

