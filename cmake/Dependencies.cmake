function(find_dependencies)
  find_lib(glaze)
  find_lib(Boost)
  find_lib(spdlog)

  if(BUILD_TESTING)
    find_lib(GTest)
  endif()
  if(BUILD_BENCHMARKS)
    find_lib(benchmark)
  endif()

endfunction()
