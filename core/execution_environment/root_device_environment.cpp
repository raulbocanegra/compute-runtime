/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "core/execution_environment/root_device_environment.h"

#include "runtime/aub/aub_center.h"
#include "runtime/execution_environment/execution_environment.h"
#include "runtime/gmm_helper/page_table_mngr.h"

namespace NEO {

RootDeviceEnvironment::RootDeviceEnvironment(ExecutionEnvironment &executionEnvironment) : executionEnvironment(executionEnvironment) {}
RootDeviceEnvironment::~RootDeviceEnvironment() = default;

void RootDeviceEnvironment::initAubCenter(bool localMemoryEnabled, const std::string &aubFileName, CommandStreamReceiverType csrType) {
    if (!aubCenter) {
        aubCenter.reset(new AubCenter(executionEnvironment.getHardwareInfo(), localMemoryEnabled, aubFileName, csrType));
    }
}
} // namespace NEO
