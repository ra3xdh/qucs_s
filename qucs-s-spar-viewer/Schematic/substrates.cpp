#include "structures.h"


// Logic operator to compare MS_Substrate structures
bool operator==(const MS_Substrate& a, const MS_Substrate& b) {
    return a.height == b.height &&
        a.er == b.er &&
        a.tand == b.tand &&
        a.MetalConductivity == b.MetalConductivity &&
        a.MetalThickness == b.MetalThickness;
}


// Remove duplicates from QList<MS_Substrate>
QList<MS_Substrate> removeDuplicates(const QList<MS_Substrate>& list) {
    QList<MS_Substrate> uniqueList;
    for (const auto& item : list) {
        if (!uniqueList.contains(item)) {
            uniqueList.append(item);
        }
    }
    return uniqueList;
}
