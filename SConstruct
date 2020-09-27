import os

env = Environment(ENV=os.environ)
opts = Variables([], ARGUMENTS)

opts.Add(EnumVariable('platform', 'Compilation platform', 'windows', ['windows', 'linux', 'osx']))
opts.Add(EnumVariable('bits', 'Compilation bits', '64', ['32', '64']))

opts.Update(env)

if env['platform'] == '':
    print("No valid target platform selected.")
    quit()

if env['platform'] == 'windows':
    if env['bits'] == '64':
        env = Environment(TARGET_ARCH='amd64')
    elif env['bits'] == '32':
        env = Environment(TARGET_ARCH='x86')
    opts.Update(env)
    env.Append(CPPDEFINES=['WIN32', '_WIN32', '_WINDOWS', '_CRT_SECURE_NO_WARNINGS', 'NDEBUG'])
    env.Append(CCFLAGS=['/W3', '/GR', '/O2', '/EHsc', '/MD', '/wd4244', '/wd4305'])

env.Append(CPPPATH=[
    'src/',
    'godot-cpp/godot_headers/',
    'godot-cpp/include/',
    'godot-cpp/include/core/',
    'godot-cpp/include/gen/'
])

env.Append(LIBPATH=['godot-cpp/bin/'])
env.Append(LIBS=['libgodot-cpp.' + env['platform'] + '.release.' + env['bits']])

env.VariantDir('obj', 'src')
env.SharedLibrary('bin/kdanmaku.' + env['platform'] + '.' + env['bits'], Glob('obj/*.cpp'))

Help(opts.GenerateHelpText(env))