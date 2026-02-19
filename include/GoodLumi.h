#ifndef GOODLUMI_H
#define GOODLUMI_H

#include <map>
#include <vector>
#include <fstream>
#include "../external/nlohmann/json.hpp"

using json = nlohmann::json;

std::map<int, std::vector<std::pair<int,int>>> goodLumiMap;

bool LoadGoldenJSON(const char* filename)
{
    std::ifstream f(filename);
    if (!f.is_open()) return false;

    json j;
    f >> j;
    for (auto &it: j.items()) {
        int run = std::stoi(it.key());
        for (auto &range: it.value()) {
            int start = range[0];
            int end   = range[1];
            goodLumiMap[run].push_back({start, end});
        }
    }
    return true;
}

bool PassesGoodLumi(int run, int lumi)
{
    auto it = goodLumiMap.find(run);
    if (it == goodLumiMap.end()) return false;

    for (auto &range : it->second) {
        if (lumi >= range.first && lumi <= range.second)
            return true;
    }
    return false;
}

#endif
