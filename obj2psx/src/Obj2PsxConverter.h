#pragma once

struct PsxModel;
struct ObjModel;

struct ConversionParams {
    float scale{1.f};
};

PsxModel objToPsxModel(const ObjModel& objModel, const ConversionParams& params);
