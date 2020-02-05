#ifndef IDENTINFO_H
#define IDENTINFO_H
#include "../parser/elements/CssIdentifier.h"

namespace CSS {
namespace Minification {
using namespace CSS::Parsing::Elements;

template<class T>
class IdentInfo
{
public:
    IdentInfo(const T &_identifier, bool _defined = false) :
        identifier(_identifier), defined(_defined) {}

    T identifier;
    uint32_t count {1};
    bool defined;
};

}
}

#endif // IDENTINFO_H
