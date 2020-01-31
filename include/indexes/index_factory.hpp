#ifndef INDEX_FACTORY_H
#define INDEX_FACTORY_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
//#include "cracking_kd_tree_broad.hpp"
//#include "cracking_kd_tree_narrow.hpp"
//#include "median_kd_tree.hpp"
//#include "average_kd_tree.hpp"
#include "quasii.hpp"
#include "standard_cracking.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class IndexFactory
{
public:
    static unique_ptr<AbstractIndex> getIndex(
            string index_name,
            map<string, string> config = map<string, string>()
        ){
        if(index_name == "Full-Scan")
            return make_unique<FullScan>(config);
        //if(index_name == "Cracking KD-Tree Broad")
        //    return make_unique<CrackingKDTreeBroad>(config);
        //if(index_name == "Cracking KD-Tree Narrow")
        //    return make_unique<CrackingKDTreeNarrow>(config);
        //if(index_name == "KDTree-Median")
        //    return make_unique<MedianKDTree>(config);
        //if(index_name == "KDTree-Average")
        //    return make_unique<AverageKDTree>(config);
        if(index_name == "Quasii")
            return make_unique<Quasii>(config);
        if(index_name == "Standard Cracking")
            return make_unique<StandardCracking>(config);

        assert(false);
    }

    static vector<shared_ptr<AbstractIndex>> allIndexes(
            map<string, string> config = map<string, string>()
        ){
        vector<shared_ptr<AbstractIndex>> indexes;
        //indexes.push_back(make_unique<CrackingKDTreeBroad>(config));
        //indexes.push_back(make_unique<CrackingKDTreeNarrow>(config));
        indexes.push_back(make_unique<MedianKDTree>(config));
        indexes.push_back(make_unique<AverageKDTree>(config));
        indexes.push_back(make_unique<Quasii>(config));
        indexes.push_back(make_unique<StandardCracking>(config));
        return indexes;
    }

    static shared_ptr<AbstractIndex> baselineIndex(
            map<string, string> config = map<string, string>()
        ){
        return make_unique<FullScan>(config);
    }
};
#endif // INDEX_FACTORY_H
