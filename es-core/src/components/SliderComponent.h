#pragma once
#ifndef ES_CORE_COMPONENTS_SLIDER_COMPONENT_H
#define ES_CORE_COMPONENTS_SLIDER_COMPONENT_H

#include "components/ImageComponent.h"
#include "GuiComponent.h"

class Font;
class TextCache;

// Used to display/edit a value between some min and max values.
class SliderComponent : public GuiComponent
{
public:
	//Minimum value (far left of the slider), maximum value (far right of the slider), increment size (how much just pressing L/R moves by), unit to display (optional).
	SliderComponent(Window* window, float min, float max, float increment, const std::string& suffix = "");

	void setValue(float val, bool callhandler = true);
	float getValue();

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void render(const Transform4x4f& parentTrans) override;

	void onSizeChanged() override;

	virtual std::vector<HelpPrompt> getHelpPrompts() override;

	void setChangeHandler(const std::function<void(int)>& hndl) { mChangeHandler = hndl; }

private:
	void onValueChanged();

	float mMin, mMax;
	float mValue;
	float mSingleIncrement;
	float mMoveRate;
	int mMoveAccumulator;

	ImageComponent mKnob;

	std::string mSuffix;
	std::shared_ptr<Font> mFont;
	std::shared_ptr<TextCache> mValueCache;

	std::function<void(int)> mChangeHandler = nullptr;
};

#endif // ES_CORE_COMPONENTS_SLIDER_COMPONENT_H
