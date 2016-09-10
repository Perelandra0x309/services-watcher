/* MainWindow.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "MainWindow.h"

MainWindow::MainWindow(BRect size, window_look look, window_feel feel, AppOptions options)
	:
	BWindow(size, "Services Watcher", look, feel, B_NOT_ZOOMABLE)
{
	Lock();
	fView = new MainView(Bounds(), options);
	AddChild(fView);
	Unlock();
}


bool
MainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::OptionsChanged(BMessage *msg)
{
	fView->OptionsChanged(msg);
}
