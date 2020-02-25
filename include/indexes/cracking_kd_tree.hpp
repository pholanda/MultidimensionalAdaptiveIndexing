#ifndef CRACKING_KD_TREE_HPP
#define CRACKING_KD_TREE_HPP

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>

class CrackingKDTree : public AbstractIndex
{
    public:
    CrackingKDTree(std::map<std::string, std::string> config);
    ~CrackingKDTree();

    string name() override{
        return "CrackingKDTree";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    void draw_index(std::string path) override{
        index->draw(path);
    }

private:
    unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;

    void adapt(Query& query);

    void insert_point(std::vector<float> point, size_t is_right_hand_side);

    std::vector<std::vector<float> > query_to_points(Query& query);

    void crack_point(
            std::vector<float> point,   // point to be inserted
            size_t is_right_hand_side,  // if each axis of the point comes
                                        //  from the right side of query
            std::vector<bool> should_insert, // if the axis should be inserted
            KDNode* current, // node to insert the new point
            int64_t low_position,       // lower position from partition
            int64_t high_position      // upper position from partition
            );

    int64_t next_dim(int64_t start, std::vector<bool> should_insert);

    bool all_elements_false(std::vector<bool> v);
};

#endif