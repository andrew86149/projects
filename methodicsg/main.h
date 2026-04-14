#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <string>

void conditionVariableBlock();
void threadLocal();
void readerWriterLock();
void deadlock();
void threadWithoutJoin();
void createThread();
void runBench();

