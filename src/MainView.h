/* MainView.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICES_MAIN_VIEW_H
#define SERVICES_MAIN_VIEW_H

#include <InterfaceKit.h>
#include <SupportKit.h>
#include <StorageKit.h>
#include <fs_attr.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <Roster.h>
#include "constants.h"
#include "MainBListView.h"
#include "ServiceListItem.h"


class MainView : public BView {
public:
					MainView(BRect, AppOptions);
					~MainView();
	virtual void	AllAttached();
	virtual void	MessageReceived(BMessage*);
			void	OptionsChanged(BMessage*);
private:
	BScrollView		*fScrollView;
	MainBListView	*fListView;
	bool			fWatchingRoster;
	status_t		_AddServiceListItem(BEntry serviceEntry, AppOptions *options);

};

#endif
