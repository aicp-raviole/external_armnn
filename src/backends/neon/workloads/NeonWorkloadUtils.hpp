﻿//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//
#pragma once

#include <backendsCommon/Workload.hpp>
#include <aclCommon/ArmComputeTensorUtils.hpp>
#include <neon/NeonTensorHandle.hpp>
#include <neon/NeonTimer.hpp>
#include <backendsCommon/CpuTensorHandle.hpp>

#include <armnn/Utils.hpp>

#include <Half.hpp>

#define ARMNN_SCOPED_PROFILING_EVENT_NEON(name) \
    ARMNN_SCOPED_PROFILING_EVENT_WITH_INSTRUMENTS(armnn::Compute::CpuAcc, \
                                                  name, \
                                                  armnn::NeonTimer(), \
                                                  armnn::WallClockTimer())

using namespace armnn::armcomputetensorutils;

namespace armnn
{

template <typename T>
void CopyArmComputeTensorData(arm_compute::Tensor& dstTensor, const T* srcData)
{
    InitialiseArmComputeTensorEmpty(dstTensor);
    CopyArmComputeITensorData(srcData, dstTensor);
}

inline void InitializeArmComputeTensorData(arm_compute::Tensor& tensor,
                                           const ConstCpuTensorHandle* handle)
{
    BOOST_ASSERT(handle);

    switch(handle->GetTensorInfo().GetDataType())
    {
        case DataType::Float16:
            CopyArmComputeTensorData(tensor, handle->GetConstTensor<armnn::Half>());
            break;
        case DataType::Float32:
            CopyArmComputeTensorData(tensor, handle->GetConstTensor<float>());
            break;
        case DataType::QAsymmU8:
            CopyArmComputeTensorData(tensor, handle->GetConstTensor<uint8_t>());
            break;
        ARMNN_NO_DEPRECATE_WARN_BEGIN
        case DataType::QuantizedSymm8PerAxis:
            ARMNN_FALLTHROUGH;
        case DataType::QSymmS8:
            CopyArmComputeTensorData(tensor, handle->GetConstTensor<int8_t>());
            break;
        ARMNN_NO_DEPRECATE_WARN_END
        case DataType::Signed32:
            CopyArmComputeTensorData(tensor, handle->GetConstTensor<int32_t>());
            break;
        default:
            BOOST_ASSERT_MSG(false, "Unexpected tensor type.");
    }
};

inline auto SetNeonStridedSliceData(const std::vector<int>& m_begin,
                                    const std::vector<int>& m_end,
                                    const std::vector<int>& m_stride)
{
    arm_compute::Coordinates starts;
    arm_compute::Coordinates ends;
    arm_compute::Coordinates strides;

    unsigned int num_dims = static_cast<unsigned int>(m_begin.size());

    for (unsigned int i = 0; i < num_dims; i++)
    {
        unsigned int revertedIndex = num_dims - i - 1;

        starts.set(i, static_cast<int>(m_begin[revertedIndex]));
        ends.set(i, static_cast<int>(m_end[revertedIndex]));
        strides.set(i, static_cast<int>(m_stride[revertedIndex]));
    }

    return std::make_tuple(starts, ends, strides);
}

inline auto SetNeonSliceData(const std::vector<unsigned int>& m_begin,
                             const std::vector<unsigned int>& m_size)
{
    // This function must translate the size vector given to an end vector
    // expected by the ACL NESlice workload
    arm_compute::Coordinates starts;
    arm_compute::Coordinates ends;

    unsigned int num_dims = static_cast<unsigned int>(m_begin.size());

    // For strided slices, we have the relationship size = (end - begin) / stride
    // For slice, we assume stride to be a vector of all ones, yielding the formula
    // size = (end - begin) therefore we know end = size + begin
    for (unsigned int i = 0; i < num_dims; i++)
    {
        unsigned int revertedIndex = num_dims - i - 1;

        starts.set(i, static_cast<int>(m_begin[revertedIndex]));
        ends.set(i, static_cast<int>(m_begin[revertedIndex] + m_size[revertedIndex]));
    }

    return std::make_tuple(starts, ends);
}

} //namespace armnn
