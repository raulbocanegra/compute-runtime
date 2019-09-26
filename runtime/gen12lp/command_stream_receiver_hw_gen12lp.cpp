/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/command_stream/command_stream_receiver_hw_bdw_plus.inl"
#include "runtime/command_stream/device_command_stream.h"
#include "runtime/gen12lp/helpers_gen12lp.h"
#include "runtime/gen12lp/hw_cmds.h"
#include "runtime/helpers/blit_commands_helper_bdw_plus.inl"

#include "hw_info.h"

namespace NEO {
typedef TGLLPFamily Family;
static auto gfxCore = IGFX_GEN12LP_CORE;

template <>
void CommandStreamReceiverHw<Family>::programL3(LinearStream &csr, DispatchFlags &dispatchFlags, uint32_t &newL3Config) {
}

template <>
size_t CommandStreamReceiverHw<Family>::getCmdSizeForL3Config() const {
    return 0;
}

template <>
size_t CommandStreamReceiverHw<Family>::getCmdSizeForComputeMode() {
    if (!csrSizeRequestFlags.hasSharedHandles) {
        for (const auto &allocation : this->getResidencyAllocations()) {
            if (allocation->peekSharedHandle()) {
                csrSizeRequestFlags.hasSharedHandles = true;
                break;
            }
        }
    }

    size_t size = 0;
    if (csrSizeRequestFlags.coherencyRequestChanged || csrSizeRequestFlags.hasSharedHandles) {
        size += sizeof(typename Family::STATE_COMPUTE_MODE);
        if (csrSizeRequestFlags.hasSharedHandles) {
            size += sizeof(typename Family::PIPE_CONTROL);
        }
    }
    return size;
}

template <>
void CommandStreamReceiverHw<Family>::programComputeMode(LinearStream &stream, DispatchFlags &dispatchFlags) {
    typedef typename Family::STATE_COMPUTE_MODE STATE_COMPUTE_MODE;
    typedef typename Family::PIPE_CONTROL PIPE_CONTROL;

    if (csrSizeRequestFlags.coherencyRequestChanged || csrSizeRequestFlags.hasSharedHandles) {
        auto stateComputeMode = stream.getSpaceForCmd<STATE_COMPUTE_MODE>();
        *stateComputeMode = Family::cmdInitStateComputeMode;
        STATE_COMPUTE_MODE::FORCE_NON_COHERENT coherencyValue = !dispatchFlags.requiresCoherency ? STATE_COMPUTE_MODE::FORCE_NON_COHERENT_FORCE_GPU_NON_COHERENT : STATE_COMPUTE_MODE::FORCE_NON_COHERENT_FORCE_DISABLED;
        stateComputeMode->setForceNonCoherent(coherencyValue);
        stateComputeMode->setMaskBits(Family::stateComputeModeForceNonCoherentMask);

        this->lastSentCoherencyRequest = static_cast<int8_t>(dispatchFlags.requiresCoherency);

        if (csrSizeRequestFlags.hasSharedHandles) {
            auto pc = stream.getSpaceForCmd<PIPE_CONTROL>();
            *pc = Family::cmdInitPipeControl;
        }
    }
}

template <>
void populateFactoryTable<CommandStreamReceiverHw<Family>>() {
    extern CommandStreamReceiverCreateFunc commandStreamReceiverFactory[IGFX_MAX_CORE];
    commandStreamReceiverFactory[gfxCore] = DeviceCommandStreamReceiver<Family>::create;
}

template <>
inline typename Family::PIPE_CONTROL *CommandStreamReceiverHw<Family>::addPipeControlBeforeStateBaseAddress(LinearStream &commandStream) {
    auto pCmd = addPipeControlCmd(commandStream);
    pCmd->setTextureCacheInvalidationEnable(true);
    pCmd->setDcFlushEnable(true);

    if (Gen12LPHelpers::hdcFlushForPipeControlBeforeStateBaseAddressRequired(executionEnvironment.getHardwareInfo()->platform.eProductFamily)) {
        pCmd->setHdcFlushEnable(true);
    }
    return pCmd;
}

template class CommandStreamReceiverHw<Family>;
template struct BlitCommandsHelper<Family>;

const Family::GPGPU_WALKER Family::cmdInitGpgpuWalker = Family::GPGPU_WALKER::sInit();
const Family::INTERFACE_DESCRIPTOR_DATA Family::cmdInitInterfaceDescriptorData = Family::INTERFACE_DESCRIPTOR_DATA::sInit();
const Family::MEDIA_INTERFACE_DESCRIPTOR_LOAD Family::cmdInitMediaInterfaceDescriptorLoad = Family::MEDIA_INTERFACE_DESCRIPTOR_LOAD::sInit();
const Family::MEDIA_STATE_FLUSH Family::cmdInitMediaStateFlush = Family::MEDIA_STATE_FLUSH::sInit();
const Family::MI_BATCH_BUFFER_START Family::cmdInitBatchBufferStart = Family::MI_BATCH_BUFFER_START::sInit();
const Family::MI_BATCH_BUFFER_END Family::cmdInitBatchBufferEnd = Family::MI_BATCH_BUFFER_END::sInit();
const Family::PIPE_CONTROL Family::cmdInitPipeControl = Family::PIPE_CONTROL::sInit();
const Family::STATE_COMPUTE_MODE Family::cmdInitStateComputeMode = Family::STATE_COMPUTE_MODE::sInit();
const Family::MI_SEMAPHORE_WAIT Family::cmdInitMiSemaphoreWait = Family::MI_SEMAPHORE_WAIT::sInit();
const Family::RENDER_SURFACE_STATE Family::cmdInitRenderSurfaceState = Family::RENDER_SURFACE_STATE::sInit();
const Family::MI_LOAD_REGISTER_IMM Family::cmdInitLoadRegisterImm = Family::MI_LOAD_REGISTER_IMM::sInit();
const Family::MI_LOAD_REGISTER_REG Family::cmdInitLoadRegisterReg = Family::MI_LOAD_REGISTER_REG::sInit();
const Family::MI_LOAD_REGISTER_MEM Family::cmdInitLoadRegisterMem = Family::MI_LOAD_REGISTER_MEM::sInit();
const Family::MI_STORE_DATA_IMM Family::cmdInitStoreDataImm = Family::MI_STORE_DATA_IMM::sInit();
const Family::MI_STORE_REGISTER_MEM Family::cmdInitStoreRegisterMem = Family::MI_STORE_REGISTER_MEM::sInit();
const Family::MI_NOOP Family::cmdInitNoop = Family::MI_NOOP::sInit();
const Family::MI_REPORT_PERF_COUNT Family::cmdInitReportPerfCount = Family::MI_REPORT_PERF_COUNT::sInit();
const Family::MI_ATOMIC Family::cmdInitAtomic = Family::MI_ATOMIC::sInit();
const Family::PIPELINE_SELECT Family::cmdInitPipelineSelect = Family::PIPELINE_SELECT::sInit();
const Family::MI_ARB_CHECK Family::cmdInitArbCheck = Family::MI_ARB_CHECK::sInit();
const Family::MEDIA_VFE_STATE Family::cmdInitMediaVfeState = Family::MEDIA_VFE_STATE::sInit();
const Family::STATE_BASE_ADDRESS Family::cmdInitStateBaseAddress = Family::STATE_BASE_ADDRESS::sInit();
const Family::MEDIA_SURFACE_STATE Family::cmdInitMediaSurfaceState = Family::MEDIA_SURFACE_STATE::sInit();
const Family::SAMPLER_STATE Family::cmdInitSamplerState = Family::SAMPLER_STATE::sInit();
const Family::GPGPU_CSR_BASE_ADDRESS Family::cmdInitGpgpuCsrBaseAddress = Family::GPGPU_CSR_BASE_ADDRESS::sInit();
const Family::STATE_SIP Family::cmdInitStateSip = Family::STATE_SIP::sInit();
const Family::BINDING_TABLE_STATE Family::cmdInitBindingTableState = Family::BINDING_TABLE_STATE::sInit();
const Family::MI_USER_INTERRUPT Family::cmdInitUserInterrupt = Family::MI_USER_INTERRUPT::sInit();
const Family::XY_SRC_COPY_BLT Family::cmdInitXyCopyBlt = Family::XY_SRC_COPY_BLT::sInit();
const Family::MI_FLUSH_DW Family::cmdInitMiFlushDw = Family::MI_FLUSH_DW::sInit();
} // namespace NEO