#include <chrono>
#include <iostream>
#include <vector>
//точка времени. Временная точка состоит из часов и продолжительности времени
//template <typename Clock, typename Duration= typename Clock::duration> class Time_point;
//продолжительность времени - это разница между двумя точками времени
//template <typename Rep, typename Period = std::ratio<1>> class duration;
// часы состоят из точки отсчёта и тика

void chronoex(){
    // время прошедшее с эпохи
    auto timeNow = std::chrono::system_clock::now();  // time_point, точка времени
    auto duration = timeNow.time_since_epoch();      // длительность
    std::cout << duration.count() << "ns" << std::endl;

    typedef std::chrono::duration<double> MySecondTick;
    MySecondTick mySecond(duration);
    std::cout << mySecond.count() << "s" << std::endl;

    const int minute = 60;
    //using MyMinuteTick = std::chrono::duration<double, <minute>>;
    //typedef std::chrono::duration<double, <minute>> MyMinuteTick;
    typedef std::chrono::duration<double, std::ratio<minute>> MyMinuteTick;

    MyMinuteTick myMinute(duration);
    std::cout << myMinute.count() << "m" << std::endl;

    typedef std::chrono::duration<long long, std::ratio<1>> MySecondTick2;
    MySecondTick2 aSecond(1);
    std::chrono::milliseconds milli(aSecond);
    std::cout << milli.count() << " milli" << std::endl;;
}

void performanceMeasurement(){
    std::vector<int> myBigVec(10000000, 2011);
    std::vector<int> myEmptyVec1;

    auto begin = std::chrono::high_resolution_clock::now();
    myEmptyVec1 = myBigVec;
    auto end = std::chrono::high_resolution_clock::now() - begin;
    auto timeInSeconds = std::chrono::duration<double>(end).count();
    std::cout << timeInSeconds << '\n';
}

void currentTime(){
    auto now = std::chrono::system_clock::now();
    std::cout << "The current time is " << now << "UTC\n"; // обязательно -std=c++20
    auto currentYear = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(now)).year();
    std::cout << "The current year is " << currentYear << '\n';
    auto h = std::chrono::floor<std::chrono::hours>(now) - std::chrono::sys_days(std::chrono::January/1/currentYear);//"January/1/" 
    std::cout << "It has been " << h << " since New Years!\n";
    auto birthOfChrist = std::chrono::year_month_weekday(std::chrono::sys_days(std::chrono::January/01/0000));
    std::cout << "Weekday: " << birthOfChrist.weekday() << '\n';
}

int main(){
    currentTime();
    //performanceMeasurement();
    //chronoex();
    return 0;
}