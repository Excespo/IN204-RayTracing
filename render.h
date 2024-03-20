//
// Created by LUO Yijie on 2024/3/20.
//

#ifndef RAY_TRACING_RENDER_H
#define RAY_TRACING_RENDER_H

#include "common.h"
#include "camera.h"

class RenderStrategy {
public:
    virtual ~RenderStrategy() = default;
    virtual void render(const std::string )
};

#endif //RAY_TRACING_RENDER_H
