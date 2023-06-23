set(srcs                
  "src/fft.c"
)
set(include_dirs        
  "src/include/"
)
idf_component_register(
  SRCS ${srcs}
  INCLUDE_DIRS ${include_dirs}
)