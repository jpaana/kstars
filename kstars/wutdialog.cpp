/***************************************************************************
                          wutdialog.cpp  -  K Desktop Planetarium
                             -------------------
    begin                : Die Feb 25 2003
    copyright            : (C) 2003 by Thomas Kabelmann
    email                : tk78@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "wutdialog.h"

#include "kstars.h"
#include "ksutils.h"
#include "objectnamelist.h"
#include "detaildialog.h"

#include <klocale.h>
#include <klistbox.h>
#include <kpushbutton.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qframe.h>
#include <qtabbar.h>
#include <qtimer.h>
#include <qcursor.h>

WUTDialog::WUTDialog(KStars *ks) :
	KDialogBase (KDialogBase::Plain, i18n("What's up tonight"), Ok, Ok, (QWidget*)ks),
	kstars(ks) {

	objectList = &(ks->data()->ObjNames);
	objectList->setLanguage( ks->options()->useLatinConstellNames );
	today = kstars->data()->CurrentDate;
	// add 1 day to get rise time on next morning
	tomorrow = today + 1.0;
	geo = kstars->geo();

	page = plainPage();
	createSunBox();
	createMoonBox();
	createTabWidget();
	createLayout();
	resize(400, 350);
	makeConnections();

	QTimer::singleShot(0, this, SLOT(init()));
}

WUTDialog::~WUTDialog(){
}

void WUTDialog::createSunBox() {
	// the layout and boxes for sun
	sunBox = new QGroupBox(6, Qt::Horizontal, i18n("Sun"), page, "sunBox");
	QLabel *sunSet = new QLabel(i18n("Set time:"), sunBox);
	sunSet->setAlignment(AlignRight);
	sunSetTimeLabel = new QLabel(i18n("--:--:--"), sunBox);
	sunSetTimeLabel->setAlignment(AlignLeft);
	sunBox->addSpace(0);
	QLabel *sunRise = new QLabel(i18n("Rise time:"), sunBox);
	sunRise->setAlignment(AlignRight);
	sunRiseTimeLabel = new QLabel(i18n("--:--:--"), sunBox);
	sunRiseTimeLabel->setAlignment(AlignLeft);
	sunBox->addSpace(0);
}

void WUTDialog::createMoonBox() {
	// the layout and boxes for moon
	moonBox = new QGroupBox(6, Qt::Horizontal, i18n("Moon"), page, "moonBox");
	QLabel *moonRise = new QLabel(i18n("Rise time:"), moonBox);
	moonRise->setAlignment(AlignRight);
	moonRiseTimeLabel = new QLabel("--:--:--", moonBox);
	moonRiseTimeLabel->setAlignment(AlignLeft);
	moonBox->addSpace(0);
	QLabel *moonSet = new QLabel(i18n("Set time:"), moonBox);
	moonSet->setAlignment(AlignRight);
	moonSetTimeLabel = new QLabel("--:--:--", moonBox);
	moonSetTimeLabel->setAlignment(AlignLeft);
	moonBox->addSpace(0);
}

void WUTDialog::createTabWidget() {
	// tabbar
	tabBar = new QTabBar(page, "tabBar");
	tabBar->addTab(new QTab(i18n("Solar System")));
	tabBar->addTab(new QTab(i18n("Stars")));
	tabBar->addTab(new QTab(i18n("Constellations")));
	tabBar->addTab(new QTab(i18n("Asteroids/Comets")));
	tabBar->addTab(new QTab(i18n("Cluster/Nebulae/Galaxies")));

	// frame
	frame = new QFrame(page, "tabFrame");
	frame->setFrameShape(QFrame::TabWidgetPanel);
	frame->setMargin(10);

	// listbox
	objectListBox = new KListBox(frame, "objectListBox");
	objectListBox->setMinimumSize(300, 250);

	// right text labels
	QLabel *riseLabel = new QLabel(i18n("Rise time:"), frame);
	riseLabel->setAlignment(AlignRight);
	riseTimeLabel = new QLabel(i18n("--:--:--"), frame);
	riseTimeLabel->setAlignment(AlignLeft);
	QLabel *setLabel = new QLabel(i18n("Set time:"), frame);
	setLabel->setAlignment(AlignRight);
	setTimeLabel = new QLabel(i18n("--:--:--"), frame);
	setTimeLabel->setAlignment(AlignLeft);

	// information button
	detailsButton = new KPushButton(i18n("More Information"), frame, "detailsButton");
	detailsButton->setFixedSize(180, 40);

	// the layouts
	QHBoxLayout *frameLayout = new QHBoxLayout(frame);
	QVBoxLayout *rightVLayout = new QVBoxLayout();

	// toplevel layout
	frameLayout->setMargin(10);
	frameLayout->addWidget(objectListBox);
	frameLayout->addSpacing(40);
	frameLayout->addStretch();
	frameLayout->addLayout(rightVLayout);

	// rise time layout
	QHBoxLayout *riseLayout = new QHBoxLayout();
	riseLayout->addWidget(riseLabel);
	riseLayout->addWidget(riseTimeLabel);

	// set time layout
	QHBoxLayout *setLayout = new QHBoxLayout();
	setLayout->addWidget(setLabel);
	setLayout->addWidget(setTimeLabel);

	// right layout wiht labels and detail button
	rightVLayout->addLayout(riseLayout);
	rightVLayout->addLayout(setLayout);
	rightVLayout->addWidget(detailsButton);
}

void WUTDialog::createLayout() {
	// dialog layout
	vlay = new QVBoxLayout(page);
	vlay->setMargin(10);
	vlay->addWidget(sunBox);
	vlay->addSpacing(15);
	vlay->addWidget(moonBox);
	vlay->addSpacing(20);
	vlay->addWidget(tabBar);
	vlay->addWidget(frame);
}

void WUTDialog::makeConnections() {
	connect(tabBar, SIGNAL(selected(int)), SLOT(loadList(int)));
	connect(objectListBox, SIGNAL(selectionChanged(QListBoxItem*)), SLOT(updateTimes(QListBoxItem*)));
	connect(detailsButton, SIGNAL(clicked()), SLOT(slotDetails()));
}

void WUTDialog::init() {
	// reset all lists
	for (int i=0; i<5; i++) lists.initialized[i] = false;

	// sun informations
	SkyObject *o = (SkyObject*) kstars->data()->PC->planetSun();
	sunRiseTomorrow = o->riseTime(tomorrow, geo);
	sunSetToday = o->setTime(today, geo);
	sunRiseToday = o->riseTime(today, geo);
	sunSetTimeLabel->setText(sunSetToday.toString("hh:mm"));
	sunRiseTimeLabel->setText(sunRiseTomorrow.toString("hh:mm") + " (" + i18n("tomorrow") + ")");
	// moon informations
	o = (SkyObject*) kstars->data()->Moon;
	moonRise = o->riseTime(today, geo);
	moonSet = o->setTime(tomorrow, geo);
	moonRiseTimeLabel->setText(moonRise.toString("hh:mm"));
	moonSetTimeLabel->setText(moonSet.toString("hh:mm"));
	splitObjectList();
	// load first list
	loadList(0);
}

void WUTDialog::splitObjectList() {
	// don't append objects which are never visible
	for (SkyObjectName *oname=objectList->first(); oname; oname=objectList->next()) {
		bool visible = true;
		SkyObject *o = oname->skyObject();
		// is object circumpolar or never visible
		if (o->checkCircumpolar(geo->lat()) == true) {
			// never visible
			if (o->alt()->Degrees() <= 0) visible = false;
		}
		if (visible == true) appendToList(oname);
	}
}

void WUTDialog::appendToList(SkyObjectName *o) {
	// split into several lists
	switch (o->skyObject()->type()) {
		case SkyObject::PLANET						: lists.visibleList[0].append(o); break;
		case SkyObject::STAR							: lists.visibleList[1].append(o); break;
		//case SkyObject::CATALOG_STAR			: //Omitting CATALOG_STARs from list
		case SkyObject::COMET							:
		case SkyObject::ASTEROID					: lists.visibleList[3].append(o); break;
		case SkyObject::OPEN_CLUSTER			:
		case SkyObject::GLOBULAR_CLUSTER	:
		case SkyObject::GASEOUS_NEBULA		:
		case SkyObject::PLANETARY_NEBULA	:
		case SkyObject::SUPERNOVA_REMNANT	:
		case SkyObject::GALAXY						: lists.visibleList[4].append(o); break;
		// constellations
		default : lists.visibleList[2].append(o);
	}
}

void WUTDialog::loadList(int i) {
	objectListBox->clear();
	setCursor(QCursor(Qt::WaitCursor));
	QPtrList <SkyObjectName> invisibleList;
	for (SkyObjectName *oname=lists.visibleList[i].first(); oname; oname=lists.visibleList[i].next()) {
		bool visible = true;
		if (lists.initialized[i] == false) {
			visible = checkVisibility(oname);
			// don't show the sun
			if (i == 0) {
				if (oname->skyObject()->name() == "Sun") visible = false;
			}
			if (visible == false) {
				// collect all invisible objects
				invisibleList.append(oname);
			}
		}
		// append to listbox
		if (visible == true) new SkyObjectNameListItem(objectListBox, oname);
	}
	// remove all invisible objects from list
	if (invisibleList.isEmpty() == false) {
		for (SkyObjectName *o=invisibleList.first(); o; o=invisibleList.next()) {
			lists.visibleList[i].removeRef(o);
		}
	}
	setCursor(QCursor(Qt::ArrowCursor));
	lists.initialized[i] = true;
	// highlight first item
	objectListBox->setSelected(0, true);
	objectListBox->setFocus();
}

bool WUTDialog::checkVisibility(SkyObjectName *oname) {
	bool visible( false );
	double minAlt = 20.0; //minimum altitude for object to be considered 'visible'
	SkyPoint sp = (SkyPoint)*(oname->skyObject()); //local copy of skyObject's position
	
	QDateTime ssDT( kstars->data()->LTime.date(), sunSetToday );
	QDateTime srDT( kstars->data()->LTime.date().addDays(1), sunRiseTomorrow );
	
	for ( QDateTime test = ssDT; test < srDT; test = test.addSecs(3600) ) {
		//Need LST of the test time, expressed as a dms object.
		QDateTime ut = test.addSecs( int( -3600*geo->TZ() ) );
		QTime lst = KSUtils::UTtoLST( ut, geo->lng() );
		dms LSTh;
		LSTh.setH( lst.hour(), lst.minute(), lst.second() );
		
		//check altitude of object at this time.
		sp.EquatorialToHorizontal( &LSTh, geo->lat() );
		
		if ( sp.alt()->Degrees() > minAlt ) {
			visible = true;
			break;
		}
	}
	
	return visible;
}

/*
bool WUTDialog::checkVisibility(SkyObjectName *oname) {
	bool visible = false;
	// circumpolar objects
	if (oname->skyObject()->checkCircumpolar(geo->lat()) == true) {
		visible = true;
	} else {
		// non circumpolar objects
		QTime riseTimeToday = oname->skyObject()->riseTime(today, geo);
		QTime setTimeToday, setTimeTomorrow;
		// if rise time is invalid => circumpolar
		// object rises after sunset => visible
		if (riseTimeToday.isValid() == false || riseTimeToday > sunSetToday) {
			visible = true;
		} else {
			// object set time is after sunset => visible
			setTimeToday = oname->skyObject()->setTime(today, geo);
			if (setTimeToday > sunSetToday) {
				visible = true;
			} else {
				// object rises after midnight but before sunrise => visible
				riseTimeToday = oname->skyObject()->riseTime(today, geo);
				if (riseTimeToday < sunRiseTomorrow) {
					visible = true;
				} else {
					// object set time is before sunrise => visible
					setTimeTomorrow = oname->skyObject()->setTime(tomorrow, geo);
					if (setTimeTomorrow < sunRiseTomorrow) {
						visible = true;
					} else {
						// rises before sunset and set after sun rise tomorrow
						if (riseTimeToday > setTimeTomorrow && riseTimeToday < sunSetToday && setTimeTomorrow > sunRiseTomorrow) {
							visible = true;
						}
					}
				}
			}
		}
	}
	return visible;
}
*/

void WUTDialog::updateTimes(QListBoxItem *item) {
	QString rise, set;
	// update the time labels of current object
	if (item == 0) {
		// no object selected
		rise = QString("--:--:--");
		set = QString("--:--:--");
	} else {
		// get times
		QTime riseTime = ((SkyObjectNameListItem*)item)->objName()->skyObject()->riseTime(today, geo);
		QTime setTime = ((SkyObjectNameListItem*)item)->objName()->skyObject()->setTime(today, geo);
		// we assume that object rises and sets today
		bool riseToday = true, setToday = true;
		// object set time is tomorrow
		if ((riseTime < sunSetToday && riseTime < sunRiseToday) && riseTime < setTime) {
			riseTime = ((SkyObjectNameListItem*)item)->objName()->skyObject()->riseTime(tomorrow, geo);
			riseToday = false;
		}
		// if object set time is on morning, take set time from tomorrow
		if (setTime < riseTime || riseToday == false) {
			setTime = ((SkyObjectNameListItem*)item)->objName()->skyObject()->setTime(tomorrow, geo);
			setToday = false;
		}
		if (riseTime.isValid()) {
			rise = riseTime.toString("hh:mm");
			rise.append(" (");
			if (riseToday == true)
				rise.append(i18n("today"));
			else
				rise.append(i18n("tomorrow"));
			rise.append(")");
		} else {
			rise = QString(i18n("Circumpolar"));
		}
		if (setTime.isValid()) {
			set = setTime.toString("hh:mm");
			set.append(" (");
			if (setToday == true)
				set.append(i18n("today"));
			else
				set.append(i18n("tomorrow"));
			set.append(")");
		} else {
			set = QString(i18n("Circumpolar"));
		}
	}
	riseTimeLabel->setText(rise);
	setTimeLabel->setText(set);
}

void WUTDialog::slotDetails() {
	SkyObject *o = 0;
	// get selected item
	if (objectListBox->selectedItem() != 0) {
		o = ((SkyObjectNameListItem*)objectListBox->selectedItem())->objName()->skyObject();
	}
	if (o != 0) {
		DetailDialog detail(o, kstars->data()->LTime, geo, kstars);
		detail.exec();
	}
}

#include "wutdialog.moc"
