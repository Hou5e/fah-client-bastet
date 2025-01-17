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

#include "UnitState.h"

#include <cbang/json/Observable.h>

#include <cbang/event/Event.h>
#include <cbang/event/Enum.h>
#include <cbang/event/OutgoingRequest.h>

#include <cbang/os/Subprocess.h>
#include <cbang/os/Thread.h>

#include <cbang/net/URI.h>


namespace FAH {
  namespace Client {
    class App;
    class Units;
    class GPUResource;
    class Core;
    class Config;

    class Unit :
      public cb::JSON::ObservableDict,
      public cb::Event::Enum,
      public UnitState::Enum {
      App &app;
      cb::SmartPointer<Units> units;

      cb::SmartPointer<cb::Event::Event>           event;
      cb::SmartPointer<cb::Event::OutgoingRequest> pr;

      uint64_t    wu;
      std::string id;

      cb::JSON::ValuePtr              data;
      cb::JSON::ValuePtr              topology;
      std::vector<cb::JSON::ValuePtr> frames;
      cb::SmartPointer<Core>          core;

      unsigned viewerFrame = 0;

      cb::SmartPointer<cb::Subprocess> process;
      cb::SmartPointer<cb::Thread>     logCopier;

      bool     success     = false;
      unsigned retries     = 0;
      double   wait        = 0;
      int      cs          = -1;
      uint32_t runningCPUs = 0;

      uint64_t processStartTime     = 0; // Core process start time
      uint64_t processInterruptTime = 0; // Core process interrupt time
      uint64_t lastProcessTimer     = 0; // For detecting clock skew
      int64_t  clockSkew            = 0; // Due to sleeping or clock changes

      uint64_t lastKnownDone                  = 0;
      uint64_t lastKnownTotal                 = 0;
      uint64_t lastKnownProgressUpdate        = 0;
      uint64_t lastKnownProgressUpdateRunTime = 0;

      Unit(App &app);

    public:
      Unit(App &app, uint64_t wu, const std::string &group, uint32_t cpus,
           const std::set<std::string> &gpus);
      Unit(App &app, const cb::JSON::ValuePtr &data);
      ~Unit();

      void setUnits(const cb::SmartPointer<Units> &units);
      const Config &getConfig() const;

      std::string getGroup() const;
      const std::string &getID() const {return id;}
      cb::JSON::ValuePtr getTopology() const {return topology;}
      const std::vector<cb::JSON::ValuePtr> &getFrames() const {return frames;}
      unsigned getRetries() const {return retries;}

      UnitState getState() const;
      bool atRunState() const;
      bool hasRun() const;

      uint64_t getProjectKey() const;

      bool isWaiting() const;
      bool isPaused() const;
      void setPause(bool pause);
      const char *getPauseReason() const;
      bool isRunning() const;

      void setCPUs(uint32_t cpus);
      uint32_t getCPUs() const {return getU32("cpus");}
      uint32_t getMinCPUs() const;
      uint32_t getMaxCPUs() const;
      void setGPUs(const std::set<std::string> &gpus);
      std::set<std::string> getGPUs() const;
      bool hasGPU(const std::string &id) const;
      bool hasGPUs() const {return !getGPUs().empty();}

      uint64_t getRunTime() const;
      uint64_t getRunTimeEstimate() const;
      double   getEstimatedProgress() const;
      uint64_t getCreditEstimate() const;
      uint64_t getETA() const;
      uint64_t getPPD() const;

      std::string getLogPrefix() const;
      std::string getDirectory() const;
      cb::URI getWSURL(const std::string &path) const;
      uint64_t getDeadline() const;
      bool isFinished() const;
      bool isExpired() const;

      void triggerNext(double secs = 0);
      void dumpWU();
      void save();

    protected:
      void cancelRequest();
      void setState(UnitState state);
      void next();

      void processStarted();
      void processEnded();
      void processTimer();
      double getKnownProgress() const;
      void updateKnownProgress(uint64_t done, uint64_t total);

      void setProgress(double done, double total);
      void getCore();
      void run();
      void readInfo();
      void readViewerData();
      void readViewerTop();
      bool readViewerFrame();
      void setResults(const std::string &status, const std::string &dataHash);
      void finalizeRun();
      void stopRun();
      void monitorRun();
      void clean();
      void setWait(double delay);
      void retry();

      void assignResponse(const cb::JSON::ValuePtr &data);
      void writeProjectRestrictions(cb::JSON::Sink &sink,
                                    const cb::JSON::ValuePtr &project);
      void writeRequest(cb::JSON::Sink &sink);
      void assign();
      void downloadResponse(const cb::JSON::ValuePtr &data);
      void download();
      void uploadResponse(const cb::JSON::ValuePtr &data);
      void upload();
      void dumpResponse(const cb::JSON::ValuePtr &data);
      void dump();
      void response(cb::Event::Request &req);
    };
  }
}
