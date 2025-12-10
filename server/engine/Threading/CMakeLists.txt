# Threading module CMakeLists.txt
# Contains all threading utilities (header-only templates)

set(THREADING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeQueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeQueue.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeEntityManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeEntityManager.tpp
)

# Export headers to parent scope
set(THREADING_MODULE_HEADERS ${THREADING_HEADERS} PARENT_SCOPE)
