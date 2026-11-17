//
//  Ramdisk.hpp
//  libRamdisk
//
//  Created by tihmstar on 17.11.26.
//

#ifndef Ramdisk_hpp
#define Ramdisk_hpp

#include <libgeneral/Mem.hpp>

#include <vector>

#include <stdlib.h>

namespace tihmstar {
namespace Ramdisk {

class Ramdisk{
public:
    virtual tihmstar::Mem extractFile(const char *path) = 0;
    virtual void putFile(const char *path, const void *buf, size_t bufSize) = 0;
    virtual void rm(const char *path, bool recursive = false) = 0;
    virtual std::vector<std::string> listFilesAtPath(const char *path, bool recursive = false) = 0;
    virtual void growRamdisk(size_t size) = 0;
    
    virtual const void *buf() = 0;
    virtual size_t bufSize() = 0;
    
#pragma mark static
    static std::shared_ptr<Ramdisk> make_ramdisk(const void *buf, size_t bufSize);
};

}
}

#endif /* Ramdisk_hpp */
