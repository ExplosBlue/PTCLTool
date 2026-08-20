#pragma once

#include "ptcl/ptcl.h"


namespace Ptcl::PtclJson {


// ========================================================================== //


bool exportProject(const PtclRes& res, const QString& dirPath);

bool importProject(const QString& projPath, PtclRes& res, PtclSanitizeReport& report);


// ========================================================================== //


} // namespace Ptcl::PtclJson
