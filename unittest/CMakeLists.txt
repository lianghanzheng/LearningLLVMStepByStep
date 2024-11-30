include(FetchContent)

if (NOT GTEST_LOCAL)
FetchContent_Declare(
  google_test
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
else()
FetchContent_Declare(
  google_test
  URL ${GTEST_LOCAL}
)

endif()

FetchContent_MakeAvailable(google_test)

add_subdirectory(Lexer)