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

#pragma once

#include <vector>
#include <memory>

/* Core */
#include "ApduResponse.h"

/* Calypso */
#include "AbstractOpenSessionRespPars.h"

namespace keyple {
namespace calypso {
namespace command {
namespace po {
namespace parser {
namespace security {

using namespace keyple::core::seproxy::message;

class KEYPLECALYPSO_API OpenSession31RespPars final
: public AbstractOpenSessionRespPars {
public:
    /**
     *
     */
    OpenSession31RespPars(std::shared_ptr<ApduResponse> response);

    /**
     *
     */
    std::shared_ptr<SecureSession>
    toSecureSession(const std::vector<uint8_t>& apduResponseData);

protected:
    /**
     *
     */
    std::shared_ptr<OpenSession31RespPars> shared_from_this()
    {
        return std::static_pointer_cast<OpenSession31RespPars>(
            AbstractOpenSessionRespPars::shared_from_this());
    }
};

}
}
}
}
}
}
