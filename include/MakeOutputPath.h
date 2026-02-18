// MakeSkimOutputPath.h
#ifndef MAKE_SKIM_OUTPUT_PATH_H
#define MAKE_SKIM_OUTPUT_PATH_H

#include "TSystem.h"
#include <string>
#include <cstring>
#include <iostream>

inline std::string MakeOutputPath(const char* sampleType,
                                      const char* inFileName,
                                      const char* baseOutDir = "/u/user/sucho/SE_UserHome/reco")
{
    if (!sampleType || !inFileName) {
        std::cerr << "[ERROR] sampleType or inFileName is null!\n";
        return "";
    }

    std::string in(inFileName);
    std::string base(baseOutDir);

    std::string key;
    if (strcasecmp(sampleType, "mc") == 0) {
        key = "mc/";
    }
    else if (strcasecmp(sampleType, "data") == 0) {
        key = "data/";
    }
    else {
        std::cerr << "[WARNING] sampleType should be 'mc' or 'data'. Given: "
                  << sampleType << "\n"
                  << " → Falling back to 'data/' key\n";
        key = "data/";
    }

    size_t pos = in.rfind(key);
    std::string relPath;
    if (pos != std::string::npos) {
        relPath = in.substr(pos);  // mc/... 또는 data/...
    } else {
        relPath = gSystem->BaseName(inFileName);
    }

    std::string tmp = "/" + relPath;
    std::string relDir  = gSystem->DirName(tmp.c_str());
    std::string relFile = gSystem->BaseName(tmp.c_str());

    if (!relDir.empty() && relDir[0] == '/') relDir.erase(0,1);

    std::string outDir = base + "/" + relDir;
    gSystem->mkdir(outDir.c_str(), true);

    const std::string ext = ".root";
    size_t dot = relFile.rfind(ext);
    if (dot != std::string::npos) {
        relFile.insert(dot, "_reco");
    } else {
        relFile += "_reco";
    }

    return outDir + "/" + relFile;
}

#endif

