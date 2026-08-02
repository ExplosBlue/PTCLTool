#pragma once

#include <QtCore/qnamespace.h>


namespace PtclEditor {


// ========================================================================== //


enum TextureListRoles {
    TexturePtrRole = Qt::UserRole + 1,
    IndexRole,
    FormatRole,
    SizeRole,
    UserCountRole,
    WidthRole,
    HeightRole
};


// ========================================================================== //


enum TextureColumn {
    ThumbnailColumn = 0,
    FormatColumn,
    DimensionsColumn,
    SizeColumn,
    UsersColumn,
    TextureColumnCount
};


// ========================================================================== //


} // namespace PtclEditor
