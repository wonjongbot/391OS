import os

'''
f = open("student-distrib/scripts/protos.txt", "w")

for i in range(32):
    f.write(f"extern void INT{i}();\n")
'''

'''
f = open("student-distrib/scripts/protos_linker_global.txt", "w")

for i in range(32):
    f.write(f"INT{i}_linker, ")
'''

'''
f = open("student-distrib/scripts/protos_linker_S.txt", "w")

for i in range(32):
    f.write(f"INT{i}_linker:\nPUSHAL\ncall INT{i}\nPOPAL\nIRET\n\n")
    '''
'''
f = open("student-distrib/scripts/protos_linker_ss.txt", "w")

for i in range(32):
    f.write(f"CREATE_LINKER(INT{i}_linker, INT{i})\n")
'''


f = open("student-distrib/scripts/proto_h.txt", "w")

for i in range(32):
    f.write(f"void INT{i}_linker();\n")
    