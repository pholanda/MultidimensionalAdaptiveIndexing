#ifndef ABSTRACT_INDEX_H
#define ABSTRACT_INDEX_H

#include "../helpers/measurements.hpp"
#include "../helpers/query.hpp"
#include "../helpers/table.hpp"
#include <index_table.hpp>
#include <string>
#include <vector>

class AbstractIndex
{
protected:
    // Table with copy of the data
    unique_ptr<IdxTbl> table;
    int64_t n_tuples_scanned_before_adapting;
public:
    // Class to keep track of the time/index measurements
    unique_ptr<Measurements> measurements;

    AbstractIndex(){
        measurements = make_unique<Measurements>();
    }
    virtual ~AbstractIndex(){}
    virtual void initialize(Table *table_to_copy) = 0;
    virtual void adapt_index(Query& query) = 0;
    virtual Table range_query(Query& query) = 0;
    virtual string name() = 0;
    virtual void draw_index(std::string path){}
};
#endif // ABSTRACT_INDEX_H
