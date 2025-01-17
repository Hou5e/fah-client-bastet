/******************************************************************************\

                  This file is part of the Folding@home Client.

          The fah-client runs Folding@home protein folding simulations.
                    Copyright (c) 2001-2023, foldingathome.org
                               All rights reserved.

       This program is free software; you can redistribute it and/or modify
       it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 3 of the License, or
                       (at your option) any later version.

         This program is distributed in the hope that it will be useful,
          but WITHOUT ANY WARRANTY; without even the implied warranty of
          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                   GNU General Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
           51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

                  For information regarding this software email:
                                 Joseph Coffland
                          joseph@cauldrondevelopment.com

\******************************************************************************/

#pragma once

#include <cbang/SmartPointer.h>
#include <cbang/event/Event.h>
#include <atomic>


namespace FAH {
  namespace Client {
    class App;

    class OS {
      App &app;

      bool idle = false;

      std::atomic<bool> paused;
      std::atomic<bool> active;
      std::atomic<bool> failure;

    protected:
      cb::SmartPointer<cb::Event::Event> event;

    public:
      OS(App &app);
      virtual ~OS() {}

      static cb::SmartPointer<OS> create(App &app);

      virtual const char *getName() const = 0;
      virtual const char *getCPU() const;
      virtual bool isSystemIdle() const = 0;
      virtual void dispatch();

      bool isPaused()   const {return paused;}
      bool isActive()   const {return active;}
      bool hasFailure() const {return failure;}
      void requestExit() const;
      void togglePause() const;

    protected:
      void update();
    };
  }
}
