#include "table.hpp"
#include "column.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>
#include "split_string.hpp"

using namespace std;

Table::Table(size_t number_of_columns) : number_of_columns(number_of_columns){
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
    number_of_rows = 0;
}

Table::Table(Table *table_to_copy){
    number_of_rows = table_to_copy->row_count();
    number_of_columns = table_to_copy->col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (size_t col_index = 0; col_index < table_to_copy->col_count(); col_index++){
        columns[col_index] = make_unique<Column>(
            table_to_copy->columns[col_index]->data, number_of_rows
        );
    }
}

std::unique_ptr<Table> Table::read_file(std::string path){
    ifstream file(path.c_str(), ios::in);
    if(!file.is_open()){
        cout << "Error *opening* data file\n";
        exit(-1);
    }
    string line;
    if(!getline(file, line)){
        cout << "Error *reading* data file\n";
        exit(-1);
    }

    auto row = SplitString<float>::split(line, " ");
    auto table = make_unique<Table>(row.size());
    table->append(&(row[0]));

    while(getline(file,line)){
        row = SplitString<float>::split(line, " ");
        table->append(&(row[0]));
    }

    file.close();

    return table;

}

void Table::save_file(std::string path){
    ofstream file(path.c_str(), ios::out);
    if(!file.is_open()){
        cout << "Error *opening* file\n";
        exit(-1);
    }

    for(size_t i = 0; i < number_of_rows; ++i){
        for(size_t j = 0; j < number_of_columns - 1; ++j){
            file << columns[j]->data[i] << " "; 
        }
        file << columns[number_of_columns-1]->data[i] << "\n";
    }

    file.close();
}

Table::~Table(){}

std::unique_ptr<float[]> Table::materialize_row(size_t row_index){
    auto row = make_unique<float[]>(number_of_rows);
    for(size_t col = 0; col < number_of_columns; col++){
        row[col] = columns[col]->data[row_index];
    }
    return row;
}

void Table::append(float* row){
    for(size_t col = 0; col < number_of_columns; col++){
        columns[col]->append(row[col]);
    }
    number_of_rows++;
}


size_t Table::row_count() const{
    return number_of_rows;
}

size_t Table::col_count() const{
    return number_of_columns;
}

size_t Table::CrackTable(size_t low, size_t high, float element, size_t c)
{
  int64_t x1 = low;
  int64_t x2 = high - 1;

  while (x1 <= x2 && x2 > 0)
  {
    if (columns[c]->data[x1] < element)
      x1++;
    else
    {
      while (x2 > 0 && x2 >= x1 && (columns[c]->data[x2] >= element))
        x2--;
      if (x1 < x2)
      {
        exchange(x1, x2);
        x1++;
        x2--;
      }
    }
  }
  return x1;
}

pair<size_t, size_t> Table::CrackTableInThree(size_t low, size_t high, float key_left, float key_right, size_t c)
{
  auto p1 = CrackTable(low, high, key_left, c);
  auto p2 = CrackTable(p1, high, key_right, c);
  return make_pair(p1, p2);
}

void Table::exchange(size_t index1, size_t index2){
  for(size_t column_index = 0; column_index < number_of_columns; ++column_index){
    auto value1 = columns[column_index]->data[index1];
    auto value2 = columns[column_index]->data[index2];

    auto tmp = value1;
    columns[column_index]->data[index1] = value2;
    columns[column_index]->data[index2] = tmp;
  }
}
