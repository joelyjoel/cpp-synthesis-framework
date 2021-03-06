#include "Process.h"
#include <iostream>

int processCount = 0;

// Constructors
Process::Process()
  : process_id(processCount++)
{}

// Identification
std::string Process::name() {  
  return std::to_string(process_id) + "_" + process_type();
} 

void Process::addInlet(Inlet& inlet) {
  inlets.push_back(&inlet);
  inlet.owner = this;
}

void Process::addOutlet(Outlet& outlet) {
  outlets.push_back(&outlet);
  outlet.owner = this;
}

Outlet* Process::mainOutlet() {
  return outlets[0];
}

Inlet* Process::mainInlet() {
  return inlets[0];
}

Inlet* Process::firstFreeInlet() {
  for(Inlet* inlet : inlets) {
    if(inlet -> isFree())
      return inlet;
  }

  // Otherwise
  throw name() + " has no free inlets (call to firstFreeInlet)";
}

std::set<Process*>* Process::dependentProcesses() {
  std::set<Process*> * list = new std::set<Process*>;
  for(Outlet* outlet : outlets) {
    for(Inlet* inlet : outlet->connectedTo) {
      list->insert(inlet->owner);
    }
  }

  return list;
}

std::set<Process*>* Process::neighbours() {
  std::set<Process*> * list = new std::set<Process*>;

  for(Outlet* outlet : outlets) {
    for(Inlet* inlet : outlet->connectedTo) {
      list->insert(inlet->owner);
    }
  }

  for(Inlet* inlet : inlets) {
    if(inlet->isConnected)
      list->insert(inlet->connectedTo->owner);
  }

  return list;
}


void Process::process() {
}

int Process::numberOfInlets() {
  return inlets.size();
}

int Process::numberOfOutlets() {
  return outlets.size();
}

int Process::getPriority(int generation) {
  if(isFinalProcess)
    return 0;

  if(generation == priority_generation) {
    return priority;
  } else {
    recalculatePriority(generation);

    return priority;
  }
}


void Process::recalculatePriority(int generation) {
  priority_generation = generation;
  priority = 0;
  int max = 0;
  for(Process* p : *dependentProcesses()) {
    if(p == this)
      continue;

    int pPriority = p -> getPriority(generation);
    if(pPriority + 1 > max)
      max = pPriority + 1;
  }

  

  priority = max;
}


bool sort_by_priority(Process* l, Process* r) {
  return l->priority > r->priority;
}

void sortProcesses(std::vector<Process*>& processes) {
  std::sort(
      processes.begin(), 
      processes.end(), 
      sort_by_priority
  );
}
