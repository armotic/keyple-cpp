/******************************************************************************
 * Copyright (c) 2018 Calypso Networks Association                            *
 * https://www.calypsonet-asso.org/                                           *
 *                                                                            *
 * See the NOTICE file(s) distributed with this work for additional           *
 * information regarding copyright ownership.                                 *
 *                                                                            *
 * This program and the accompanying materials are made available under the   *
 * terms of the Eclipse Public License 2.0 which is available at              *
 * http://www.eclipse.org/legal/epl-2.0                                       *
 *                                                                            *
 * SPDX-License-Identifier: EPL-2.0                                           *
 ******************************************************************************/

#include "WaitForSeInsertion.h"

/* Core */
#include "AbstractObservableState.h"
#include "ReaderEvent.h"

namespace keyple {
namespace core {
namespace seproxy {
namespace plugin {
namespace local {
namespace state {

WaitForSeInsertion::WaitForSeInsertion(AbstractObservableLocalReader* reader)
: AbstractObservableState(MonitoringState::WAIT_FOR_SE_INSERTION, reader)
{
}

WaitForSeInsertion::WaitForSeInsertion(
    AbstractObservableLocalReader* reader,
    std::shared_ptr<MonitoringJob> monitoringJob,
    std::shared_ptr<MonitoringPool> executorService)
: AbstractObservableState(MonitoringState::WAIT_FOR_SE_INSERTION, reader,
                          monitoringJob, executorService)
{
}

void WaitForSeInsertion::onEvent(const InternalEvent event)
{
    std::shared_ptr<ReaderEvent> seEvent;

    logger->trace("[%] onEvent => Event % received in currentState %\n",
                  reader->getName(), event, state);

    /*
     * Process InternalEvent
     */
    switch (event) {
    case InternalEvent::SE_INSERTED:
        /* Process default selection if any, return an event, can be null */
        logger->debug("processing se inserted\n");
        seEvent = this->reader->processSeInserted();

        if (seEvent != nullptr) {
            /* Switch internal state */
            logger->debug("switching to WAIT_FOR_SE_PROCESSING state\n");
            switchState(MonitoringState::WAIT_FOR_SE_PROCESSING);

            /* Notify the external observer of the event */
            logger->debug("notifying observers\n");
            reader->notifyObservers(seEvent);
        } else {
            /*
             * If none event was sent to the application, back to SE detection
             * stay in the same state, however switch to WAIT_FOR_SE_INSERTION
             * to relaunch the monitoring job
             */
            logger->trace("[%] onEvent => Inserted SE hasn't matched\n",
                          reader->getName());
            switchState(MonitoringState::WAIT_FOR_SE_REMOVAL);
        }
        break;
    case InternalEvent::STOP_DETECT:
        logger->debug("switching to WAIT_FOR_SE_PROCESSING state\n");
        switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        break;
    case InternalEvent::SE_REMOVED:
        /*
         * TODO Check if this case really happens (NFC?)
         * SE has been removed during default selection
         */
        if (reader->getPollingMode() ==
            ObservableReader::PollingMode::REPEATING) {
            logger->debug("switching to WAIT_FOR_SE_INSERTION state\n");
            switchState(MonitoringState::WAIT_FOR_SE_INSERTION);
        } else {
            logger->debug("switching to WAIT_FOR_START_DETECTION state\n");
            switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        }
        break;
    default:
        logger->warn("[%] Ignore =>  Event % received in currentState %\n",
                     reader->getName(), event, state);
        break;
    }
}

}
}
}
}
}
}
