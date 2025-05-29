boost_dep = dependency('boost', modules: ['system'], required: true)
ssl_dep = dependency('OpenSSL', required: true)

sources = files(
    'main.cpp',
    'Session.cpp',
    'http_utils.cpp',
    'RequestHandler.cpp'
)

executable('http-server', sources, dependencies: [boost_dep, ssl_dep])