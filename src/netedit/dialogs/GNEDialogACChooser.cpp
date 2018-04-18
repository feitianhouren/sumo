/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialogACChooser.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2018
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fxkeys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewParent.h>

#include "GNEDialogACChooser.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEDialogACChooser) GNEDialogACChooserMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CENTER, GNEDialogACChooser::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,         GNEDialogACChooser::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_TEXT,   GNEDialogACChooser::onChgText),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_TEXT,   GNEDialogACChooser::onCmdText),
    FXMAPFUNC(SEL_KEYPRESS, MID_CHOOSER_LIST,   GNEDialogACChooser::onListKeyPress),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER, GNEDialogACChooser::onCmdFilter),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT, GNEDialogACChooser::onCmdToggleSelection),
};

FXIMPLEMENT(GNEDialogACChooser, FXMainWindow, GNEDialogACChooserMap, ARRAYNUMBER(GNEDialogACChooserMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEDialogACChooser::GNEDialogACChooser(GNEViewParent* viewParent, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs):
    FXMainWindow(viewParent->getApp(), title.c_str(), icon, NULL, GUIDesignChooserDialog),
    myViewParent(viewParent) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    // build the list
    FXVerticalFrame* layout1 = new FXVerticalFrame(hbox, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP, 0, 0, 0, 0, 4, 4, 4, 4);
    myTextEntry = new FXTextField(layout1, 0, this, MID_CHOOSER_TEXT, LAYOUT_FILL_X | FRAME_THICK | FRAME_SUNKEN);
    FXVerticalFrame* style1 = new FXVerticalFrame(layout1, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP | FRAME_THICK | FRAME_SUNKEN, 0, 0, 0, 0, 0, 0, 0, 0);
    myList = new FXList(style1, this, MID_CHOOSER_LIST, LAYOUT_FILL_X | LAYOUT_FILL_Y | LIST_SINGLESELECT | FRAME_SUNKEN | FRAME_THICK);
    // first fill myACsByID to sort ACs by tags
    for (auto i : ACs) {
        myACsByID.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
    }
    // iterate over ACsByID and fill list
    for (auto i : myACsByID) {
        // set icon
        FXIcon* selectIcon = GNEAttributeCarrier::parse<bool>(i.second->getAttribute(GNE_ATTR_SELECTED)) ? GUIIconSubSys::getIcon(ICON_FLAG) : 0;
        myACs[myList->appendItem(i.first.c_str(), selectIcon)] = i.second;
    }
    // build the buttons
    FXVerticalFrame* layout = new FXVerticalFrame(hbox, LAYOUT_TOP, 0, 0, 0, 0, 4, 4, 4, 4);
    myCenterButton = new FXButton(layout, "Center\t\t", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_CHOOSER_CENTER, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layout, GUIDesignHorizontalSeparator);
    new FXButton(layout, "&Hide Unselected\t\t", GUIIconSubSys::getIcon(ICON_FLAG), this, MID_CHOOSER_FILTER, GUIDesignChooserButtons);
    new FXButton(layout, "&Select/deselect\tSelect/deselect current object\t", GUIIconSubSys::getIcon(ICON_FLAG), this, MID_CHOOSEN_INVERT, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layout, GUIDesignHorizontalSeparator);
    new FXButton(layout, "&Close\t\t", GUIIconSubSys::getIcon(ICON_NO), this, MID_CANCEL, GUIDesignChooserButtons);
}


GNEDialogACChooser::~GNEDialogACChooser() {}


void
GNEDialogACChooser::show() {
    FXMainWindow::show();
    myTextEntry->setFocus();
}


long
GNEDialogACChooser::onCmdCenter(FXObject*, FXSelector, void*) {
    int selected = myList->getCurrentItem();
    if (selected >= 0) {
        myViewParent->setView(dynamic_cast<GUIGlObject*>(myACs[selected])->getGlID());
    }
    return 1;
}


long
GNEDialogACChooser::onCmdClose(FXObject*, FXSelector, void*) {
    close(true);
    return 1;
}


long
GNEDialogACChooser::onChgText(FXObject*, FXSelector, void*) {
    int id = myList->findItem(myTextEntry->getText(), -1, SEARCH_PREFIX);
    if (id < 0) {
        if (myList->getNumItems() > 0) {
            myList->deselectItem(myList->getCurrentItem());
        }
        myCenterButton->disable();
        return 1;
    }
    myList->deselectItem(myList->getCurrentItem());
    myList->makeItemVisible(id);
    myList->selectItem(id);
    myList->setCurrentItem(id, true);
    myCenterButton->enable();
    return 1;
}


long
GNEDialogACChooser::onCmdText(FXObject*, FXSelector, void*) {
    int current = myList->getCurrentItem();
    if (current >= 0 && myList->isItemSelected(current)) {
        myViewParent->setView(dynamic_cast<GUIGlObject*>(myACs[current])->getGlID());
    }
    return 1;
}


long
GNEDialogACChooser::onListKeyPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    switch (event->code) {
        case KEY_Return:
            onCmdText(0, 0, 0);
            break;
        default:
            break;
    }
    return 1;
}


long
GNEDialogACChooser::onCmdFilter(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(ICON_FLAG);
    std::set<std::pair<std::string, GNEAttributeCarrier*> > selectedACs;
    // iterate over myACs to check what has the selected flag
    for (auto i : myACs) {
        if (myList->getItemIcon(i.first) == flag) {
            myACsByID.insert(std::pair<std::string, GNEAttributeCarrier*>(i.second->getID(), i.second));
        }
    }
    // clear list and myACs
    myList->clearItems();
    myACs.clear();
    // iterate over ACsByID and fill list again only with the selected elements
    for (auto i : selectedACs) {
        // set icon
        FXIcon* selectIcon = GNEAttributeCarrier::parse<bool>(i.second->getAttribute(GNE_ATTR_SELECTED)) ? GUIIconSubSys::getIcon(ICON_FLAG) : 0;
        myACs[myList->appendItem(i.first.c_str(), selectIcon)] = i.second;
    }
    myList->update();
    return 1;
}

long
GNEDialogACChooser::onCmdToggleSelection(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(ICON_FLAG);
    int i = myList->getCurrentItem();
    if (i >= 0) {
        if(GNEAttributeCarrier::parse<bool>(myACs[i]->getAttribute(GNE_ATTR_SELECTED))) {
            myACs[i]->setAttribute(GNE_ATTR_SELECTED, "false", dynamic_cast<GNEViewNet*>(myViewParent->getView())->getUndoList());
            myList->setItemIcon(i, 0);
        } else {
            myACs[i]->setAttribute(GNE_ATTR_SELECTED, "true", dynamic_cast<GNEViewNet*>(myViewParent->getView())->getUndoList());
            myList->setItemIcon(i, flag);
        }
    }
    myList->update();
    myViewParent->getView()->update();
    return 1;
}


/****************************************************************************/

