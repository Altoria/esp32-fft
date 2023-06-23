set(srcs                
  "components/fft/fft.c"
)
set(include_dirs        
  "components/fft/include/"
)
idf_component_register(
  SRCS ${srcs}
  INCLUDE_DIRS ${include_dirs}
)