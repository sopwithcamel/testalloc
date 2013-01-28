import os.path

src_files = Glob('./*.cpp')

env = Environment()
env.Append(CCFLAGS = ['-g','-O2','-Wall'])

#jemalloc binary
jem_bin = env.Program('test_jem', src_files, LIBS='jemalloc')

#hoard binary; uncomment if required
#hoard_bin = env.Program('test_hoard', src_files, LIBS='hoard')
