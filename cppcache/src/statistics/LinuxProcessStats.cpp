/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ace/Thread_Mutex.h>
#include <ace/Singleton.h>

#include <geode/geode_globals.hpp>

#include "LinuxProcessStats.hpp"
#include "HostStatHelperLinux.hpp"
#include "../Assert.hpp"

namespace apache {
namespace geode {
namespace statistics {

LinuxProcessStats::LinuxProcessStats(GeodeStatisticsFactory* statFactory,
                                     int64_t pid, const char* name) {
  // Create Statistics Type
  createType(statFactory);

  // Create Statistics
  this->stats = statFactory->createOsStatistics(m_statsType, name, pid);
  GF_D_ASSERT(this->stats != nullptr);

// Refresh Stats Values
#if defined(_LINUX)
  HostStatHelperLinux::refreshProcess(this);
#endif  // if defined(_LINUX)
}

void LinuxProcessStats::createType(StatisticsFactory* statFactory) {
  try {
    StatisticDescriptor** statDescriptorArr = new StatisticDescriptor*[6];
    statDescriptorArr[0] = statFactory->createIntGauge(
        "imageSize", "The size of the process's image in megabytes.",
        "megabytes");

    statDescriptorArr[1] = statFactory->createIntGauge(
        "rssSize", "The size of the process's resident set size in megabytes.",
        "megabytes");

    statDescriptorArr[2] = statFactory->createIntCounter(
        "userTime", "The os statistic for the process cpu usage in user time",
        "jiffies");

    statDescriptorArr[3] = statFactory->createIntCounter(
        "systemTime",
        "The os statistic for the process cpu usage in system time", "jiffies");

    statDescriptorArr[4] = statFactory->createIntGauge(
        "hostCpuUsage", "The os statistic for the host cpu usage",
        "percent cpu");

    statDescriptorArr[5] = statFactory->createIntGauge(
        "threads", "Number of threads currently active in this process.",
        "threads");

    try {
      m_statsType = statFactory->createType("LinuxProcessStats",
                                            "Statistics for a Linux process.",
                                            statDescriptorArr, 6);
    } catch (Exception&) {
      m_statsType = statFactory->findType("LinuxProcessStats");
    }
    if (m_statsType == nullptr) {
      throw OutOfMemoryException("LinuxProcessStats::createType: out memory");
    }
    imageSizeINT = m_statsType->nameToId("imageSize");
    rssSizeINT = m_statsType->nameToId("rssSize");
    userTimeINT = m_statsType->nameToId("userTime");
    systemTimeINT = m_statsType->nameToId("systemTime");
    hostCpuUsageINT = m_statsType->nameToId("hostCpuUsage");
    threadsINT = m_statsType->nameToId("threads");
  } catch (IllegalArgumentException&) {
    GF_D_ASSERT(false);
    throw;
  }
}

int64_t LinuxProcessStats::getProcessSize() {
  return static_cast<int64_t>(stats->getInt(rssSizeINT));
}

int32_t LinuxProcessStats::getCpuUsage() {
  return stats->getInt(hostCpuUsageINT);
}

int64_t LinuxProcessStats::getCPUTime() { return stats->getInt(userTimeINT); }

int32_t LinuxProcessStats::getNumThreads() { return stats->getInt(threadsINT); }

int64_t LinuxProcessStats::getAllCpuTime() {
  return ((stats->getInt(userTimeINT)) + (stats->getInt(systemTimeINT)));
}

void LinuxProcessStats::close() {
  if (stats != nullptr) {
    stats->close();
  }
}

LinuxProcessStats::~LinuxProcessStats() {
  m_statsType = nullptr;
  stats = nullptr;
}

}  // namespace statistics
}  // namespace geode
}  // namespace apache
