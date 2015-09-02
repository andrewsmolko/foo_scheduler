#include "pch.h"
#include "definitions.h"
#include "foobar_services.h"
#include "pref_page.h"

DECLARE_COMPONENT_VERSION(PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_ABOUT)
VALIDATE_COMPONENT_FILENAME("foo_scheduler.dll")

// {d1321dbc-9511-43e1-a0ee-5f9be0402ad0} 
static const GUID g_guidPluginCfg =
	{0xd1321dbc, 0x9511, 0x43e1, {0xa0, 0xee, 0x5f, 0x9b, 0xe0, 0x40, 0x2a, 0xd0}};

static PluginConfiguration g_cfgPlugin(g_guidPluginCfg);

static initquit_factory_t<PluginInitQuit> foo_initquit;

static mainmenu_commands_factory_t<MainMenuItems::SchedulerGroup> foo_menu_SchedulerGroup;
static mainmenu_commands_factory_t<MainMenuItems::SchedulerMainPopupMenu> foo_menu_SchedulerMainPopupMenu;
static mainmenu_commands_factory_t<MainMenuItems::SchedulerMainPopupCommands> foo_menu_SchedulerMainPopupCommands;
static mainmenu_commands_factory_t<MainMenuItems::SchedulerActionListsGroup> foo_menu_SchedulerActionListsGroup;
static mainmenu_commands_factory_t<MainMenuItems::SchedulerActionListsCommands> foo_menu_SchedulerActionListsCommands;

static preferences_page_factory_t<PreferencesPageImpl> foo_preferences_page_myimpl_factory;