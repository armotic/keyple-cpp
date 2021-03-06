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

#include "ReadRecordsCmdBuildTest.h"

using namespace keyple::calypso::command::po::builder;

namespace keyple {
namespace calypso {
namespace command {
namespace po {
namespace builder {

using PoClass = keyple::calypso::command::PoClass;
using AbstractApduCommandBuilder =
    keyple::core::command::AbstractApduCommandBuilder;
using ApduRequest = keyple::core::seproxy::message::ApduRequest;

void ReadRecordsCmdBuildTest::readRecords_rev2_4()
{

    uint8_t cla               = (0x94);
    uint8_t cmd               = (0xB2);
    bool readJustOneRecord = false;
    uint8_t sfi               = (0x08);
    uint8_t p2                = ((sfi * 8) + 5);
    ReadDataStructure readDataStructure;

    // revision 2.4
    std::vector<uint8_t> request2_4 = {cla, cmd, record_number, p2, 0x00};
    apduCommandBuilder           = std::make_shared<ReadRecordsCmdBuild>(
        PoClass::LEGACY, sfi, readDataStructure, record_number,
        readJustOneRecord, expectedLength, "TestRev2_4");
    apduRequest = apduCommandBuilder->getApduRequest();
    ASSERT_EQ(request2_4, apduRequest->getBytes());
}

void ReadRecordsCmdBuildTest::readRecords_rev3_1()
{

    uint8_t cla               = (0x00);
    uint8_t cmd               = (0xB2);
    bool readJustOneRecord = false;
    uint8_t sfi               = (0x08);
    uint8_t p2                = ((sfi * 8) + 5);
    ReadDataStructure readDataStructure;

    // revision 3.1
    std::vector<uint8_t> request3_1 = {cla, cmd, record_number, p2, 0x00};
    apduCommandBuilder           = std::make_shared<ReadRecordsCmdBuild>(
        PoClass::ISO, sfi, readDataStructure, record_number, readJustOneRecord,
        expectedLength, "TestRev3_1");
    apduRequest = apduCommandBuilder->getApduRequest();
    ASSERT_EQ(request3_1, apduRequest->getBytes());
}

void ReadRecordsCmdBuildTest::readRecords_rev3_2()
{
    uint8_t cla               = (0x00);
    uint8_t cmd               = (0xB2);
    bool readJustOneRecord = false;
    uint8_t sfi               = (0x08);
    uint8_t p2                = ((sfi * 8) + 5);
    ReadDataStructure readDataStructure;

    // revision 3.2
    std::vector<uint8_t> request3_2 = {cla, cmd, record_number, p2, 0x00};
    apduCommandBuilder           = std::make_shared<ReadRecordsCmdBuild>(
        PoClass::ISO, sfi, readDataStructure, record_number, readJustOneRecord,
        expectedLength, "TestRev3_2");
    apduRequest = apduCommandBuilder->getApduRequest();
    ASSERT_EQ(request3_2, apduRequest->getBytes());
}
}
}
}
}
}

TEST(ReadRecordsCmdBuildTest, testA)
{
    std::shared_ptr<ReadRecordsCmdBuildTest> LocalTest =
        std::make_shared<ReadRecordsCmdBuildTest>();
    LocalTest->readRecords_rev2_4();
}

TEST(ReadRecordsCmdBuildTest, testB)
{
    std::shared_ptr<ReadRecordsCmdBuildTest> LocalTest =
        std::make_shared<ReadRecordsCmdBuildTest>();
    LocalTest->readRecords_rev3_1();
}

TEST(ReadRecordsCmdBuildTest, testC)
{
    std::shared_ptr<ReadRecordsCmdBuildTest> LocalTest =
        std::make_shared<ReadRecordsCmdBuildTest>();
    LocalTest->readRecords_rev3_2();
}
