#
# Copyright (C) 2020 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

project(neo_page_fault_manager_tests)

add_executable(neo_page_fault_manager_tests
  $<TARGET_OBJECTS:igdrcl_libult>
  $<TARGET_OBJECTS:igdrcl_libult_cs>
  $<TARGET_OBJECTS:igdrcl_libult_env>
  $<TARGET_OBJECTS:mock_gmm>
  ${NEO_SOURCE_DIR}/runtime/aub/aub_stream_interface.cpp
  ${NEO_SOURCE_DIR}/unit_tests/libult/os_interface.cpp
  ${NEO_SOURCE_DIR}/unit_tests/ult_configuration.cpp
)

append_sources_from_properties(CPU_PAGE_FAULT_MANAGER_TESTS  NEO_CORE_CPU_PAGE_FAULT_MANAGER_TESTS)
append_sources_from_properties(CPU_PAGE_FAULT_MANAGER_WINDOWS_TESTS  NEO_CORE_PAGE_FAULT_MANAGER_WINDOWS_TESTS)
append_sources_from_properties(CPU_PAGE_FAULT_MANAGER_LINUX_TESTS  NEO_CORE_PAGE_FAULT_MANAGER_LINUX_TESTS)

target_sources(neo_page_fault_manager_tests PRIVATE ${CPU_PAGE_FAULT_MANAGER_TESTS})

if(WIN32)
  target_sources(neo_page_fault_manager_tests PRIVATE
  ${CPU_PAGE_FAULT_MANAGER_WINDOWS_TESTS}
  ${NEO_SOURCE_DIR}/runtime/os_interface/windows/wddm/wddm_create.cpp
)
endif()

if(UNIX)
  target_sources(neo_page_fault_manager_tests PRIVATE
  ${CPU_PAGE_FAULT_MANAGER_LINUX_TESTS}
  )
endif()

target_link_libraries(neo_page_fault_manager_tests ${NEO_MOCKABLE_LIB_NAME} igdrcl_mocks gmock-gtest ${IGDRCL_EXTRA_LIBS})

target_include_directories(neo_page_fault_manager_tests PRIVATE
  ${NEO_SOURCE_DIR}/core/unit_tests/test_macros${BRANCH_DIR_SUFFIX}
  ${NEO_SOURCE_DIR}/unit_tests/gen_common${BRANCH_DIR_SUFFIX}
)

create_project_source_tree(neo_page_fault_manager_tests)

add_custom_target(run_page_fault_manager_tests ALL DEPENDS unit_tests neo_page_fault_manager_tests)
add_custom_command(
  TARGET run_page_fault_manager_tests
  POST_BUILD
  COMMAND WORKING_DIRECTORY ${TargetDir}
  COMMAND echo Target Directory is: ${TargetDir}
  COMMAND echo Running page fault manager tests
  COMMAND neo_page_fault_manager_tests
)
add_dependencies(run_unit_tests run_page_fault_manager_tests)

set_target_properties(neo_page_fault_manager_tests PROPERTIES FOLDER ${TEST_PROJECTS_FOLDER})
set_target_properties(run_page_fault_manager_tests PROPERTIES FOLDER ${TEST_PROJECTS_FOLDER})
