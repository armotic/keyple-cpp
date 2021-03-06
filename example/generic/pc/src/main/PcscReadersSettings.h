/********************************************************************************
* Copyright (c) 2018 Calypso Networks Association https://www.calypsonet-asso.org/
*
* See the NOTICE file(s) distributed with this work for additional information regarding copyright
* ownership.
*
* This program and the accompanying materials are made available under the terms of the Eclipse
* Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0
*
* SPDX-License-Identifier: EPL-2.0
********************************************************************************/

#pragma once

#include <string>

namespace keyple {
namespace example {
namespace generic {
namespace pc {

class PcscReadersSettings {
public:
    /* This is where you should add patterns of readers you want to use for tests */
    static const std::string PO_READER_NAME_REGEX;
};

}
}
}
}
