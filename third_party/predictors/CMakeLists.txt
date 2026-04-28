if (ENABLE_PREDICT)
  add_subdirectory(model_selection)

  # ### DEPRECATED ###
  # if (${PREDICTOR_IMPL} STREQUAL "torchscript")
  #   add_subdirectory(torchscript)
  # else
  if(PREDICTOR_IMPL)
    if (${PREDICTOR_IMPL} STREQUAL "onnx")
      add_subdirectory(onnx)
    elseif (${PREDICTOR_IMPL} STREQUAL "llama_cpp")
      add_subdirectory(llama_cpp)  
    endif()
  else ()
    if (NOT ENABLE_LLM_API)
      message(FATAL_ERROR "Unknown predictor implementation requested: `${PREDICTOR_IMPL}`! Set with `PREDICTOR_IMPL=<torchscript|onnx|llama_cpp>`.")
    endif()
  endif ()

  if (ENABLE_LLM_API)
    add_subdirectory(llm_api)  
  endif()
endif()

