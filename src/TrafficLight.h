#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;
enum TrafficLightPhase
{
	red,
  	green,
};


template <class T>
class MessageQueue
{
public:
	void send(T &&msg);
    T receive();
private:
	std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mtx;
};

// class „TrafficLight“ is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

  
class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
	TrafficLight();
    // getters / setters
	TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(TrafficLightPhase &phase);
    // typical behaviour methods
	void waitForGreen();
    void simulate() override;
private:
    // typical behaviour methods
	void cycleThroughPhases();
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _mQueue;
};

#endif