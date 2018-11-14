#include "test.h"

extern int64_t COLUMN_SIZE, NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD, INDEXING_TYPE;

using namespace std;


vector<vector<int64_t>> range_query_baseline(Table *table, vector<vector<array<int64_t, 3>>> *queries) {
    vector<vector<int64_t>> queryResult;
    for (size_t q = 0; q < NUM_QUERIES; ++q) {
        vector<int64_t> rowId;
        for (size_t i = 0; i < COLUMN_SIZE; ++i) {
            bool match = true;
            for (size_t query_num = 0; query_num < queries->at(q).size(); query_num++) {
                int64_t low = queries->at(q).at(query_num).at(0);
                int64_t high = queries->at(q).at(query_num).at(1);
                int64_t col = queries->at(q).at(query_num).at(2);
                if (!(
                        ((low <= table->columns[col][i]) || (low == -1)) &&
                        ((table->columns[col][i] < high) || (high == -1))
                )) {
                    match = false;
                    break;
                }
            }
            if (match)
                rowId.push_back(table->ids[i]);
        }
        sort(rowId.begin(), rowId.end());
        queryResult.push_back(rowId);
    }

    return queryResult;
}

vector<vector<int64_t>> vectorized_branchless_full_scan(Table *table, vector<vector<array<int64_t, 3>>> *queries) {
    vector<vector<int64_t>> queryResult;
    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        vector<int64_t> rowId;
        full_scan(table, &queries->at(i), NULL, &rowId);
        sort(rowId.begin(), rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> unidimensional_cracking(Table *table, vector<vector<array<int64_t, 3>>> *queries) {
    table->crackercolumns = (IndexEntry **) malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    Tree *T = (Tree *) malloc(sizeof(Tree) * NUMBER_OF_COLUMNS);
    cracking_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;
    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        vector<pair<int, int>> offsets;
        vector<int64_t> rowId;
        vector<vector<bool>> bitmaps(NUMBER_OF_COLUMNS);
        cracking_partial_built(table, T, &queries->at(i));
        cracking_index_lookup(T, &queries->at(i), &offsets);
        cracking_intersection(table, &offsets, &bitmaps, &rowId);
        sort(rowId.begin(), rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> cracking_kdtree(Table *table, vector<vector<array<int64_t, 3>>> *queries) {
    Tree *T = (Tree *) malloc(sizeof(Tree));
    cracking_kdtree_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;

    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        vector<pair<int, int>> offsets;
        vector<int64_t> rowId;
        cracking_kdtree_partial_built(table, T, &queries->at(i));
        kdtree_index_lookup(T, &queries->at(i), &offsets);
        kdtree_scan(table, &queries->at(i), &offsets, &rowId);

        sort(rowId.begin(), rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> full_kdtree(Table *table, vector<vector<array<int64_t, 3>>> *queries) {
    Tree *T = (Tree *) malloc(sizeof(Tree));
    full_kdtree_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;

    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        vector<pair<int, int>> offsets;
        vector<int64_t> rowId;
        kdtree_index_lookup(T, &queries->at(i), &offsets);
        kdtree_scan(table, &queries->at(i), &offsets, &rowId);

        sort(rowId.begin(), rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> sideways_cracking(Table *table, vector<vector<array<int64_t, 3>>> *queries) {
    Tree *T = (Tree *) malloc(sizeof(Tree) * NUMBER_OF_COLUMNS);
    sideways_cracking_pre_processing(table, T);
    vector<vector<int64_t>> queryResult;

    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        vector<pair<int, int>> offsets;
        vector<int64_t> rowId;
        sideways_cracking_partial_built(table, T, &queries->at(i));
        sideways_cracking_index_lookup(T, &queries->at(i), &offsets);
        sideways_cracking_scan(table, &queries->at(i), &offsets, &rowId);
        sort(rowId.begin(), rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

int64_t verify_range_query(vector<vector<int64_t>> queryResultBaseline, vector<vector<int64_t>> queryResultToBeTested) {
    int64_t n_w = 0;
    for (int64_t i = 0; i < queryResultBaseline.size(); ++i) {
        if (queryResultBaseline.at(i).size() != queryResultToBeTested.at(i).size()) {
            fprintf(stderr, "Different sizes(%ld): %zu | %zu (%ld)\n", i, queryResultBaseline.at(i).size(),
                    queryResultToBeTested.at(i).size(), (int64_t) queryResultBaseline.at(i).size() - (int64_t) queryResultToBeTested.at(i).size());
            n_w++;
            continue;
        }
        for (int64_t j = 0; j < queryResultBaseline.at(i).size(); ++j) {
            if (queryResultBaseline.at(i).at(j) != queryResultToBeTested.at(i).at(j)) {
                fprintf(stderr, "Different values: %ld | %ld\n",
                        queryResultBaseline.at(i).at(j), queryResultToBeTested.at(i).at(j));
                n_w++;
                break;
            }
        }
    }
    return n_w;
}


void verifyAlgorithms(Table *table, vector<vector<array<int64_t, 3>>> rangeQueries) {

    vector<vector<int64_t>> queryResultToBeTested;
    fprintf(stderr, "Running Baseline.\n");
    vector<vector<int64_t>> queryResultBaseline = range_query_baseline(table, &rangeQueries);

    fprintf(stderr, "Running Vectorized Branchless Scan.\n");
    queryResultToBeTested = vectorized_branchless_full_scan(table,&rangeQueries);
    int64_t fs = verify_range_query(queryResultBaseline,queryResultToBeTested);

    fprintf(stderr, "Running Unidimensional Cracking.\n");
    queryResultToBeTested = unidimensional_cracking(table,&rangeQueries);
    int64_t std = verify_range_query(queryResultBaseline,queryResultToBeTested);

    fprintf(stderr, "Running Cracking KD-Tree.\n");
    queryResultToBeTested = cracking_kdtree(table,&rangeQueries);
    int64_t ckd = verify_range_query(queryResultBaseline,queryResultToBeTested);

    fprintf(stderr, "Running Full Kd-Tree.\n");
    queryResultToBeTested = full_kdtree(table,&rangeQueries);
    int64_t kd = verify_range_query(queryResultBaseline,queryResultToBeTested);

    fprintf(stderr, "Running Sideways Cracking.\n");
    queryResultToBeTested = sideways_cracking(table, &rangeQueries);
    int64_t sw = verify_range_query(queryResultBaseline, queryResultToBeTested);


    fprintf(stderr, "SUMMARY------------------------------------------------\n");
    fprintf(stderr, "|Full Scan - Number of errors: %ld\n", fs);
    fprintf(stderr, "|Unidimensional Crackig - Number of errors: %ld\n", std);
    fprintf(stderr, "|Cracking KD - Number of errors: %ld\n", ckd);
    fprintf(stderr, "|Full KD - Number of errors: %ld\n", kd);
    fprintf(stderr, "|Sideways Cracking - Number of errors: %ld\n", sw);

    double avg_selec = 0.0;
    for (size_t i = 0; i < rangeQueries.size(); i++) {
        avg_selec += static_cast<double>(queryResultBaseline.at(i).size()) / static_cast<double>(table->ids.size());
    }

    fprintf(stderr, "|Average Selectivity: %f\n", avg_selec / static_cast<int64_t>(rangeQueries.size()));

    fprintf(stderr, "-------------------------------------------------------\n");


}
