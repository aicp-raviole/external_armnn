﻿//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//
#include <boost/test/unit_test.hpp>
#include "armnnCaffeParser/ICaffeParser.hpp"
#include "ParserPrototxtFixture.hpp"

BOOST_AUTO_TEST_SUITE(CaffeParser)

struct MulFixture : public armnnUtils::ParserPrototxtFixture<armnnCaffeParser::ICaffeParser>
{
    MulFixture()
    {
        m_Prototext = "name: \"MinimalMul\"\n"
            "layer {\n"
            "  name: \"data\"\n"
            "  type: \"Input\"\n"
            "  top: \"data\"\n"
            "  input_param { shape: { dim: 1 dim: 1 dim: 4 dim: 4 } }\n"
            "}\n"
            "layer {\n"
            "    bottom: \"data\"\n"
            "    top: \"pool1\"\n"
            "    name: \"pool1\"\n"
            "    type: \"Pooling\"\n"
            "    pooling_param {\n"
            "        kernel_size: 2\n"
            "        stride: 2\n"
            "        pool: MAX\n"
            "    }\n"
            "}\n"
            "layer {\n"
            "    bottom: \"data\"\n"
            "    top: \"pool2\"\n"
            "    name: \"pool2\"\n"
            "    type: \"Pooling\"\n"
            "    pooling_param {\n"
            "        kernel_size: 2\n"
            "        stride: 2\n"
            "        pool: MAX\n"
            "    }\n"
            "}\n"
            "layer {\n"
            "    bottom: \"pool1\"\n"
            "    bottom: \"pool2\"\n"
            "    top: \"mul\"\n"
            "    name: \"mul\"\n"
            "    type: \"Eltwise\"\n"
            "    eltwise_param {\n"
            "        operation: 0\n"
            "    }\n"
            "}\n";
        SetupSingleInputSingleOutput("data", "mul");
    }
};

BOOST_FIXTURE_TEST_CASE(ParseMul, MulFixture)
{
    RunTest<4>(
        {
            0, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 1, 0,
            1, 0, 1, 1
        },
        {
            1, 0,
            1, 1
        });
}

BOOST_AUTO_TEST_SUITE_END()
