/**
 * Copyright © 2022 Zen Shawn. All rights reserved.
 * 
 * @file ku.cpp
 * @author: Zen Shawn
 * @email: xiaozisheng2008@qq.com
 * @date: 19:02:55, May 24, 2022
 */

#include "ku.h"
#include <algorithm>
#include <iostream>
#include <cmath>

void print_vec(const std::vector<float> &vec)
{
    std::cout << "{";
    for (const auto &e : vec)
    {
        std::cout << e << ",";
    }
    std::cout << "}\n";
}

std::vector<float> vec_sqrt(const std::vector<float> &vec)
{
    std::vector<float> res(vec.size());
    int i = 0;
    for (const auto &e : vec)
    {
        res[i++] = std::sqrt(e);
    }
    return res;
}