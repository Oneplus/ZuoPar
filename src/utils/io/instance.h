#ifndef __ZUOPAR_UTILS_IO_INSTANCE_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_H__

#include <iostream>
#include "types/dependency.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

void write_dependency_instance(std::ostream& os,
    const Dependency& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& deprels_alphabet);

} //  end for namespace io
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_H__
