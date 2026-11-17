//
//  Ramdisk_libdmg.hpp
//  libRamdisk
//
//  Created by tihmstar on 17.11.26.
//

#ifndef Ramdisk_libdmg_hpp
#define Ramdisk_libdmg_hpp

#include <Ramdisk/Ramdisk.hpp>
#include <libgeneral/Mem.hpp>

#include <memory>
#include <vector>
#include <iostream>

#include <stdlib.h>

extern "C" {
#include <libdmg-hfsplus/dmg/dmgfile.h>
}

class Ramdisk_libdmg : public tihmstar::Ramdisk::Ramdisk {
    void *_locbuf;
    AbstractFile *_af;
    io_func *_afio;
    Volume *_hfs;
    
    tihmstar::Mem extractFileHFS(const char *path);
    tihmstar::Mem extractFileAPFS(const char *path);
    void putFileHFS(const char *path, const void *buf, size_t bufSize);
    void rmHFS(const char *path, bool recursive = false);
    std::vector<std::string> listFilesAtPathHFS(const char *path, bool recursive = false);
public:
    Ramdisk_libdmg(const void *buf, size_t bufSize);
    ~Ramdisk_libdmg();
    
    virtual tihmstar::Mem extractFile(const char *path) override;
    virtual void putFile(const char *path, const void *buf, size_t bufSize) override;
    virtual void rm(const char *path, bool recursive = false) override;
    virtual std::vector<std::string> listFilesAtPath(const char *path, bool recursive = false) override;
    virtual void growRamdisk(size_t size) override;

    virtual const void *buf() override;
    virtual size_t bufSize() override;
};

#endif /* Ramdisk_libdmg_hpp */
