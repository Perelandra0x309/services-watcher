/* ServicesApp.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICES_APP_H
#define SERVICES_APP_H

#include <Application.h>
#include "MainWindow.h"
#include "OptionsWindow.h"

class ServicesApp : public BApplication {
public:
					ServicesApp();
	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage*);
			void	AboutRequested();
private:
	MainWindow		*fWindow;
	OptionsWindow	*fOptions;
};

#endif
