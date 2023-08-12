#include "TrafficLight.h"
#include <future>
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // wait for and receive new messages
  std::unique_lock<std::mutex> ulock(_mutex);
  _condition.wait(ulock, [this] { return !_queue.empty(); });

  // pull them from the queue using move semantics.
  T msg = std::move(_queue.front());
  _queue.pop_front();

  // return The received object
  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // lock the current thread to avoid data race
  std::lock_guard<std::mutex> lock(_mutex);
  // add a new message to the queue
  _queue.push_back(std::move(msg));
  // send a notification to wake up the consumer
  _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  // an infinite while-loop runs and repeatedly calls the receive function on
  // the message queue. Once it receives TrafficLightPhase::green, the method
  // returns.
  while (true) {
    if (_messages.receive() == TrafficLightPhase::green)
      return;
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  // started a private member function in a thread when the public method
  // „simulate“ is called.
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

  // set the clycle duration
  int randCycleDuration = rand() % (6000 - 4000 + 1) + 4000;

  // Init stop watch
  std::chrono::time_point<std::chrono::system_clock> lastUpdate =
      std::chrono::system_clock::now();

  // infinite loop
  while (true) {
    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // compute time difference to stop watch
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();

    if (timeSinceLastUpdate >= randCycleDuration) {
      // reset the clycle duration at each iteration
      randCycleDuration = rand() % (6000 - 4000 + 1) + 4000;

      // toggle between red and green phase
      _currentPhase = (_currentPhase == red) ? green : red;

      // push each new TrafficLightPhase into it by calling send with move
      // semantics.
      _messages.send(std::move(_currentPhase));

      // reset stop watch for next cycle
      lastUpdate = std::chrono::system_clock::now();
    }
  }
}
