#ifndef TLB_H
#define TLB_H

#include "copyright.h"
#include "utility.h"

enum PageType{
    vmcode,vminitData,vmuserStack,vmuninitData
};
class TranslationEntry {
  public:
    int virtualPage;
    int physicalPage;
    bool valid;
    bool readOnly;
    bool use;
    bool dirty;
    int inFileAddr;
    PageType type;
};
#endif
