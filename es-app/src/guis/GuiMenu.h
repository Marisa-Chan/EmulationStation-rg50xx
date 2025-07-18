#pragma once
#ifndef ES_APP_GUIS_GUI_MENU_H
#define ES_APP_GUIS_GUI_MENU_H

#include "components/MenuComponent.h"
#include "GuiComponent.h"
#include "components/OptionListComponent.h"
#include "FileData.h"

class GuiMenu : public GuiComponent
{
public:
	GuiMenu(Window* window);
	virtual ~GuiMenu();

	bool input(InputConfig* config, Input input) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override;
	HelpStyle getHelpStyle() override;

private:
	void addEntry(const char* name, unsigned int color, bool add_arrow, const std::function<void()>& func);
	void addVersionInfo();
	void openCollectionSystemSettings();
	void openConfigInput();
	void openOtherSettings();
	void openQuitMenu();
	void openScraperSettings();
	void openScreensaverOptions();
	void openSoundSettings();
	void openUISettings();

	inline void addSaveFunc(const std::function<void()>& func) { mSaveFuncs.push_back(func); };
	void save();

	MenuComponent mMenu;
	TextComponent mVersion;

	typedef OptionListComponent<const FileData::SortType*> SortList;
	std::shared_ptr<SortList> mListSort;

	std::vector< std::function<void()> > mSaveFuncs;
};

#endif // ES_APP_GUIS_GUI_MENU_H
