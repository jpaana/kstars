#ifndef LX200_16_H
#define LX200_16_H

/*
    LX200 16"
    Copyright (C) 2003 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "lx200autostar.h"

class LX200_16 : public LX200Autostar
{
 public:
  LX200_16();
  ~LX200_16() {}

 virtual void ISGetProperties (const char *dev);
 virtual void ISNewText (IText *t);
 virtual void ISNewNumber (INumber *n);
 virtual void ISNewSwitch (ISwitches *s);
 virtual void ISPoll ();
 virtual void getBasicData();
         void handleAltAzSlew();

 private:

 double currentAlt;
 double currentAz;
 double targetAlt;
 double targetAz;

};

#endif


