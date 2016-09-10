/* MainBListView.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICES_MAINBLISTVIEW_H
#define SERVICES_MAINBLISTVIEW_H

#include <InterfaceKit.h>
#include "constants.h"
#include "ServiceListItem.h"
#include "OptionsWindow.h"


class MainBListView : public BListView {
public:
						MainBListView();
			void		AttachedToWindow();
	virtual void		MessageReceived(BMessage*);
	virtual void		MouseDown(BPoint pos);
			void		OptionsChanged(BMessage*);
private:
	BPopUpMenu			*fMenu;
	BMenuItem			*restartMI, *startStopMI, *optionsMI;
};

#endif
