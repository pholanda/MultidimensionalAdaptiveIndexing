#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <chrono>
#include <vector>
#include <numeric>

class Measurements
{
    using time_point = std::__1::chrono::steady_clock::time_point;
public:
    double initialization_time;
    std::vector<double> adaptation_time;
    std::vector<double> query_time;
    std::vector<size_t> max_height;
    std::vector<size_t> min_height;
    std::vector<size_t> number_of_nodes;
    std::vector<size_t> memory_footprint;

    Measurements();
    ~Measurements();

    time_point time();

    static double difference(time_point end, time_point start);

    double average_adaptation_time(){
        return average(adaptation_time);
    }

    double average_query_time(){
        return average(query_time);
    }

    double average(std::vector<double> v){
        return std::accumulate(v.begin(), v.end(), 0.0)/(double)v.size();
    }
};
#endif // MEASUREMENTS_H