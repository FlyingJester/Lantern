#include "fileops.h"
#include "storage/Directory.h"

struct Lantern_FileFinder : public BDirectory{
    Lantern_FileFinder(const char *path)
      : BDirectory(path){}
    
    BEntry m_entry;
    BPath m_entry_path;
    
    bool GetNextEntry(){
        const status_t status = BDirectory::GetNextEntry(&m_entry, true);
        switch(status){
            case B_OK:
                break;
            case B_LINK_LIMIT:
            case B_BUSY:
            case B_FILE_ERROR:
                ret = finder->GetNextEntry() == B_OK;
                break;
            default:
                return false;
        }
        
        m_entry.GetPath(&m_entry_path);
    }
};

unsigned Lantern_FileFinderSize(){
    return sizeof(Lantern_FileFinder);
}

bool Lantern_InitFileFinder(struct Lantern_FileFinder *finder, const char *path){
    return new (finder) Lantern_FileFinder(path);
}

void Lantern_DestroyFileFinder(struct Lantern_FileFinder *finder){
    finder->~Lantern_FileFinder();
}

const char *Lantern_FileFinderPath(const struct Lantern_FileFinder *finder){
    return finder->m_entry_path.Path();
}

unsigned long Lantern_FileFinderFileSize(const struct Lantern_FileFinder *finder){
    return finder->m_entry_path.GetSize();
}

bool Lantern_FileFinderNext(struct Lantern_FileFinder *finder){
    return finder->GetNextEntry();
}
