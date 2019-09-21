#include "pch.h"
#include "event_list_window.h"
#include "service_manager.h"
#include "action.h"
#include "action_list.h"
#include "pref_page_model.h"
#include "generate_duplicate_name.h"

namespace
{

class EventDuplicateVisitor : public IEventVisitor
{
public:
    explicit EventDuplicateVisitor(const std::vector<Event*>& events)
        : m_modelEvents(events)
    {
    }

    std::unique_ptr<Event> TakeEvent()
    {
        return std::move(m_event);
    }

private: // IEventVisitor
    void Visit(PlayerEvent& event) override
    {
        m_event = event.Clone();
    }

    void Visit(DateTimeEvent& event) override
    {
        const std::wstring newTitle = GenerateDuplicateName(event.GetTitle(), m_modelEvents, [] (Event* e) {
            if (const DateTimeEvent* dateTimeEvent = dynamic_cast<const DateTimeEvent*>(e))
                return dateTimeEvent->GetTitle();
            return std::wstring();
        });

        m_event = event.Duplicate(newTitle);
    }

    void Visit(MenuItemEvent& event) override
    {
        const std::wstring newMenuItemName =
            GenerateDuplicateName(event.GetMenuItemName(), m_modelEvents, [](Event *e) {
                if (const MenuItemEvent *menuItemEvent = dynamic_cast<const MenuItemEvent *>(e))
                    return menuItemEvent->GetMenuItemName();
                return std::wstring();
            });

        m_event = event.Duplicate(newMenuItemName);
    }

private:
    std::vector<Event*> m_modelEvents;
    std::unique_ptr<Event> m_event;
};

} // namespace

EventListWindow::EventListWindow() : m_columnsBeingInited(false)
{

}

void EventListWindow::InitWindow(PrefPageModel* pModel)
{
	m_pModel = pModel;

	DWORD dwStyle = LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER;
	SetExtendedListViewStyle(dwStyle, dwStyle);

	::SetWindowTheme(m_hWnd, L"explorer", 0);

	InitColumns();

	std::vector<Event*> events = m_pModel->GetEvents();
	std::for_each(events.begin(), events.end(), boost::bind(&EventListWindow::AddNewEvent, this, _1));

	m_pModel->ConnectEventAddedSlot(boost::bind(&EventListWindow::OnNewEventAdded, this, _1));
	m_pModel->ConnectEventUpdatedSlot(boost::bind(&EventListWindow::OnEventUpdated, this, _1));
	m_pModel->ConnectEventRemovedSlot(boost::bind(&EventListWindow::OnEventRemoved, this, _1));

	m_pModel->ConnectModelResetSlot(boost::bind(&EventListWindow::OnModelReset, this));
}

void EventListWindow::OnNewEventAdded(Event* pNewEvent)
{
	SelectItem(AddNewEvent(pNewEvent));
}

int EventListWindow::AddNewEvent(Event* pNewEvent)
{
	int pos = GetItemCount();
	InsertEventAtPos(pos, pNewEvent);
	return pos;
}

LRESULT EventListWindow::OnItemChanged(LPNMHDR pnmh)
{
	NMLISTVIEW* pInfo = reinterpret_cast<NMLISTVIEW*>(pnmh);

	int item = pInfo->iItem;

	if (item == -1)
		return 0;

	if ((pInfo->uChanged & LVIF_STATE) != LVIF_STATE)
		return 0;

	int checked = ((pInfo->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1;

	if (checked == -1)
		return 0;

	Event* pEvent = reinterpret_cast<Event*>(GetItemData(item));
	
	if (pEvent == 0)
		return 0;

	bool newState = checked == 1 ? true : false;

	if (pEvent->IsEnabled() != newState)
	{
		pEvent->Enable(checked == 1 ? true : false);
		m_pModel->UpdateEvent(pEvent);
	}

	return 0;
}

void EventListWindow::OnEventUpdated(Event* pEvent)
{
	int item = FindItemByEvent(pEvent);
	_ASSERTE(item != -1);

	SetItem(item, 0, LVIF_TEXT, pEvent->GetDescription().c_str(), 0, 0, 0, 0);
	
	ActionList* pActionList = m_pModel->GetActionListByGUID(pEvent->GetActionListGUID());
	SetItem(item, 1, LVIF_TEXT, pActionList != 0 ? pActionList->GetName().c_str() : L"", 0, 0, 0, 0);
}

void EventListWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	DefWindowProc();

	UINT nHitFlags;
	int  iHit = HitTest(point, &nHitFlags);

	if (nHitFlags == LVHT_ONITEMSTATEICON)
		return;

	int item = GetSelectedIndex();

	if (item == -1)
		return;

	EditEvent(reinterpret_cast<Event*>(GetItemData(item)));
}

int EventListWindow::FindItemByEvent(const Event* pEvent)
{
	for (int i = 0; i < GetItemCount(); ++i)
		if (reinterpret_cast<const Event*>(GetItemData(i)) == pEvent)
			return i;

	return -1;
}

void EventListWindow::ShowEventContextMenu(int item, const CPoint& point)
{
	Event* pEvent = reinterpret_cast<Event*>(GetItemData(item));

	CPoint pnt = point;
	ClientToScreen(&pnt);

	CMenu menuPopup;
	menuPopup.CreatePopupMenu();

	AppendActionListsItems(menuPopup, pEvent);
	AppendEventItems(menuPopup, pEvent);

	UINT uCmdID = menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, pnt.x, pnt.y, GetActiveWindow());

	switch (uCmdID)
	{
	case 0:
		return;

	case menuItemEdit:
		EditEvent(pEvent);
		break;

    case menuItemDuplicate:
        {
            EventDuplicateVisitor visitor(m_pModel->GetEvents());
            pEvent->ApplyVisitor(visitor);           
            m_pModel->AddEvent(visitor.TakeEvent());
        }
        break;

	case menuItemRemove:
		m_pModel->RemoveEvent(pEvent);
		break;

	case menuItemMoveUp:
		m_pModel->MoveEventUp(pEvent);
		MoveEventItem(pEvent, true);
		break;

	case menuItemMoveDown:
		m_pModel->MoveEventDown(pEvent);
		MoveEventItem(pEvent, false);
		break;

	default: // Action list selected.
		{
			std::size_t actionListIndex = uCmdID - 1;
			std::vector<ActionList*> actionLists = m_pModel->GetActionLists();

			pEvent->SetActionListGUID(actionListIndex != actionLists.size() ?
				actionLists[actionListIndex]->GetGUID() : pfc::guid_null);
			m_pModel->UpdateEvent(pEvent);		
		}
		break;
	}
}

void EventListWindow::AppendActionListsItems(CMenu& menuPopup, Event* pEvent)
{
	CMenu menuActionLists;
	menuActionLists.CreatePopupMenu();

	std::vector<ActionList*> actionLists = m_pModel->GetActionLists();

	for (std::size_t i = 0; i < actionLists.size(); ++i)
	{
		menuActionLists.AppendMenu(MF_STRING | MF_BYCOMMAND, static_cast<UINT_PTR>(i + 1),
			actionLists[i]->GetName().c_str());
	}

	// Create "None" item.
	if (!actionLists.empty())
		menuActionLists.AppendMenu(MF_SEPARATOR);

	menuActionLists.AppendMenu(MF_STRING | MF_BYCOMMAND,
		static_cast<UINT_PTR>(actionLists.size() + 1), L"None");

	// Check item.
	if (pEvent->GetActionListGUID() == pfc::guid_null)
	{
		menuActionLists.CheckMenuRadioItem(1, actionLists.size() + 1,
			actionLists.size() + 1, MF_BYCOMMAND);
	}
	else
	{
		for (std::size_t i = 0; i < actionLists.size(); ++i)
			if (pEvent->GetActionListGUID() == actionLists[i]->GetGUID())
			{
				menuActionLists.CheckMenuRadioItem(1, actionLists.size(),
					i + 1, MF_BYCOMMAND);
				break;
			}
	}

	menuPopup.AppendMenu(MF_POPUP, menuActionLists.Detach(), L"Assign action list");
}

void EventListWindow::AppendEventItems(CMenu& menuPopup, const Event* pEvent)
{
	menuPopup.AppendMenu(MF_SEPARATOR);

	menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND,
		static_cast<UINT_PTR>(menuItemEdit), L"Edit...");

    menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND,
        static_cast<UINT_PTR>(menuItemDuplicate), L"Duplicate");

	menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND,
		static_cast<UINT_PTR>(menuItemRemove), L"Remove");

	menuPopup.AppendMenu(MF_SEPARATOR);
	menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND |
		(m_pModel->CanMoveEventUp(pEvent) ? 0 : MF_DISABLED | MF_GRAYED),
		static_cast<UINT_PTR>(menuItemMoveUp), L"Move up");
	menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND |
		(m_pModel->CanMoveEventDown(pEvent) ? 0 : MF_DISABLED | MF_GRAYED),
		static_cast<UINT_PTR>(menuItemMoveDown), L"Move down");
}

void EventListWindow::EditEvent(Event* pEvent)
{
	if (!pEvent->ShowConfigDialog(*this, m_pModel))
		return;

	m_pModel->UpdateEvent(pEvent);
}

void EventListWindow::OnEventRemoved(Event* pEvent)
{
	int item = FindItemByEvent(pEvent);
	_ASSERTE(item != -1);
	DeleteItem(item);
}

void EventListWindow::OnModelReset()
{
	DeleteAllItems();
}

void EventListWindow::OnContextMenu(CWindow wnd, CPoint point)
{
	int selected = GetSelectedIndex();

	if (selected == -1)
		return;

	if (point.x < 0 && point.y < 0)
	{
		CRect itemRect;
		GetItemRect(selected, itemRect, LVIR_LABEL);

		point.x = itemRect.left;
		point.y = itemRect.bottom;
	}
	else
		ScreenToClient(&point);

	ShowEventContextMenu(selected, point);
}

LRESULT EventListWindow::OnHeaderItemChanged(LPNMHDR pnmh)
{
	NMHEADER* pHeader = reinterpret_cast<NMHEADER*>(pnmh);

	if (!pHeader->pitem || (pHeader->pitem->mask & HDI_WIDTH) != HDI_WIDTH || m_columnsBeingInited)
		return 0;

	std::vector<int> cw;
	cw.push_back(GetColumnWidth(0));
	cw.push_back(GetColumnWidth(1));

	ServiceManager::Instance().GetModel().SetEventsWindowColumnsWidths(cw);

	SetMsgHandled(false);

	return 0;
}

void EventListWindow::InitColumns()
{
	m_columnsBeingInited = true;

	AddColumn(L"Event", 0);
	AddColumn(L"Action list", 1);

	CRect rcList;
	GetClientRect(rcList);

	// Using global model to request columns' widths.
	std::vector<int> cw = ServiceManager::Instance().GetModel().GetEventsWindowColumnsWidths();

	if (cw.empty())
	{
		SetColumnWidth(0, (rcList.Width() * 2) / 3);
		SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	}
	else
	{
		_ASSERTE(cw.size() == 2);

		SetColumnWidth(0, cw[0]);
		SetColumnWidth(1, cw[1]);
	}

	m_columnsBeingInited = false;
}

void EventListWindow::InsertEventAtPos(int pos, const Event* pEvent)
{
	AddItem(pos, 0, pEvent->GetDescription().c_str());

	ActionList* pActionList = m_pModel->GetActionListByGUID(pEvent->GetActionListGUID());
	AddItem(pos, 1, pActionList != 0 ? pActionList->GetName().c_str() : L"");

	// Do not reorder! Dependency in EventListWindow::OnItemChanged
	SetItemData(pos, reinterpret_cast<DWORD_PTR>(pEvent));
	SetCheckState(pos, pEvent->IsEnabled());
}

void EventListWindow::MoveEventItem(const Event* pEvent, bool up)
{
	int pos = FindItemByEvent(pEvent);
	int newPos = pos + (up ? - 1 : 1);

	DeleteItem(pos);
	InsertEventAtPos(newPos, pEvent);
	SelectItem(newPos);
}