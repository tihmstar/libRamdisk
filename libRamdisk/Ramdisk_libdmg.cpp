//
//  Ramdisk_libdmg.cpp
//  libRamdisk
//
//  Created by tihmstar on 17.11.26.
//
#include <libgeneral/macros.h>

#include "Ramdisk_libdmg.hpp"
#include "../include/Ramdisk/Ramdisk.hpp"

#if defined(WIN32) && !__clang__
#   define RAMDISK_API __declspec( dllexport )
#else
#   ifdef HAVE_FVISIBILITY
#       define RAMDISK_API __attribute__((visibility("default")))
#   else
#       define RAMDISK_API
#   endif
#endif

extern "C" {
#include <libdmg-hfsplus/hfs/hfslib.h>
char endianness;
}


#pragma mark Ramdisk_libdmg
Ramdisk_libdmg::Ramdisk_libdmg(const void *buf, size_t bufSize){
    {
        short int word = 0x0001;
        char *byte = (char *) &word;
        endianness = byte[0] ? IS_LITTLE_ENDIAN : IS_BIG_ENDIAN;
    }
    _locbuf = malloc(bufSize);
    memcpy(_locbuf, buf, bufSize);
    _af = createAbstractFileFromMemory(&_locbuf, bufSize);
    _afio = IOFuncFromAbstractFile(_af);
    _hfs = openVolume(_afio);
}

Ramdisk_libdmg::~Ramdisk_libdmg(){
    safeFreeCustom(_hfs, closeVolume);
    if (_afio) {
        _afio->close(_afio);
        _afio = NULL;
    }else if (_af) {
        _af->close(_af);
        _af = NULL;
    }
    _locbuf = NULL;
}

#pragma mark private
tihmstar::Mem Ramdisk_libdmg::extractFileHFS(const char *path){
    void *buf = NULL;
    AbstractFile *af = NULL;
    cleanup([&]{
        if (af){
            af->close(af); af = NULL;
        }
        safeFree(buf);
    });
    buf = malloc(0x1000*20);
    af = createAbstractFileFromMemory(&buf, 0x1000*20);
    get_hfs(_hfs, path, af);
    size_t realSize = af->tell(af);
    return {(const void*)buf,realSize};
}

tihmstar::Mem Ramdisk_libdmg::extractFileAPFS(const char *path){
    reterror("TODO");
}

void Ramdisk_libdmg::putFileHFS(const char *path, const void *buf, size_t bufSize){
    void *locbuf = NULL;
    AbstractFile *af = NULL;
    cleanup([&]{
        if (af){
            af->close(af); af = NULL;
        }
        safeFree(locbuf);
    });
    bool hasPath = false;
    try {
        listFilesAtPath(path);
        hasPath = true;
    } catch (...) {
        //
    }
    if (hasPath) ::removeFile(path, _hfs);
    if (_hfs->volumeHeader->freeBlocks < 10 || (_hfs->volumeHeader->freeBlocks-10) * _hfs->volumeHeader->blockSize < bufSize) {
        growRamdisk(bufSize);
    }

    locbuf = malloc(bufSize);
    memcpy(locbuf, buf, bufSize);
    af = createAbstractFileFromMemory(&locbuf, bufSize);
    add_hfs(_hfs, af, path); af = NULL;
    chmodFile(path, 0777, _hfs);
}

void Ramdisk_libdmg::rmHFS(const char *path, bool recursive){
    if (recursive) {
        auto paths = listFilesAtPath(path);
        for (auto p = paths.rbegin(); p != paths.rend(); ++p){
            ::removeFile(p->c_str(), _hfs);
        }
    }else{
        ::removeFile(path, _hfs);
    }
}

std::vector<std::string> Ramdisk_libdmg::listFilesAtPathHFS(const char *path, bool recursive){
    HFSPlusCatalogRecord *record = NULL;
    cleanup([&]{
        safeFree(record);
    });
    char* name = NULL; //dont't free this? idk
    std::vector<std::string> ret;
    assure(record = getRecordFromPath(path, _hfs, &name, NULL));
    
    if(record != NULL) {
        if(record->recordType == kHFSPlusFolderRecord){
            CatalogRecordList* theList = NULL;
            cleanup([&]{
                safeFreeCustom(theList, releaseCatalogRecordList);
            });
            std::string self = path;
            if (self.back() != '/') self += '/';
            ret.push_back(self);
            for (CatalogRecordList* list = (theList = getFolderContents(((HFSPlusCatalogFolder*)record)->folderID, _hfs)); list; list = list->next) {
                char *curname = NULL;
                cleanup([&]{
                    safeFree(curname);
                });
                curname = unicodeToAscii(&list->name);
                std::string cur = self + curname;
                if (strncmp(curname, ".HFS+ Private Directory Data", sizeof(".HFS+ Private Directory Data")-1) == 0) continue;
                if (!strlen(curname)) continue;
                if(list->record->recordType == kHFSPlusFolderRecord) {
                    auto retlist = listFilesAtPathHFS(cur.c_str(), recursive);
                    ret.insert(ret.end(), retlist.begin(), retlist.end());
                } else if(list->record->recordType == kHFSPlusFileRecord) {
                    ret.push_back(cur);
                }
            }
        }else{
            return {path};
        }
    } else {
        return {path};
    }
    return ret;
}

#pragma mark public
tihmstar::Mem Ramdisk_libdmg::extractFile(const char *path){
    return extractFileHFS(path);
    reterror("No ramdisk backend available for FS Type");
}

void Ramdisk_libdmg::putFile(const char *path, const void *buf, size_t bufSize){
    return putFileHFS(path, buf, bufSize);
    reterror("No ramdisk backend available for file creation");
}

void Ramdisk_libdmg::rm(const char *path, bool recursive){
    return rmHFS(path, recursive);
    reterror("No ramdisk backend available for file deleting");
}

std::vector<std::string> Ramdisk_libdmg::listFilesAtPath(const char *path, bool recursive){
    return listFilesAtPathHFS(path, recursive);
    reterror("No ramdisk backend available for file listing");
}

void Ramdisk_libdmg::growRamdisk(size_t size){
    size_t addsize = size & ~0x3fff;
    if (size & 0x3fff) addsize += 0x4000;
    grow_hfs(_hfs, _hfs->volumeHeader->totalBlocks * _hfs->volumeHeader->blockSize +addsize);
}

const void *Ramdisk_libdmg::buf(){
    return _locbuf;
}

size_t Ramdisk_libdmg::bufSize(){
    return _hfs->volumeHeader->totalBlocks * _hfs->volumeHeader->blockSize;
}

#pragma mark lib
RAMDISK_API std::shared_ptr<tihmstar::Ramdisk::Ramdisk> tihmstar::Ramdisk::Ramdisk::make_ramdisk(const void *buf, size_t bufSize){
    return std::make_shared<Ramdisk_libdmg>(buf, bufSize);
}
