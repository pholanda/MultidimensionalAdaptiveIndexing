#ifndef FULL_SCAN
#define FULL_SCAN

#include "abstract_index.cpp"

class FullScan : public AbstractIndex
{
public:
    FullScan(){}
    ~FullScan(){}

    string name(){
        return "Full Scan";
    }

    void initialize(const shared_ptr<Table> table_to_copy){
        auto start = measurements->time();

        // Simply copies the pointer of the table, since it does not change anything
        table = table_to_copy;

        measurements->initialization_time = measurements->time() - start;;
    }

    void adapt_index(Query& query){
        // Zero adaptation for full scan
        measurements->adaptation_time.push_back(
            Measurements::difference(measurements->time(), measurements->time())
        );
    }

    shared_ptr<Table> range_query(Query& query){
        auto start = measurements->time();


        // Scan the table and returns a materialized view of the result.
        auto result = make_shared<Table>(table->col_count());

        scan_partition(table, query, 0, table->row_count() - 1, result);

        auto end = measurements->time();

        measurements->query_time.push_back(
            Measurements::difference(end, start)
        );

        return result;
    }

    static void scan_partition(
        shared_ptr<Table> table, Query& query,
        size_t low, size_t high,
        shared_ptr<Table> table_to_store_results
    ){
        for(size_t row_id = low; row_id <= high; row_id++)
            if(condition_is_true(table, query, row_id))
                table_to_store_results->append(table->materialize_row(row_id));
    }
private:
    bool static condition_is_true(shared_ptr<Table> table, Query& query, size_t row_index){
        for(auto predicate : query.predicates){
            auto column = predicate.column;
            auto low = predicate.low;
            auto high = predicate.high;

            auto value = table->columns.at(column)->at(row_index);
            if(!(low <= value && value < high))
                return false;
        }
        return true;
    }
};
#endif