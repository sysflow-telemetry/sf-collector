import sys
import glob
import os
schema_json = "....." 
output_directory = "python" 
from avrogen import write_schema_files 


for file in glob.glob("../avsc/SysFlow.avsc"):
    with open(file, 'r') as myfile:
        schema_json=myfile.read().replace('\n', '')
    base=os.path.basename(file)
    name = os.path.splitext(base)[0].lower()
    dir = "classes/" + name
    write_schema_files(schema_json, dir) 
