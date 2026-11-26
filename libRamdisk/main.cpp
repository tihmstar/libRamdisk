//
//  main.cpp
//  libRamdisk
//
//  Created by tihmstar on 17.11.26.
//

#include <libgeneral/macros.h>
#include <libgeneral/Mem.hpp>
#include <libgeneral/Utils.hpp>

#include "Ramdisk.hpp"

int main(int argc, const char * argv[]) {
    info("test");

    auto f = tihmstar::readFile(argv[1]);

    auto rd = tihmstar::Ramdisk::Ramdisk::make_ramdisk(f.data(), f.size());
    
    rd->rm("/usr/local/standalone/firmware/ICE2_01.45.00.fls");
    rd->rm("/usr/local/standalone/firmware/ICE2_01.45.00.eep");
    
    {
        auto ff = tihmstar::readFile("/private/tmp/kk/ICE2_06.15.00.eep");
        rd->putFile("/usr/local/standalone/firmware/ICE2_06.15.00.eep", ff.data(), ff.size());
    }
    {
        auto ff = tihmstar::readFile("/private/tmp/kk/ICE2_06.15.00.fls");
        rd->putFile("/usr/local/standalone/firmware/ICE2_06.15.00.fls", ff.data(), ff.size());
    }

    info("done");
    return 0;
}
