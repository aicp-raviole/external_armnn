//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "RefL2NormalizationWorkload.hpp"
#include "RefWorkloadUtils.hpp"
#include "Decoders.hpp"
#include "Encoders.hpp"

#include <Profiling.hpp>

#include <armnnUtils/DataLayoutIndexed.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <cmath>

using namespace armnnUtils;

namespace armnn
{
RefL2NormalizationWorkload::RefL2NormalizationWorkload(
        const L2NormalizationQueueDescriptor& descriptor,
        const WorkloadInfo& info)
    : BaseWorkload<L2NormalizationQueueDescriptor>(descriptor, info) {}

void RefL2NormalizationWorkload::Execute() const
{
    ARMNN_SCOPED_PROFILING_EVENT(Compute::CpuRef, "RefL2NormalizationWorkload_Execute");

    const TensorInfo& inputInfo = GetTensorInfo(m_Data.m_Inputs[0]);
    const TensorInfo& outputInfo = GetTensorInfo(m_Data.m_Outputs[0]);

    auto inputDecoder  = MakeDecoder<float>(inputInfo, m_Data.m_Inputs[0]->Map());
    auto outputEncoder = MakeEncoder<float>(outputInfo, m_Data.m_Outputs[0]->Map());

    DataLayoutIndexed dataLayout(m_Data.m_Parameters.m_DataLayout);

    const TensorShape& shape = inputInfo.GetShape();
    unsigned int paddedShapeArray[4];
    const int idxShift = 4 - boost::numeric_cast<int>(shape.GetNumDimensions());

    const unsigned int batches = (idxShift == 0) ? shape[0] : 1;
    paddedShapeArray[0] = batches;

    const int channelsIdx = boost::numeric_cast<int>(dataLayout.GetChannelsIndex());
    const unsigned int channels = (channelsIdx - idxShift >= 0)
                                  ? shape[boost::numeric_cast<unsigned int>(channelsIdx - idxShift)]
                                  : 1;
    paddedShapeArray[channelsIdx] = channels;

    const int heightIdx = boost::numeric_cast<int>(dataLayout.GetHeightIndex());
    const unsigned int height = (heightIdx - idxShift >= 0)
                                ? shape[boost::numeric_cast<unsigned int>(heightIdx - idxShift)]
                                : 1;
    paddedShapeArray[heightIdx] = height;

    const int widthIdx = boost::numeric_cast<int>(dataLayout.GetWidthIndex());
    const unsigned int width = (widthIdx - idxShift >= 0)
                               ? shape[boost::numeric_cast<unsigned int>(widthIdx - idxShift)]
                               : 1;
    paddedShapeArray[widthIdx] = width;

    const TensorShape& paddedShape = TensorShape(4, paddedShapeArray);

    for (unsigned int n = 0; n < batches; ++n)
    {
        for (unsigned int c = 0; c < channels; ++c)
        {
            for (unsigned int h = 0; h < height; ++h)
            {
                for (unsigned int w = 0; w < width; ++w)
                {
                    float reduction = 0.0;
                    for (unsigned int d = 0; d < channels; ++d)
                    {
                        unsigned int inputIndex = dataLayout.GetIndex(paddedShape, n, d, h, w);

                        (*inputDecoder)[inputIndex];
                        const float value = inputDecoder->Get();
                        reduction += value * value;
                    }

                    unsigned int index = dataLayout.GetIndex(paddedShape, n, c, h, w);

                    float maximum = reduction < m_Data.m_Parameters.m_Eps ? m_Data.m_Parameters.m_Eps : reduction;

                    const float scale = 1.0f / sqrtf(maximum);

                    (*inputDecoder)[index];
                    (*outputEncoder)[index];
                    outputEncoder->Set(inputDecoder->Get() * scale);
                }
            }
        }
    }
}

} //namespace armnn
