#pragma once

class LeiSaiBac332E;

class ColimateController
{
public:
	ColimateController(const QString& _moduleFileDirectory);
	~ColimateController(void);

	bool switchFrontVerticalToPanel();
	bool switchFrontHorizontalToPanel();
	bool switchFrontVerticalToLine();
	bool switchFrontHorizontalToLine();
	bool changeFrontVerticalWidth(float _width);
	bool changeFrontHorizontalWidth(float _width);

	bool switchPanelVerticalToPanel();
	bool switchPanelHorizontalToPanel();
	bool switchPanelVerticalToLine();
	bool switchPanelHorizontalToLine();
	bool changePanelVerticalWidth(float _width);
	bool changePanelHorizontalWidth(float _width);

	bool swithcToPanel();
	bool swithcToLine();
	bool changeLineThickness(float _thickNess);

	int getColimateMode();
	bool getColimateSwitching();
	bool homeAllAxes();
	void homeAllAxesAndSwitchToLine();
	void homeAllAxesAndSwitchToPanel();
	void homeAndSwitchThread(int lineOrPanel);
	void stopAll();

	LeiSaiBac332E* getMotor()
	{
		return leiSaiMotor;
	};

private:
	LeiSaiBac332E* leiSaiMotor;
	bool homeAndSwitchThreadRun;
	int linePanelMode;
};

