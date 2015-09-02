#pragma once

#include "resource.h"
#include "event.h"
#include "popup_tooltip_message.h"
#include "pref_page_model.h"
#include "menu_item_event_s11n_block.h"

class MenuItemEvent : public Event
{
public: // Event
	virtual GUID GetPrototypeGUID() const;
	virtual int GetPriority() const;
	virtual std::wstring GetName() const;
	virtual std::wstring GetDescription() const;
	virtual bool ShowConfigDialog(CWindow parent, PrefPageModel* pPrefPageModel);
	virtual void OnSignal();
	virtual Event* Clone() const;
	virtual Event* CreateFromPrototype() const;
	virtual void LoadFromS11nBlock(const EventS11nBlock& block);
	virtual void SaveToS11nBlock(EventS11nBlock& block) const;

public:
	MenuItemEvent();

	enum EFinalAction
	{
		finalActionReenable,
		finalActionDisable,
		finalActionRemove
	};

	EFinalAction GetFinalAction() const;
	std::wstring GetMenuItemName() const;
	GUID GetGUID() const;
	
private:
	friend class MenuItemEventEditor;
	void SetFinalAction(EFinalAction finalAction);
	void SetMenuItemName(const std::wstring& menuItemName);

private:
	MenuItemEvent(const MenuItemEvent& rhs);

	std::wstring m_menuItemName;
	EFinalAction m_finalAction;
	GUID m_guid;
};

class MenuItemEventEditor : public CDialogImpl<MenuItemEventEditor>
{
public:
	enum { IDD = IDD_MENU_ITEM_EVENT_CONFIG };

	MenuItemEventEditor(MenuItemEvent* pEvent, PrefPageModel* pPrefPageModel);

private:
	BEGIN_MSG_MAP(MenuItemEventEditor)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnClose)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnClose)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	bool CheckEventName(const std::wstring& eventName) const;

private:
	MenuItemEvent* m_pEvent;
	PrefPageModel* m_pPrefPageModel;

	CEdit m_menuItemName;
	CComboBox m_finalAction;

	PopupTooltipMessage m_popupTooltipMsg;
};