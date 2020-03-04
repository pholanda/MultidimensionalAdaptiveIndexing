#ifndef INDEX_FACTORY_H
#define INDEX_FACTORY_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
#include "full_scan_slow.hpp"
#include "median_kd_tree.hpp"
#include "average_kd_tree.hpp"
#include "quasii.hpp"
#include "cracking_kd_tree.hpp"
#include "cracking_kd_tree_mine.hpp"
#include "cracking_kd_tree_recursive.hpp"
#include "cracking_kd_tree_faces.hpp"
#include "cracking_kd_tree_recursive_with_recursive_search.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

using namespace std;

class IndexFactory
{
public:
    static unique_ptr<AbstractIndex> getIndex(
            size_t algorithm_id,
            map<string, string> config = map<string, string>()
        ){
        switch(algorithm_id){
            case FullScan::ID:
                return make_unique<FullScan>(config);
            case FullScanSlow::ID:
                return make_unique<FullScanSlow>(config);
            case MedianKDTree::ID:
                return make_unique<MedianKDTree>(config);
            case CrackingKDTree::ID:
                return make_unique<CrackingKDTree>(config);
            case CrackingKDTreeMine::ID:
                return make_unique<CrackingKDTreeMine>(config);
            case CrackingKDTreeFaces::ID:
                return make_unique<CrackingKDTreeFaces>(config);
            case CrackingKDTreeRecursive::ID:
                return make_unique<CrackingKDTreeRecursive>(config);
            case CrackingKDTreeRecursiveWithRecursiveSearch::ID:
                return make_unique<CrackingKDTreeRecursiveWithRecursiveSearch>(config);
            case AverageKDTree::ID:
                return make_unique<AverageKDTree>(config);
            case Quasii::ID:
                return make_unique<Quasii>(config);
            default:
                throw std::invalid_argument("Invalid Algorithm ID");
                assert(false);
        }
    }

    static vector<size_t> algorithmIDs(){
        return {
                //FullScan::ID,
                //MedianKDTree::ID,
                CrackingKDTree::ID,
                CrackingKDTreeMine::ID,
                CrackingKDTreeRecursive::ID,
                CrackingKDTreeFaces::ID,
                CrackingKDTreeRecursiveWithRecursiveSearch::ID,
                //AverageKDTree::ID,
                Quasii::ID
        };
    }

    static vector<shared_ptr<AbstractIndex>> allIndexes(
            map<string, string> config = map<string, string>()
        ){
        return {
            make_unique<CrackingKDTree>(config),
            make_unique<CrackingKDTreeMine>(config),
            make_unique<CrackingKDTreeRecursive>(config),
            make_unique<CrackingKDTreeFaces>(config),
            make_unique<CrackingKDTreeRecursiveWithRecursiveSearch>(config),
            make_unique<MedianKDTree>(config),
            make_unique<AverageKDTree>(config),
            make_unique<Quasii>(config)
        };
    }

    static shared_ptr<AbstractIndex> baselineIndex(
            map<string, string> config = map<string, string>()
        ){
        return make_unique<FullScanSlow>(config);
    }
};
#endif // INDEX_FACTORY_H
