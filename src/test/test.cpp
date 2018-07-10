#include "test.h"

extern int64_t COLUMN_SIZE,NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD, INDEXING_TYPE;

using namespace std;


vector<vector<int64_t>> range_query_baseline(Table *table, vector<vector<array<int64_t, 3>>> *queries)
{
    vector<vector<int64_t>> queryResult;
    for (size_t q = 0; q < NUM_QUERIES; ++ q){
        vector<int64_t> rowId;
        for (size_t i = 0; i < COLUMN_SIZE; ++i){
            bool match = true;
            for (size_t query_num = 0; query_num < queries->at(q).size(); query_num++){
                int64_t low = queries->at(q).at(query_num).at(0);
                int64_t high = queries->at(q).at(query_num).at(1);
                int64_t col = queries->at(q).at(query_num).at(2);
                // if(table->columns[col][i] < low || table->columns[col][i] >= high)
                if(!(
                        ((low <= table->columns[col][i]) || (low == -1)) &&
                        ((table->columns[col][i] < high) || (high == -1))
                    ))
                {
                    match = false;
                    break;
                }
            }
        if(match)
            rowId.push_back(table->ids[i]);
        }
        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }

	return queryResult;
}

vector<vector<int64_t>> vectorized_branchless_full_scan(Table *table, vector<vector<array<int64_t, 3>>> *queries)
{
    vector<vector<int64_t>> queryResult;
    for (size_t i = 0; i < NUM_QUERIES; ++ i){
        vector<int64_t> rowId;
        full_scan(table,&queries->at(i),NULL,&rowId);
        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

    // table->crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    // Tree *T;
    // if(INDEXING_TYPE == 1)
    //     T = (Tree *)malloc(sizeof(Tree) * NUMBER_OF_COLUMNS);
    // else 
    //     T = (Tree *)malloc(sizeof(Tree));

vector<vector<int64_t>> unidimensional_cracking(Table *table, vector<vector<array<int64_t, 3>>> *queries)
{
    table->crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    Tree *T = (Tree *)malloc(sizeof(Tree) * NUMBER_OF_COLUMNS);
    cracking_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;
    for (size_t i = 0; i < NUM_QUERIES; ++ i){
        vector<pair<int, int>> offsets; 
        vector<boost::dynamic_bitset<>> bitmaps(NUMBER_OF_COLUMNS);
        vector<int64_t> rowId;
        cracking_partial_built(table, T,&queries->at(i));
        cracking_index_lookup(T,&queries->at(i),&offsets);
        cracking_intersection(table, &offsets, &bitmaps, &rowId);
        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> cracking_kdtree(Table *table, vector< vector<array<int64_t, 3>>> *queries)
{
    Tree *T = (Tree *)malloc(sizeof(Tree));
    cracking_kdtree_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;

    for (size_t i = 0; i < NUM_QUERIES; ++ i){
        vector<pair<int, int>> offsets; 
        vector<int64_t> rowId;
        cracking_kdtree_partial_built(table, T,&queries->at(i));
        kdtree_index_lookup(T,&queries->at(i),&offsets);
        kdtree_scan(table,&queries->at(i), &offsets, &rowId);

        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> full_kdtree(Table *table, vector< vector<array<int64_t, 3>>> *queries)
{
    Tree *T = (Tree *)malloc(sizeof(Tree));
    full_kdtree_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;

    for (size_t i = 0; i < NUM_QUERIES; ++ i){
        vector<pair<int, int>> offsets; 
        vector<int64_t> rowId;
        kdtree_index_lookup(T,&queries->at(i),&offsets);
        kdtree_scan(table,&queries->at(i), &offsets, &rowId);

        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

void verify_range_query(vector<vector<int64_t>> queryResultBaseline,vector<vector<int64_t>> queryResultToBeTested)
{
    for (size_t i = 0; i < queryResultBaseline.size(); ++i)
        for(size_t j = 0; j < queryResultBaseline.at(i).size(); ++ j)
        	if (queryResultBaseline.at(i).at(j) != queryResultToBeTested.at(i).at(j))
        	{
        		fprintf(stderr, "Incorrect Results!\n");
        		fprintf(stderr, "Query: %ld\n", i);
        		fprintf(stderr, "Expected: %ld Got: %ld\n", queryResultBaseline.at(i).at(j), queryResultToBeTested.at(i).at(j));
        		assert(0);
        	}
}



void verifyAlgorithms(Table *table, vector<vector<array<int64_t, 3>>> rangeQueries){
    vector<vector<int64_t>> queryResultToBeTested;
    fprintf(stderr, "Running Baseline.\n");
    vector<vector<int64_t>> queryResultBaseline = range_query_baseline(table,&rangeQueries);

    fprintf(stderr, "Running Vectorized Branchless Scan.\n");
    queryResultToBeTested = vectorized_branchless_full_scan(table,&rangeQueries);
    verify_range_query(queryResultBaseline,queryResultToBeTested);
   
    fprintf(stderr, "Running Unidimensional Cracking.\n");
    queryResultToBeTested = unidimensional_cracking(table,&rangeQueries);
    verify_range_query(queryResultBaseline,queryResultToBeTested);

    fprintf(stderr, "Running Cracking KD-Tree.\n");
    queryResultToBeTested = cracking_kdtree(table,&rangeQueries);
    verify_range_query(queryResultBaseline,queryResultToBeTested);
    
    fprintf(stderr, "Running Full Kd-Tree.\n");
    queryResultToBeTested = full_kdtree(table,&rangeQueries);
    verify_range_query(queryResultBaseline,queryResultToBeTested);

    fprintf(stderr, "Everything works!\n");

        
}