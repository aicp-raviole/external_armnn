﻿//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <backendsCommon/Workload.hpp>
#include <backendsCommon/WorkloadData.hpp>
#include "BaseIterator.hpp"
#include "Decoders.hpp"
#include "Encoders.hpp"


namespace armnn
{

class RefFullyConnectedWorkload : public BaseWorkload<FullyConnectedQueueDescriptor>
{
public:
    explicit RefFullyConnectedWorkload(const FullyConnectedQueueDescriptor& descriptor,
                                       const WorkloadInfo& info);

    void PostAllocationConfigure() override;

    virtual void Execute() const override;

private:
    std::unique_ptr<ScopedCpuTensorHandle> m_Weight;
    std::unique_ptr<ScopedCpuTensorHandle> m_Bias;

    std::unique_ptr<Decoder<float>> m_InputDecoder;
    std::unique_ptr<Encoder<float>> m_OutputEncoder;
    std::unique_ptr<Decoder<float>> m_WeightDecoder;
    std::unique_ptr<Decoder<float>> m_BiasDecoder;

    TensorShape m_InputShape;
    TensorShape m_OutputShape;
    TensorShape m_WeightShape;
    unsigned int m_NumActivations;
};

} //namespace armnn
