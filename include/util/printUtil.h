#pragma once


namespace PrintUtil {


// ========================================================================== //


inline const char* createIndentation(size_t indentLevel) {
    size_t totalSpaces = indentLevel * 2;

    char* tabs = new char[totalSpaces + 1];

    for (size_t i = 0; i < totalSpaces; ++i) {
        tabs[i] = ' ';
    }

    tabs[totalSpaces] = '\0';

    return tabs;
}


// ========================================================================== //


} // namespace PrintUtil
