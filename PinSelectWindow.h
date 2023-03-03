#pragma once



void pinSelectWindowCreate(int deviceReference);
int pinSelectWindowActive(void);
void createPinSelectWidgets(int deviceReference);


// Handle any widget messages
static int pinSelectWidgetsHandler(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2);

