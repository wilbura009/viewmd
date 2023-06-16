viewmd_sources = [
  'main.c',
  'viewmd-application.c',
  'viewmd-window.c',
]

viewmd_deps = [
  dependency('gtk+-3.0'),
  dependency('webkit2gtk-4.1'),
]

viewmd_sources += gnome.compile_resources('viewmd-resources',
  'viewmd.gresource.xml',
  c_name: 'viewmd'
)

executable('viewmd', viewmd_sources,
  dependencies: viewmd_deps,
       install: true,
)
