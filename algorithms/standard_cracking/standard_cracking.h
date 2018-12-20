#ifndef STANDARD_CRACKING_H
#define STANDARD_CRACKING_H

#include "../interface.h"
#include "avl_tree.h"
#include <boost/dynamic_bitset.hpp>

class StandardCracking : public Algorithm {
    typedef vector<pair<int64_t, int64_t> > CrackerColumn;
    public:
        // Copies the table to the algorithms data structures and initialize
        // the data structures
        // ids: Vector of ID's
        // columns: data in column format
        void pre_processing(
            vector<int64_t> &ids,
            vector<vector<int64_t> > &columns
        );

        // Executes the partial index build step on adaptive algorithms
        // query: Set of predicates on each column
        // The first position is the low value
        // The second position is the high value
        // The third position is the column
        void partial_index_build(
            vector<array<int64_t, 3> > &query
        );

        // Executes the range search on each required attribute
        // Stores internally the partitions that need to be scanned
        // query: follows the same pattern described in the partial_index_build™
        void search(
            vector<array<int64_t, 3> > &query
        );

        // Scans the partitions defined on the search process
        // Holds the intermediate results inside the class
        void scan(vector<array<int64_t, 3> > &query);

        // For the algorithms that need to intersect the partial results
        void intersect();

        vector<int64_t> get_result();
    private:
        vector<CrackerColumn> cracker_columns;
        vector<Tree> index;
        vector<array<int64_t, 3> > offsets;
        vector<boost::dynamic_bitset<> > bitsets;
        size_t data_size;
        size_t number_of_columns;

        void exchange(CrackerColumn &cracker_column, int64_t x1, int64_t x2);
        int crackInTwoItemWise(
            CrackerColumn &cracker_column,
            int64_t posL, int64_t posH, int64_t med
        );
        IntPair crackInThreeItemWise(
            CrackerColumn &cracker_column,
            int64_t posL, int64_t posH,
            int64_t low, int64_t high
        );
        Tree standardCracking(
            CrackerColumn &cracker_column,
            Tree &T,
            int64_t lowKey,
            int64_t highKey
        );
};

#endif // STANDARD_CRACKING_H
