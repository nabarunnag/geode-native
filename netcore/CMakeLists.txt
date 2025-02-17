# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project(netcore LANGUAGES NONE)

option(INCLUDE_DOTNET_CORE "Build .NET Core client." ON)

find_program(DOTNET dotnet)

if(DOTNET AND INCLUDE_DOTNET_CORE)
  add_custom_target(netcore ALL COMMAND ${DOTNET} build --configuration $<CONFIG> WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/NetCore DEPENDS apache-geode-c VERBATIM)
  add_custom_target(netcore-test ALL COMMAND ${DOTNET} build --configuration $<CONFIG> WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/NetCore.Test DEPENDS netcore VERBATIM)

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/NetCore/bin/$<CONFIG>/netcoreapp3.1/Apache.Geode.NetCore.dll DESTINATION ./lib)
endif()
