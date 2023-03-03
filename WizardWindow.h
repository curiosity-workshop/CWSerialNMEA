#pragma once



void wizardWindowCreate(void);
int wizardWindowActive(void);
void CreateWizardWidgets(void);


// Handle any widget messages
static int wizardWidgetsHandler(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2);

