#ifndef SLICE_H
#define SLICE_H

#include <cstddef>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

// How the Slice looks like:
//         |============================|
//         | Values go from:            |
//         |    left_value (>=)         |
//         |    right_value (<)         |
//         | Column: X1                 |
//         |============================|
// Table Start Pos (>=)           Table End Pos (<)

class Slice{
public:
    int64_t column;
    int64_t offset_begin;
    int64_t offset_end;
    float left_value;
    float right_value;
    vector<Slice> children;

    Slice(int64_t column, int64_t offset_begin, int64_t offset_end, float left_value, float right_value);

    Slice(const Slice &other);

    // "Open" slice, covers the entire range
    Slice(int64_t column, int64_t offset_begin, int64_t offset_end);

    Slice();
    ~Slice();

    string label();

    bool equal(const Slice &other);

    bool intersects(float low, float high);

    int64_t size();

};

#endif // SLICE_H
