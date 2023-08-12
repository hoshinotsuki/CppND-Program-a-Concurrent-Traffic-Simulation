#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

// MessageQueue class
template <class T> class MessageQueue {
public:
  void send(T &&msg);
  T receive();

private:
  std::deque<T> _queue;
  std::condition_variable _condition;
  std::mutex _mutex;
};

enum TrafficLightPhase { red, green };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();
  //~TrafficLight();

  // getters / setters
  TrafficLightPhase getCurrentPhase();

  // typical behaviour methods
  void waitForGreen();
  void simulate();

private:
  // typical behaviour methods
  void cycleThroughPhases();
  TrafficLightPhase _currentPhase;

  // type MessageQueue for messages of type TrafficLightPhase
  MessageQueue<TrafficLightPhase> _messages;
};

#endif