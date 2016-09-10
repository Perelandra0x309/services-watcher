/* MainWindow.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICES_WINDOW_H
#define SERVICES_WINDOW_H

#include <InterfaceKit.h>
#include "constants.h"
#include "MainView.h"

class MainWindow : public BWindow {
public:
							MainWindow(BRect, window_look, window_feel, AppOptions);
	virtual	bool			QuitRequested();
			void			OptionsChanged(BMessage*);
private:
	MainView				*fView;
};

#endif
