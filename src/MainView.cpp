/* MainView.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "MainView.h"

MainView::MainView(BRect size, AppOptions options)
	:
	BView(size, "Services", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS),
	fWatchingRoster(false)
{
	fListView = new MainBListView();
	fListView->SetFontSize(options.fontSize);
	fScrollView = new BScrollView("Scroll View", fListView, B_FOLLOW_ALL_SIDES,
			false, true);

	// Get all the system servers
	BPath serversDirPath;
	find_directory(B_BEOS_SERVERS_DIRECTORY, &serversDirPath);
	BDirectory serversDir(serversDirPath.Path());
	serversDir.Rewind();
	BEntry serverEntry;
	while(serversDir.GetNextEntry(&serverEntry) == B_OK)
	{
		_AddServiceListItem(serverEntry, &options);
	}

	// Add other services
	BEntry customEntry("/boot/system/Deskbar");
	_AddServiceListItem(customEntry, &options);
	customEntry.SetTo("/boot/system/Tracker");
	_AddServiceListItem(customEntry, &options);

	SetLayout(new BGroupLayout(B_VERTICAL));
	AddChild(BGroupLayoutBuilder(B_VERTICAL)
		.Add(fScrollView)
		.SetInsets(3, 3, 3, 3)
	);
}


MainView::~MainView()
{
	//Need to stop watching the roster
	if(fWatchingRoster)
		be_roster->StopWatching(BMessenger(this));
}


void
MainView::AllAttached()
{
	BView::AllAttached();

	//Start watching the application roster for launches and quits of services
	status_t result = be_roster->StartWatching(BMessenger(this));
	if(result  != B_OK)
	{	//roster failed to be watched.  Show warning.
		(new BAlert("Watching Warning", "Warning: This app was not able to succeesfully start "
					"watching the roster for aplication quit and launch messages.  "
					"The status of the system services may not be up to date at any time.", "OK"))->Go();
	}
	else//watching was sucessful
		fWatchingRoster = true;

	// update all list items status
	int count = fListView->CountItems();
	for(int index=0; index<count; index++)
	{
		ServiceListItem *item = static_cast<ServiceListItem *>(fListView->ItemAt(index));
		item->SetRunningState();
	}
}


void
MainView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case B_SOME_APP_QUIT:
		case B_SOME_APP_LAUNCHED:
		{
			const char* sig;
			if (msg->FindString("be:signature", &sig) != B_OK)
				break;
			ServiceListItem *item = NULL;
			int count = fListView->CountItems();
			for(int index=0; index<count; index++)
			{
				item = static_cast<ServiceListItem *>(fListView->ItemAt(index));
				if(strcmp(sig,item->GetSignature()) == 0)
				{
					item->ProcessMessage(msg);
					fListView->InvalidateItem(index);
				}
			}
		}
		default:
			BView::MessageReceived(msg);
	}
}


void
MainView::OptionsChanged(BMessage *msg)
{
	fListView->OptionsChanged(msg);
}


status_t
MainView::_AddServiceListItem(BEntry serviceEntry, AppOptions *options)
{
	BNode serviceNode;
	BNodeInfo serviceNodeInfo;
	char nodeType[B_MIME_TYPE_LENGTH];
	attr_info info;
	BString sig;
	if( (serviceNode.SetTo(&serviceEntry)) != B_OK)
		return B_ERROR;
	if( (serviceNodeInfo.SetTo(&serviceNode)) != B_OK)
		return B_ERROR;
	if( (serviceNodeInfo.GetType(nodeType)) != B_OK ||
			strcmp(nodeType, "application/x-vnd.Be-elfexecutable") != 0)
		return B_ERROR;
	if(serviceNode.GetAttrInfo("BEOS:APP_SIG", &info) != B_OK)
		return B_ERROR;
	serviceNode.ReadAttrString("BEOS:APP_SIG", &sig);
	ServiceListItem *newItem = new ServiceListItem(serviceEntry, sig.String(), options);
	status_t initStatus = newItem->InitStatus();
	if(initStatus == B_OK)
		fListView->AddItem(newItem);
	else
		delete newItem;
	return initStatus;
}

