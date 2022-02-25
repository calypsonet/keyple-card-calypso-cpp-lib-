/**************************************************************************************************
 * Copyright (c) 2022 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "CmdSamSvPrepareDebit.h"

/* Keyple Card Calypso */
#include "CalypsoCardAdapter.h"
#include "CalypsoCardClass.h"
#include "CalypsoSamAccessForbiddenException.h"
#include "CalypsoSamCounterOverflowException.h"
#include "CalypsoSamDataAccessException.h"
#include "CalypsoSamIncorrectInputDataException.h"
#include "CalypsoSamIllegalParameterException.h"
#include "CardIllegalParameterException.h"
#include "SamUtilAdapter.h"

/* Keyple Core Util */
#include "ApduUtil.h"
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "System.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

const CalypsoSamCommand CmdSamSvPrepareDebit::mCommand = CalypsoSamCommand::SV_PREPARE_DEBIT;

const std::map<const int, const std::shared_ptr<StatusProperties>>
    CmdSamSvPrepareDebit::STATUS_TABLE = {
    {
        0x6700,
        std::make_shared<StatusProperties>("Lc value not supported.",
                                           typeid(CardIllegalParameterException))
    }, {
        0x6985,
        std::make_shared<StatusProperties>("Preconditions not satisfied.",
                                           typeid(CalypsoSamAccessForbiddenException))
    }, {
        0x6A00,
        std::make_shared<StatusProperties>("Incorrect P1 or P2",
                                           typeid(CalypsoSamIllegalParameterException))
    }, {
        0x6A80,
        std::make_shared<StatusProperties>("Incorrect incoming data.",
                                           typeid(CalypsoSamIncorrectInputDataException))
    }, {
        0x6A83,
        std::make_shared<StatusProperties>("Record not found: ciphering key not found",
                                           typeid(CalypsoSamDataAccessException))
    }
};

CmdSamSvPrepareDebit::CmdSamSvPrepareDebit(const CalypsoSam::ProductType productType,
                                           const std::vector<uint8_t>& svGetHeader,
                                           const std::vector<uint8_t>& svGetData,
                                           const std::vector<uint8_t>& svDebitCmdBuildData)
: AbstractSamCommand(mCommand)
{
    const uint8_t cla = SamUtilAdapter::getClassByte(productType);
    const uint8_t p1 = 0x01;
    const uint8_t p2 = 0xFF;
    std::vector<uint8_t> data(16 + svGetData.size()); /* Header(4) + SvDebit data (12) = 16 bytes*/

    System::arraycopy(svGetHeader, 0, data, 0, 4);
    System::arraycopy(svGetData, 0, data, 4, svGetData.size());
    System::arraycopy(svDebitCmdBuildData,
                      0,
                      data,
                      4 + svGetData.size(),
                      svDebitCmdBuildData.size());

    setApduRequest(
        std::make_shared<ApduRequestAdapter>(
            ApduUtil::build(cla, mCommand.getInstructionByte(), p1, p2, data)));
}

const std::map<const int, const std::shared_ptr<StatusProperties>>&
    CmdSamSvPrepareDebit::getStatusTable() const
{
    return STATUS_TABLE;
}

}
}
}
