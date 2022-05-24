/**
 * Copyright © 2022 Zen Shawn. All rights reserved.
 * 
 * @file main.cpp
 * @author: Zen Shawn
 * @email: xiaozisheng2008@qq.com
 * @date: 19:04:24, May 24, 2022
 */

#include "ku.h"
#include <algorithm>


int main()
{
    int n = 10;
    std::vector<float> vec(n);
    float v = 0.f;
    std::for_each(vec.begin(), vec.end(), [&v](float &e)
                  {
        e = v + 0.5f;
        v = e; });
    print_vec(vec);
    auto res = vec_sqrt(vec);
    print_vec(res);
    return 0;
}

