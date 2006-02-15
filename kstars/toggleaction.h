/***************************************************************************
                          toggleaction.h  -  description
                             -------------------
    begin                : Son Feb 10 2002
    copyright            : (C) 2002 by Mark Hollomon
    email                : mhh@mindspring.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTARS_TOGGLEACTION_H
#define KSTARS_TOGGLEACTION_H

/**@class ToggleAction
	*Subclass of Kaction that allows for the action to have a boolean state (on/off)
	*The class stores the current state, and changes the Action's iconset to reflect
	*the state.
	*@author Mark Hollomon
	*@version 1.0
	*/

#include <qicon.h>
#include <kaction.h>

class ToggleAction : public KAction {
	Q_OBJECT
		
	public:
		/**Constructor. */
		ToggleAction(const QString& ontext, const QIcon& onpix, const QString& offtext, 
			const QIcon& offpix, int accel, const QObject* receiver, const char* slot,
			KActionCollection* parent = 0, const char* name = 0 ) ;
		/**Constructor. Same as above, but without icons. */
		ToggleAction(const QString& ontext, const QString& offtext, int accel, 
			const QObject* receiver, const char* slot, KActionCollection* parent = 0, 
			const char* name = 0 ) ;

		/**Sets the ToolTip text for the "on" state. 
			*@param tip the tooltip string
			*/
		void setOnToolTip(const QString &tip);

		/**Sets the ToolTip text for the "off" state. 
			*@param tip the tooltip string
			*/
		void setOffToolTip(const QString &tip);

	public slots:
		/**Put the Action in the "off" state.  Update the icon and tooltip. */
		void turnOff();
		/**Put the Action in the "on" state.  Update the icon and tooltip. */
		void turnOn();

	private:
		QIcon officon;
		QIcon onicon;
		QString offcap;
		QString oncap;
		QString onTip;
		QString offTip;
		bool state;
};


#endif
