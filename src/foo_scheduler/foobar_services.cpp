#include "pch.h"
#include "foobar_services.h"
#include "service_manager.h"
#include "definitions.h"
#include "foobar_stream.h"

//------------------------------------------------------------------------------
// PluginInitQuit
//------------------------------------------------------------------------------

void PluginInitQuit::on_init()
{
	ServiceManager::Instance().GetRootController().Init();
}

void PluginInitQuit::on_quit()
{
	ServiceManager::Instance().GetRootController().Shutdown();
}

//------------------------------------------------------------------------------
// PluginConfiguration
//------------------------------------------------------------------------------

PluginConfiguration::PluginConfiguration(const GUID& p_guid) : cfg_var(p_guid)
{
}

void PluginConfiguration::get_data_raw(stream_writer* p_stream, abort_callback& p_abort)
{
	foobar_stream_writer stream(*p_stream, p_abort);

	t_uint16 cfgVersion = PLUGIN_CFG_GLOBAL_VERSION;
	stream << cfgVersion;

	ServiceManager::Instance().GetModel().Save(stream);
}

void PluginConfiguration::set_data_raw(stream_reader* p_stream, t_size p_sizehint, abort_callback& p_abort)
{
	foobar_stream_reader stream(*p_stream, p_abort);

	// Reading config version
	t_uint16 cfgVersion = 0;
	bool readFailed = false;

	try
	{
		stream >> cfgVersion;
	}
	catch (exception_io&)
	{
		readFailed = true;
	}

	if (readFailed || cfgVersion != PLUGIN_CFG_GLOBAL_VERSION)
	{
		console::info(PLUGIN_DLL " : the configuration format is of version 3.x. Upgrade is not supported. "
			"Resetting configuration.");
		return;
	}

	ServiceManager::Instance().GetModel().Load(stream);
}

//------------------------------------------------------------------------------
// MainMenuItems
//------------------------------------------------------------------------------

namespace MainMenuItems
{
	//------------------------------------------------------------------------------
	// SchedulerGroup
	//------------------------------------------------------------------------------

	// {02be80d3-0eb1-4181-bd01-89f1c114439f} 
	const GUID SchedulerGroup::m_guid = 
	{ 0x02be80d3, 0x0eb1, 0x4181, { 0xbd, 0x01, 0x89, 0xf1, 0xc1, 0x14, 0x43, 0x9f } };

	SchedulerGroup::SchedulerGroup() : mainmenu_group_impl(m_guid,
		mainmenu_groups::file_playlist, mainmenu_commands::sort_priority_last)
	{
	}

	//------------------------------------------------------------------------------
	// SchedulerMainPopupMenu
	//------------------------------------------------------------------------------

	// {9505a8f1-7f94-4c7d-ad2f-3c40b81975c8} 
	const GUID SchedulerMainPopupMenu::m_guid = 
	{ 0x9505a8f1, 0x7f94, 0x4c7d, { 0xad, 0x2f, 0x3c, 0x40, 0xb8, 0x19, 0x75, 0xc8 } };

	SchedulerMainPopupMenu::SchedulerMainPopupMenu() : mainmenu_group_popup_impl(m_guid,
		SchedulerGroup::m_guid,	mainmenu_commands::sort_priority_base, "Scheduler")
	{
	}

	//------------------------------------------------------------------------------
	// SchedulerMainPopupCommands
	//------------------------------------------------------------------------------
	
	t_uint32 SchedulerMainPopupCommands::get_command_count()
	{
		return numMenuItems;
	}

	GUID SchedulerMainPopupCommands::get_command(t_uint32 p_index)
	{
		// {7346ec60-7b03-4635-b5e1-aad38bb5e744} 
		static const GUID guidPreferences = 
		{ 0x7346ec60, 0x7b03, 0x4635, { 0xb5, 0xe1, 0xaa, 0xd3, 0x8b, 0xb5, 0xe7, 0x44 } };

		// {569c0186-2951-445e-96ef-7159d8066ec8} 
		static const GUID guidStatusWindow = 
		{ 0x569c0186, 0x2951, 0x445e, { 0x96, 0xef, 0x71, 0x59, 0xd8, 0x06, 0x6e, 0xc8 } };

		// {1D16A6EA-365E-4F8C-9D0F-E5A2BCFBDAA2}
		static const GUID guidStopAllActionLists = 
		{ 0x1d16a6ea, 0x365e, 0x4f8c, { 0x9d, 0xf, 0xe5, 0xa2, 0xbc, 0xfb, 0xda, 0xa2 } };

		if (p_index == miiPreferences)
			return guidPreferences;
		else if (p_index == miiStatusWindow)
			return guidStatusWindow;
		else if (p_index == miiStopAllActionLists)
			return guidStopAllActionLists;

		return pfc::guid_null;
	}

	void SchedulerMainPopupCommands::get_name(t_uint32 p_index, pfc::string_base& p_out)
	{
		if (p_index == miiPreferences)
			p_out = "Preferences";
		else if (p_index == miiStatusWindow)
			p_out = "Status window";
		else if (p_index == miiStopAllActionLists)
			p_out = "Stop all action lists";
	}

	bool SchedulerMainPopupCommands::get_description(t_uint32 p_index, pfc::string_base& p_out)
	{
		if (p_index == miiPreferences)
			p_out = "Opens scheduler preferences page.";
		else if (p_index == miiStatusWindow)
			p_out = "Opens status window.";
		else if (p_index == miiStopAllActionLists)
			p_out = "Stops all action lists.";
		else
			return false;

		return true;
	}

	GUID SchedulerMainPopupCommands::get_parent()
	{
		return SchedulerMainPopupMenu::m_guid;
	}

	t_uint32 SchedulerMainPopupCommands::get_sort_priority()
	{
		return mainmenu_commands::sort_priority_base + 1;
	}

	void SchedulerMainPopupCommands::execute(t_uint32 p_index, service_ptr_t<service_base> p_callback)
	{
		if (p_index == miiPreferences)
			ServiceManager::Instance().GetRootController().ShowPreferencesPage();
		else if (p_index == miiStatusWindow)
			ServiceManager::Instance().GetRootController().ShowStatusWindow();
		else if (p_index == miiStopAllActionLists)
			ServiceManager::Instance().GetRootController().RemoveAllExecSessions();
	}

	//------------------------------------------------------------------------------
	// SchedulerActionListsGroup
	//------------------------------------------------------------------------------
	
	// {243b60f4-e16e-401f-a2be-f3b7dd1239f2} 
	const GUID SchedulerActionListsGroup::m_guid = 
	{ 0x243b60f4, 0xe16e, 0x401f, { 0xa2, 0xbe, 0xf3, 0xb7, 0xdd, 0x12, 0x39, 0xf2 } };

	SchedulerActionListsGroup::SchedulerActionListsGroup() : mainmenu_group_impl(m_guid,
		SchedulerMainPopupMenu::m_guid, mainmenu_commands::sort_priority_last)
	{
	}

	//------------------------------------------------------------------------------
	// SchedulerActionListsCommands
	//------------------------------------------------------------------------------

	t_uint32 SchedulerActionListsCommands::get_command_count()
	{
		return ServiceManager::Instance().GetMenuItemEventsManager().GetNumEvents();
	}

	GUID SchedulerActionListsCommands::get_command(t_uint32 p_index)
	{
		return ServiceManager::Instance().GetMenuItemEventsManager().GetEvent(p_index)->GetGUID();
	}

	void SchedulerActionListsCommands::get_name(t_uint32 p_index, pfc::string_base& p_out)
	{
		MenuItemEvent* pEvent = ServiceManager::Instance().GetMenuItemEventsManager().GetEvent(p_index);
		p_out = pfc::stringcvt::string_utf8_from_wide(pEvent->GetMenuItemName().c_str());
	}

	bool SchedulerActionListsCommands::get_description(t_uint32 p_index, pfc::string_base& p_out)
	{
		get_name(p_index, p_out);
		return true;
	}

	GUID SchedulerActionListsCommands::get_parent()
	{
		return SchedulerActionListsGroup::m_guid;
	}

	t_uint32 SchedulerActionListsCommands::get_sort_priority()
	{
		return mainmenu_commands::sort_priority_base + 1;
	}

	void SchedulerActionListsCommands::execute(t_uint32 p_index, service_ptr_t<service_base> p_callback)
	{
		ServiceManager::Instance().GetMenuItemEventsManager().EmitEvent(p_index);
	}
}