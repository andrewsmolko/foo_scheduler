#include "pch.h"
#include "pref_page.h"
#include "service_manager.h"

PreferencesPage::PreferencesPage(preferences_page_callback::ptr callback) : m_callback(callback), m_changed(false)
{
	m_pModel.reset(new PrefPageModel(ServiceManager::Instance().GetModel().GetState()));

	m_pModel->ConnectModelChangedSlot(boost::bind(&PreferencesPage::OnChanged, this));
	m_pModel->ConnectModelResetSlot(boost::bind(&PreferencesPage::OnChanged, this));
}

t_uint32 PreferencesPage::get_state()
{
	t_uint32 state = preferences_state::resettable;
	
	if (m_changed) 
		state |= preferences_state::changed;

	return state;
}

void PreferencesPage::apply()
{
	// Our dialog content has not changed but the flags have - our currently shown
	// values now match the settings so the apply button can be disabled.
	ServiceManager::Instance().GetModel().SetState(m_pModel->GetState());

	m_changed = false;
	m_callback->on_state_changed();
}

void PreferencesPage::reset()
{
	m_pModel->Reset();
}

BOOL PreferencesPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	DoDataExchange(DDX_LOAD);

	m_eventList.InitWindow(m_pModel.get());
	m_actionLists.Init(m_pModel.get());

	CheckDlgButton(IDC_ENABLED_CHECK, m_pModel->IsSchedulerEnabled());

	return TRUE;
}

void PreferencesPage::OnBtnAddEvent(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CMenu menuPopup;
	menuPopup.CreatePopupMenu();

	std::vector<Event*> eventPrototypes = ServiceManager::Instance().GetEventPrototypesManager().GetPrototypes();

	for (std::size_t i = 0; i < eventPrototypes.size(); ++i)
	{
		std::wstring item = L"Add " + eventPrototypes[i]->GetName() + L"...";
		menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND, static_cast<UINT_PTR>(i + 1), item.c_str());
	}

	CRect rcBtnAddEvent;
	GetDlgItem(IDC_BTN_ADD_EVENT).GetWindowRect(rcBtnAddEvent);

	UINT uCmdID = menuPopup.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTALIGN,
		rcBtnAddEvent.BottomRight().x - 1, rcBtnAddEvent.BottomRight().y - 1,
		*this);

	if (uCmdID == 0)
		return;

	auto pNewEvent(eventPrototypes[uCmdID - 1]->CreateFromPrototype());

	if (!pNewEvent->ShowConfigDialog(*this, m_pModel.get()))
		return;

	m_pModel->AddEvent(std::move(pNewEvent));
}

void PreferencesPage::OnBtnAddActionList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	std::unique_ptr<ActionList> pActionList(new ActionList);

	if (!pActionList->ShowConfigDialog(*this, m_pModel.get()))
		return;

	m_pModel->AddActionList(std::move(pActionList));
}

void PreferencesPage::OnChanged()
{
	CheckDlgButton(IDC_ENABLED_CHECK, m_pModel->IsSchedulerEnabled());

	m_changed = true;

	// Tell the host that our state has changed to enable/disable the apply button appropriately.
	m_callback->on_state_changed();
}

void PreferencesPage::OnBtnShowStatusWindow(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	ServiceManager::Instance().GetRootController().ShowStatusWindow();
}

void PreferencesPage::OnEnableScheduler(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_pModel->SetSchedulerEnabled(IsDlgButtonChecked(IDC_ENABLED_CHECK) == 1);
}

//------------------------------------------------------------------------------
// PreferencesPageImpl
//------------------------------------------------------------------------------

// {c86e2704-ca6e-4467-b48e-f83f2452d2a0}
const GUID PreferencesPageImpl::m_guid =
	{ 0xc86e2704, 0xca6e, 0x4467, { 0xb4, 0x8e, 0xf8, 0x3f, 0x24, 0x52, 0xd2, 0xa0 } };

const char* PreferencesPageImpl::get_name()
{
	return "Scheduler";
}

GUID PreferencesPageImpl::get_guid()
{
	return m_guid;
}

GUID PreferencesPageImpl::get_parent_guid()
{
	return preferences_page::guid_root;
}