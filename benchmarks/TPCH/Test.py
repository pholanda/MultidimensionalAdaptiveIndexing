import os
import pandas as pd
import numpy as np
import inspect

os.system('rm src/util/define.h')
file = open('src/util/define.h',"w")
file.write('#define test')
file.close()

SCRIPT_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
os.chdir(SCRIPT_PATH) # setting current dir as script path

PATH = ""

# Setting Values For Algorithms
FULL_SCAN = "0"
STANDARD_CRACKING = "1"
CRACKING_KD_TREE = "2"
FULL_KD_TREE = "3"

# CONFIGURATIONS
SCALE_FACTOR = 0.1
NUM_QUERIES = 10
KDTREE_THRESHOLD = '2000'  # Only used for KDTree

# COLUMNS IN LINEITEM
# 0 = ORDERKEY
# 1 = PARTKEY
# 2 = SUPPKEY
# 3 = LINENUMBER
# 4 = QUANTITY
# 5 = EXTENDED-PRICE
# 6 = DISCOUNT
# 7 = TAX
# 8 = RETURN-FLAG
# 9 = LINESTATUS
# 10 = SHIP-DATE
# 11 = COMMIT-DATE
# 12 = RECEIPT-DATE
# 13 = SHIP-INSTRUCTIONS
# 14 = SHIP-MODE
# 15 = COMMENT

def dateparse(dates):
    return [pd.datetime.strptime(d, '%Y-%m-%d') for d in dates]

def generate_lineitem():
    print("#### Generating lineitem table ####")
    os.chdir("tpch-dbgen")
    os.system("./dbgen -T L -f -s " + str(SCALE_FACTOR))
    os.chdir(SCRIPT_PATH)
    os.system("mv tpch-dbgen/lineitem.tbl .")

def fix_table_and_save_to_csv():
    print("#### Reading table ####")
    table = pd.read_csv('lineitem.tbl', sep='|', header=None, parse_dates=[10, 11, 12], date_parser=dateparse)
    # Columns 10, 11 and 12 are dates
    print("#### Transforming dates into integers ####")
    table[10] = table[10].values.astype(np.int64)
    table[11] = table[11].values.astype(np.int64)
    table[12] = table[12].values.astype(np.int64)
    print("#### Saving to CSV ####")
    table.to_csv('lineitem.csv', sep=';', header=False, index=False)
    os.system('rm lineitem.tbl')
    

def generate_queries():
    print("#### Generating queries ####")
    os.chdir("tpch-dbgen")
    for _ in xrange(NUM_QUERIES):
        os.system("./qgen 6 -s " + str(SCALE_FACTOR) + " -l queries.tbl")
    os.chdir(SCRIPT_PATH)
    os.system("mv tpch-dbgen/queries.tbl .")

def fix_queries():
    print("#### Reading queries ####")
    # Ignoring first column, just the query number
    table = pd.read_table('queries.tbl', header=None,
                          parse_dates=[1], date_parser=dateparse,
                          usecols=[1, 2, 3]
                         )
    fixed_table = pd.DataFrame()
    # Fixing dates
    print("#### Fixing Dates ####")
    fixed_table['low_date'] = table[1].values.astype(np.int64)
    fixed_table['high_date'] = (table[1] + pd.DateOffset(years=1)).values.astype(np.int64)

    # Fixing floats
    print("#### Fixing Floats ####")
    table[2] = table[2].astype(np.float) * 100
    fixed_table['low_float'] = (table[2] - 1) 
    fixed_table['high_float'] = (table[2] + 1)

    # Transforming one-sided range query to two sided
    print("#### Fixing One Sided Queries ####")
    table[3] = table[3].astype(np.float)
    fixed_table['low_quantity'] = -1
    fixed_table['high_quantity'] = table[3]

    print("#### Saving to CSV ####")
    c_order = ['low_date', 'high_date', 'low_float', 'high_float', 'low_quantity', 'high_quantity']
    fixed_table.to_csv('queries.csv', sep=';', header=False, index=False, columns=c_order)
    os.system('rm queries.tbl')


# Saving Experiments
if os.path.exists("ResultsTPCH/") != 1:
    os.system('mkdir ResultsTPCH')

# SCRIPT START

if os.path.exists("lineitem.csv") != 1:
    generate_lineitem()
    fix_table_and_save_to_csv()
    print("#### Data generation complete ####")

generate_queries()
fix_queries()
print("#### Queries generation complete ####")

os.chdir("../../")

print("Compiling")
os.environ['OPT'] = 'true'
if os.system('make') != 0:
    print("Make Failed")
    exit()

print('Testing Algorithms')
os.system("./crackingtpch --num-queries=" + str(NUM_QUERIES) + " --indexing-type=0" +" --kdtree-threshold=" + str(KDTREE_THRESHOLD))