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

#include "ApduResponse.h"
#include "CalypsoPo.h"
#include "PoSelectionRequest.h"
#include "SeResponse.h"
#include "GetDataFciRespPars.h"
#include "ByteArrayUtil.h"
#include "SeResponse.h"
#include "SelectionStatus.h"

/* Common */
#include "System.h"

namespace keyple {
namespace calypso {
namespace transaction {

using namespace keyple::core::seproxy::message;
using namespace keyple::calypso::command;
using namespace keyple::calypso::command::po;
using namespace keyple::calypso::command::po::parser;
using namespace keyple::core::seproxy::message;
using namespace keyple::core::selection;
using namespace keyple::core::util;

CalypsoPo::CalypsoPo(std::shared_ptr<SeResponse> selectionResponse,
                     TransmissionMode transmissionMode,
                     const std::string& extraInfo)
: AbstractMatchingSe(selectionResponse, transmissionMode, extraInfo),
  poAtr(selectionResponse->getSelectionStatus()->getAtr()->getBytes())
{

    /*
     * The selectionSeResponse may not include a FCI field (e.g. old PO Calypso
     * Rev 1)
     */
    if (selectionResponse->getSelectionStatus()->getFci()->isSuccessful()) {
        std::shared_ptr<ApduResponse> fci =
            selectionResponse->getSelectionStatus()->getFci();
        /*
         * Parse PO FCI - to retrieve Calypso Revision, Serial Number, &amp; DF
         * Name (AID)
         */
        std::shared_ptr<GetDataFciRespPars> poFciRespPars =
            std::make_shared<GetDataFciRespPars>(fci);

        /*
         * Resolve the PO revision from the application type byte:
         *
         * <ul> <li>if
         * <code>%1-------</code>&nbsp;&nbsp;&rarr;&nbsp;&nbsp;CLAP&nbsp;&nbsp;
         * &rarr;&nbsp;&
         * nbsp; REV3.1</li> <li>if
         * <code>%00101---</code>&nbsp;&nbsp;&rarr;&nbsp;&nbsp;REV3.2</li> <li>
         * if
         * <code>%00100---</code>&nbsp;&nbsp;&rarr;&nbsp;&nbsp;REV3.1</li>
         * <li>otherwise&nbsp;&nbsp;&rarr;&nbsp;&nbsp;REV2.4</li> </ul>
         */
        char applicationTypeByte = poFciRespPars->getApplicationTypeByte();
        if ((applicationTypeByte & (1 << 7)) != 0) {
            /* CLAP */
            this->revision = PoRevision::REV3_1_CLAP;
        } else if ((applicationTypeByte >> 3) == static_cast<char>(0x05)) {
            this->revision = PoRevision::REV3_2;
        } else if ((applicationTypeByte >> 3) == static_cast<char>(0x04)) {
            this->revision = PoRevision::REV3_1;
        } else {
            this->revision = PoRevision::REV2_4;
        }

        this->dfName = poFciRespPars->getDfName();

        this->applicationSerialNumber =
            poFciRespPars->getApplicationSerialNumber();

        if (this->revision == PoRevision::REV2_4) {
            /*
             * old cards have their modification counter in number of commands
             */
            modificationCounterIsInBytes = false;
            this->modificationsCounterMax =
                REV2_PO_DEFAULT_WRITE_OPERATIONS_NUMBER_SUPPORTED_PER_SESSION;
        } else {
            this->modificationsCounterMax = poFciRespPars->getBufferSizeValue();
        }
        this->bufferSizeIndicator = poFciRespPars->getBufferSizeIndicator();
        this->bufferSizeValue     = poFciRespPars->getBufferSizeValue();
        this->platform            = poFciRespPars->getPlatformByte();
        this->applicationType     = poFciRespPars->getApplicationTypeByte();
        this->isRev3_2ModeAvailable_Renamed =
            poFciRespPars->isRev3_2ModeAvailable();
        this->isRatificationCommandRequired_Renamed =
            poFciRespPars->isRatificationCommandRequired();
        this->hasCalypsoStoredValue_Renamed =
            poFciRespPars->hasCalypsoStoredValue();
        this->hasCalypsoPin_Renamed = poFciRespPars->hasCalypsoPin();
        this->applicationSubtypeByte =
            poFciRespPars->getApplicationSubtypeByte();
        this->softwareIssuerByte = poFciRespPars->getSoftwareIssuerByte();
        this->softwareVersion    = poFciRespPars->getSoftwareVersionByte();
        this->softwareRevision   = poFciRespPars->getSoftwareRevisionByte();
        this->isDfInvalidated_Renamed = poFciRespPars->isDfInvalidated();
    } else {
        /*
         * FCI is not provided: we consider it is Calypso PO rev 1, it's serial
         * number is provided in the ATR
         */

        /*
         * basic check: we expect to be here following a selection based on the
         * ATR
         */
        if (poAtr.size() != PO_REV1_ATR_LENGTH) {
            throw IllegalStateException(StringHelper::formatSimple(
                "Unexpected ATR length: %s", ByteArrayUtil::toHex(poAtr)));
        }

        this->revision = PoRevision::REV1_0;
        this->dfName.clear();
        this->applicationSerialNumber = std::vector<uint8_t>(8);
        /* old cards have their modification counter in number of commands */
        this->modificationCounterIsInBytes = false;
        /*
         * the array is initialized with 0 (cf. default value for primitive
         * types)
         */
        System::arraycopy(poAtr, 12, this->applicationSerialNumber, 4, 4);
        this->modificationsCounterMax =
            REV1_PO_DEFAULT_WRITE_OPERATIONS_NUMBER_SUPPORTED_PER_SESSION;

        this->bufferSizeIndicator = 0;
        this->bufferSizeValue =
            REV1_PO_DEFAULT_WRITE_OPERATIONS_NUMBER_SUPPORTED_PER_SESSION;
        this->platform                              = poAtr[6];
        this->applicationType                       = poAtr[7];
        this->applicationSubtypeByte                = poAtr[8];
        this->isRev3_2ModeAvailable_Renamed         = false;
        this->isRatificationCommandRequired_Renamed = true;
        this->hasCalypsoStoredValue_Renamed         = false;
        this->hasCalypsoPin_Renamed                 = false;
        this->softwareIssuerByte                    = poAtr[9];
        this->softwareVersion                       = poAtr[10];
        this->softwareRevision                      = poAtr[11];
        this->isDfInvalidated_Renamed               = false;
    }

    logger->trace("REVISION = %, SERIALNUMBER = %, DFNAME = %\n",
                  revision, applicationSerialNumber, dfName);
}

PoRevision CalypsoPo::getRevision()
{
    return this->revision;
}

const std::vector<uint8_t>& CalypsoPo::getDfName() const
{
    return dfName;
}

const std::vector<uint8_t>& CalypsoPo::getApplicationSerialNumber() const
{
    return applicationSerialNumber;
}

const std::vector<uint8_t>& CalypsoPo::getAtr() const
{
    return poAtr;
}

bool CalypsoPo::isModificationsCounterInBytes()
{
    return modificationCounterIsInBytes;
}

int CalypsoPo::getModificationsCounter()
{
    return modificationsCounterMax;
}

char CalypsoPo::getBufferSizeIndicator()
{
    return bufferSizeIndicator;
}

int CalypsoPo::getBufferSizeValue()
{
    return bufferSizeValue;
}

char CalypsoPo::getPlatformByte()
{
    return platform;
}

char CalypsoPo::getApplicationTypeByte()
{
    return applicationType;
}

bool CalypsoPo::isRev3_2ModeAvailable()
{
    return isRev3_2ModeAvailable_Renamed;
}

bool CalypsoPo::isRatificationCommandRequired()
{
    return isRatificationCommandRequired_Renamed;
}

bool CalypsoPo::hasCalypsoStoredValue()
{
    return hasCalypsoStoredValue_Renamed;
}

bool CalypsoPo::hasCalypsoPin()
{
    return hasCalypsoPin_Renamed;
}

char CalypsoPo::getApplicationSubtypeByte()
{
    return applicationSubtypeByte;
}

char CalypsoPo::getSoftwareIssuerByte()
{
    return softwareIssuerByte;
}

char CalypsoPo::getSoftwareVersionByte()
{
    return softwareVersion;
}

char CalypsoPo::getSoftwareRevisionByte()
{
    return softwareRevision;
}

bool CalypsoPo::isDfInvalidated()
{
    return isDfInvalidated_Renamed;
}

PoClass CalypsoPo::getPoClass()
{
    /*
     * Rev1 and Rev2 expects the legacy class byte while Rev3 expects the ISO
     * class byte
     */
    if (revision == PoRevision::REV1_0 || revision == PoRevision::REV2_4) {
        logger->trace("PO revision = %, PO class = %\n", revision,
			          std::string("PoClass::LEGACY"));
        return PoClass::LEGACY;
    } else {
        logger->trace("PO revision = %, PO class = %\n", revision,
			          std::string("PoClass::ISO"));
        return PoClass::ISO;
    }
}

}
}
}
