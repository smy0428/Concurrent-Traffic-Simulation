#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    //wait for and receive new messages and pull them from the queue using move semantics. 
  	std::unique_lock<std::mutex> lck(_mtx);
  	_cond.wait(lck, [this] { return !_queue.empty(); });
  	
  	T msg = std::move(_queue.front());
  	_queue.pop_front();
  	return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // use the mechanisms std::lock_guard<std::mutex> as well as _condition.notify_one() 
    // to add a new message to the queue and afterwards send a notification.
  	std::lock_guard<std::mutex> lck(_mtx);
  	_queue.push_back(std::move(msg));
  	_cond.notify_one();
}



TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // an infinite while-loop runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
  	while (true)
    {
      	if (_mQueue.receive() == TrafficLightPhase::green ) { return; }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

// toggle the current phase of the traffic light between red and green
void TrafficLight::setCurrentPhase(TrafficLightPhase &phase)
{
  	_currentPhase = phase;
}

void TrafficLight::simulate()
{
  	// launch cycleThroughPhases function in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
  	
  	// init stop watch
  	auto lastUpdate = std::chrono::system_clock::now();
    long randomMin = 4;
  	long randomMax = 6;
  
  	// randomly generate time between 4s to 6s and scale up 1000 times
    auto randCycleDuration = [randomMin, randomMax](){ return rand() % (randomMax*1000 - randomMin*1000 + 1) + randomMin*1000; };
  	long cycleDuration = randCycleDuration();
  	while (true)
    {
    	// sleep at every iteration to reduce CPU usage
      	std::this_thread::sleep_for(std::chrono::milliseconds(1));
      	
      	long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>
        	 (std::chrono::system_clock::now() - lastUpdate).count();

      	if (timeSinceLastUpdate >= cycleDuration)
        {
          	TrafficLightPhase phase = getCurrentPhase();
          	if (phase == TrafficLightPhase::red)
            {
              	phase = TrafficLightPhase::green;
            }
          	else
            {
              	phase = TrafficLightPhase::red;
            }
          	setCurrentPhase(phase);
          	cycleDuration = randCycleDuration();
          	// send the status to the messageQueue
      		_mQueue.send(std::move(phase));
          	// rest stop watch for next while loop
      		lastUpdate = std::chrono::system_clock::now();
        }
    }
}
