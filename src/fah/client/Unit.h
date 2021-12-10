/******************************************************************************\

                  This file is part of the Folding@home Client.

          The fah-client runs Folding@home protein folding simulations.
                    Copyright (c) 2001-2020, foldingathome.org
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

#include "UnitState.h"
#include "FrameTimer.h"

#include <cbang/json/Observable.h>

#include <cbang/event/Event.h>
#include <cbang/event/Enum.h>
#include <cbang/event/OutgoingRequest.h>

#include <cbang/os/Subprocess.h>
#include <cbang/os/Thread.h>


namespace FAH {
  namespace Client {
    class App;
    class GPUResource;
    class Core;

    class Unit :
      public cb::JSON::ObservableDict,
      public cb::Event::Enum,
      public UnitState::Enum {
      App &app;

      cb::SmartPointer<cb::Event::Event> event;
      cb::SmartPointer<cb::Event::OutgoingRequest> pr;

      uint64_t wu;
      std::string id;

      cb::JSON::ValuePtr data;
      cb::SmartPointer<Core> core;

      unsigned viewerFrame = 0;
      FrameTimer frameTimer;

      cb::SmartPointer<cb::Subprocess> process;
      cb::SmartPointer<cb::Thread> logCopier;

      uint32_t totalPausedTime; // Total time unit was paused.
      uint32_t timesPaused; // No of times unit was paused.
      uint32_t lastPausedTime; // last time unit was paused.
      bool success = false;
      unsigned retries = 0;
      uint64_t wait = 0;

      Unit(App &app);

    public:
      Unit(App &app, uint64_t wu, uint32_t cpus,
           const std::set<std::string> &gpus, uint64_t projectKey);
      Unit(App &app, const cb::JSON::ValuePtr &data);
      ~Unit();

      const std::string &getID() const {return id;}
      unsigned getRetries() const {return retries;}

      void setState(UnitState state);
      UnitState getState() const;

      uint64_t getProjectKey() const;

      bool isWaiting() const;
      bool isPaused() const;
      void setPause(bool pause);
      const char *getPauseReason() const;

      uint32_t getCPUs() const {return getU32("cpus");}
      const cb::JSON::ValuePtr &getGPUs() const {return get("gpus");}

      double getEstimatedProgress() const;
      double getCurrentFrameProgress() const;
      uint64_t getRunTimeEstimate() const;
      double getCreditEstimate() const;
      uint64_t getETA() const;
      double getPPD() const;

      std::string getLogPrefix() const;
      std::string getDirectory() const {return "work/" + id;}
      std::string getWSBaseURL() const;
      uint64_t getDeadline() const;
      bool isFinished() const;
      bool isExpired() const;

      void triggerNext(double secs = 0);

    protected:
      void next();

      void setProgress(unsigned complete, int total);
      void getCore();
      void run();
      void readInfo();
      void readViewerTop();
      void readViewerFrame();
      void readResults();
      bool finalizeRun();
      void monitor();
      void dump();
      void clean();
      void retry();

      void save();
      void remove();

      void assignResponse(const cb::JSON::ValuePtr &data);
      void writeProjectRestrictions(cb::JSON::Sink &sink,
                                    const cb::JSON::ValuePtr &project);
      void writeRequest(cb::JSON::Sink &sink);
      void assign();
      void downloadResponse(const cb::JSON::ValuePtr &data);
      void download();
      void uploadResponse(const cb::JSON::ValuePtr &data);
      void upload();
      void response(cb::Event::Request &req);
    };
  }
}
